#!/usr/bin/env python3
"""Self-play match between two UCI engines, to measure whether a change is an improvement.

Engine 1 is the candidate, engine 2 the baseline: all results, Elo and LOS are
reported from engine 1's point of view. Every opening is played twice with
colors swapped, so the number of games is always even.

Example:
    python selfplay.py --engine1 ../../Cpp/Release/Vabi --engine2 /path/to/old/Vabi \\
        --tc 10+0.1 --games 400 --concurrency 6 --sprt 0 5
"""

import argparse
import concurrent.futures
import datetime
import logging
import math
import random
import sys
import threading
import time
from dataclasses import dataclass, field
from pathlib import Path
from typing import Dict, List, Optional, Tuple

import chess
import chess.engine
import chess.pgn

REPO_ROOT = Path(__file__).resolve().parents[2]
DEFAULT_OPENINGS = REPO_ROOT / "ChessHelper" / "src" / "Tools" / "openingPositions.txt"


# ---------------------------------------------------------------------------
# Configuration
# ---------------------------------------------------------------------------

@dataclass
class EngineSpec:
    path: Path
    name: str
    options: Dict[str, str]


@dataclass
class TimeControl:
    """Exactly one of (base, movetime, depth, nodes) is set."""
    base: Optional[float] = None       # seconds
    increment: float = 0.0             # seconds
    movetime: Optional[float] = None   # seconds
    depth: Optional[int] = None
    nodes: Optional[int] = None

    def describe(self) -> str:
        if self.base is not None:
            return f"{self.base:g}+{self.increment:g}"
        if self.movetime is not None:
            return f"movetime {self.movetime:g}s"
        if self.depth is not None:
            return f"depth {self.depth}"
        return f"nodes {self.nodes}"

    def pgn_tag(self) -> str:
        if self.base is not None:
            return f"{self.base:g}+{self.increment:g}"
        if self.movetime is not None:
            return f"{self.movetime:g}/move"
        return "-"


def parse_tc(text: str) -> TimeControl:
    """Parse 'BASE+INC' or 'BASE' (seconds), e.g. '10+0.1', '60'."""
    try:
        base, _, inc = text.partition("+")
        tc = TimeControl(base=float(base), increment=float(inc) if inc else 0.0)
    except ValueError:
        raise argparse.ArgumentTypeError(f"invalid time control '{text}', expected BASE+INC in seconds")
    if tc.base <= 0 or tc.increment < 0:
        raise argparse.ArgumentTypeError("time control base must be > 0 and increment >= 0")
    return tc


def parse_option(text: str) -> Tuple[str, str]:
    name, sep, value = text.partition("=")
    if not sep or not name:
        raise argparse.ArgumentTypeError(f"invalid engine option '{text}', expected NAME=VALUE")
    return name, value


def default_name(path: Path) -> str:
    # Builds of this engine are usually <preset>/Vabi, so include the directory
    return f"{path.parent.name}/{path.name}" if path.parent.name else path.name


def load_openings(path: Path) -> List[chess.Board]:
    """Read FEN or EPD lines; lines starting with '%' or '#' and blank lines are skipped."""
    boards = []
    with open(path) as f:
        for lineno, line in enumerate(f, 1):
            line = line.strip()
            if not line or line[0] in "%#":
                continue
            try:
                board = chess.Board(line)
            except ValueError:
                try:
                    board, _ = chess.Board.from_epd(line)
                except ValueError:
                    print(f"warning: skipping invalid position {path}:{lineno}: {line}", file=sys.stderr)
                    continue
            if board.is_valid() and board.outcome() is None:
                boards.append(board)
    if not boards:
        sys.exit(f"error: no usable opening positions in {path}")
    return boards


# ---------------------------------------------------------------------------
# Statistics
# ---------------------------------------------------------------------------

def logistic_score(elo: float) -> float:
    return 1.0 / (1.0 + 10.0 ** (-elo / 400.0))


def score_to_elo(score: float) -> float:
    score = min(max(score, 1e-9), 1 - 1e-9)
    return -400.0 * math.log10(1.0 / score - 1.0)


@dataclass
class Stats:
    wins: int = 0
    losses: int = 0
    draws: int = 0
    terminations: Dict[str, int] = field(default_factory=dict)

    @property
    def games(self) -> int:
        return self.wins + self.losses + self.draws

    @property
    def score(self) -> float:
        return (self.wins + 0.5 * self.draws) / self.games if self.games else 0.5

    def variance(self) -> float:
        """Per-game variance of the score."""
        n, s = self.games, self.score
        return (self.wins * (1 - s) ** 2 + self.draws * (0.5 - s) ** 2 + self.losses * s ** 2) / n

    def elo(self) -> Tuple[float, float]:
        """Elo difference and half-width of its 95% confidence interval."""
        if not self.games:
            return 0.0, float("inf")
        s = self.score
        stderr = math.sqrt(self.variance() / self.games)
        lo, hi = score_to_elo(s - 1.96 * stderr), score_to_elo(s + 1.96 * stderr)
        return score_to_elo(s), (hi - lo) / 2

    def los(self) -> float:
        """Likelihood of superiority: probability that engine 1 is stronger."""
        decisive = self.wins + self.losses
        if not decisive:
            return 0.5
        return 0.5 + 0.5 * math.erf((self.wins - self.losses) / math.sqrt(2.0 * decisive))

    def llr(self, elo0: float, elo1: float) -> float:
        """Log-likelihood ratio of H1 (elo1) vs H0 (elo0), normal approximation (GSPRT)."""
        if not self.games or not self.wins + self.losses:
            return 0.0
        var = self.variance()
        if var <= 0:
            return 0.0
        s0, s1 = logistic_score(elo0), logistic_score(elo1)
        return (s1 - s0) * (2 * self.score - s0 - s1) * self.games / (2 * var)


@dataclass
class Sprt:
    elo0: float
    elo1: float
    alpha: float
    beta: float

    @property
    def lower(self) -> float:
        return math.log(self.beta / (1 - self.alpha))

    @property
    def upper(self) -> float:
        return math.log((1 - self.beta) / self.alpha)


# ---------------------------------------------------------------------------
# Playing games
# ---------------------------------------------------------------------------

@dataclass
class GameResult:
    index: int
    white: str
    black: str
    result: str              # "1-0", "0-1", "1/2-1/2"
    termination: str
    engine1_score: float     # 1, 0.5 or 0
    game: chess.pgn.Game


class EnginePool:
    """One pair of engine processes per worker thread, restarted after crashes."""

    def __init__(self, specs: List[EngineSpec], hash_mb: Optional[int]):
        self.specs = specs
        self.hash_mb = hash_mb
        self.local = threading.local()
        self.lock = threading.Lock()
        self.all_engines: List[chess.engine.SimpleEngine] = []
        self.closed = False

    def get(self, which: int) -> chess.engine.SimpleEngine:
        engines = getattr(self.local, "engines", None)
        if engines is None:
            engines = self.local.engines = [None, None]
        if engines[which] is None:
            engines[which] = self._start(self.specs[which])
        return engines[which]

    def discard(self, which: int) -> None:
        engines = getattr(self.local, "engines", None)
        if engines and engines[which] is not None:
            self._close(engines[which])
            engines[which] = None

    def _start(self, spec: EngineSpec) -> chess.engine.SimpleEngine:
        engine = chess.engine.SimpleEngine.popen_uci(str(spec.path))
        options = dict(spec.options)
        if self.hash_mb is not None and "Hash" in engine.options:
            options.setdefault("Hash", str(self.hash_mb))
        if options:
            engine.configure(options)
        with self.lock:
            if not self.closed:
                self.all_engines.append(engine)
                return engine
        engine.quit()
        raise chess.engine.EngineTerminatedError("engine pool is shut down")

    def _close(self, engine: chess.engine.SimpleEngine) -> None:
        with self.lock:
            if engine in self.all_engines:
                self.all_engines.remove(engine)
        try:
            engine.quit()
        except Exception:
            try:
                engine.close()
            except Exception:
                pass

    def close_all(self) -> None:
        with self.lock:
            self.closed = True
            engines = list(self.all_engines)
        for engine in engines:
            self._close(engine)


def play_game(index: int, opening: chess.Board, engine1_white: bool, pool: EnginePool,
              specs: List[EngineSpec], tc: TimeControl, time_margin: float,
              max_moves: int, stop: threading.Event) -> Optional[GameResult]:
    if stop.is_set():
        return None

    # which[color] is the index of the engine playing that color
    which = {chess.WHITE: 0 if engine1_white else 1, chess.BLACK: 1 if engine1_white else 0}
    board = opening.copy(stack=False)
    clocks = {chess.WHITE: tc.base, chess.BLACK: tc.base}
    game_id = object()  # new object per game -> python-chess sends ucinewgame
    result, termination = None, None

    while result is None:
        if stop.is_set():
            return None

        outcome = board.outcome(claim_draw=True)
        if outcome is not None:
            result = outcome.result()
            termination = outcome.termination.name.lower().replace("_", " ")
            break
        if max_moves and board.fullmove_number - opening.fullmove_number >= max_moves:
            result, termination = "1/2-1/2", "move limit"
            break

        side = board.turn
        if tc.base is not None:
            limit = chess.engine.Limit(white_clock=clocks[chess.WHITE], black_clock=clocks[chess.BLACK],
                                       white_inc=tc.increment, black_inc=tc.increment)
        else:
            limit = chess.engine.Limit(time=tc.movetime, depth=tc.depth, nodes=tc.nodes)

        loser = "0-1" if side == chess.WHITE else "1-0"
        try:
            engine = pool.get(which[side])
            start = time.monotonic()
            played = engine.play(board, limit, game=game_id)
            elapsed = time.monotonic() - start
        except (chess.engine.EngineError, chess.engine.EngineTerminatedError, OSError,
                TimeoutError) as e:
            if stop.is_set():
                return None
            pool.discard(which[side])
            result, termination = loser, f"engine error ({specs[which[side]].name}: {e})"
            break

        if tc.base is not None:
            clocks[side] -= elapsed
            if clocks[side] < -time_margin:
                result, termination = loser, "time forfeit"
                break
            clocks[side] = max(clocks[side], 0.0) + tc.increment

        move = played.move
        if move is None or move not in board.legal_moves:
            result, termination = loser, f"illegal move {move}"
            break
        board.push(move)

    white, black = specs[which[chess.WHITE]].name, specs[which[chess.BLACK]].name
    white_score = {"1-0": 1.0, "0-1": 0.0}.get(result, 0.5)
    engine1_score = white_score if engine1_white else 1.0 - white_score

    game = chess.pgn.Game.from_board(board)
    game.headers.update({
        "Event": "Vabi self-play",
        "Date": datetime.date.today().strftime("%Y.%m.%d"),
        "Round": str(index + 1),
        "White": white,
        "Black": black,
        "Result": result,
        "TimeControl": tc.pgn_tag(),
        "Termination": termination,
    })
    return GameResult(index, white, black, result, termination, engine1_score, game)


# ---------------------------------------------------------------------------
# Main
# ---------------------------------------------------------------------------

def format_status(stats: Stats, sprt: Optional[Sprt]) -> str:
    elo, err = stats.elo()
    text = (f"+{stats.wins} -{stats.losses} ={stats.draws}  score {100 * stats.score:.1f}%  "
            f"Elo {elo:+.1f} ± {err:.1f}  LOS {100 * stats.los():.1f}%")
    if sprt:
        text += f"  LLR {stats.llr(sprt.elo0, sprt.elo1):+.2f} ({sprt.lower:+.2f}, {sprt.upper:+.2f})"
    return text


def parse_args() -> argparse.Namespace:
    p = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)

    eng = p.add_argument_group("engines")
    eng.add_argument("--engine1", type=Path, required=True, help="candidate engine binary")
    eng.add_argument("--engine2", type=Path, required=True, help="baseline engine binary")
    eng.add_argument("--name1", help="display name of engine 1 (default: <dir>/<binary>)")
    eng.add_argument("--name2", help="display name of engine 2 (default: <dir>/<binary>)")
    eng.add_argument("--option1", type=parse_option, action="append", default=[], metavar="NAME=VALUE",
                     help="UCI option for engine 1 (repeatable)")
    eng.add_argument("--option2", type=parse_option, action="append", default=[], metavar="NAME=VALUE",
                     help="UCI option for engine 2 (repeatable)")
    eng.add_argument("--hash", type=int, default=64, help="hash size in MB for both engines (default: 64)")

    tcg = p.add_argument_group("time control (pick one, default: --tc 10+0.1)")
    tcx = tcg.add_mutually_exclusive_group()
    tcx.add_argument("--tc", type=parse_tc, help="BASE+INC in seconds, e.g. 10+0.1 or 60")
    tcx.add_argument("--movetime", type=float, help="fixed time per move in seconds")
    tcx.add_argument("--depth", type=int, help="fixed search depth per move")
    tcx.add_argument("--nodes", type=int, help="fixed node count per move (if the engine supports it)")
    tcg.add_argument("--time-margin", type=float, default=0.1,
                     help="seconds an engine may overrun its clock before losing on time (default: 0.1)")

    match = p.add_argument_group("match")
    match.add_argument("--games", type=int, default=200, help="number of games, rounded up to even (default: 200)")
    match.add_argument("--concurrency", "-j", type=int, default=1, help="games played in parallel (default: 1)")
    match.add_argument("--openings", type=Path, default=DEFAULT_OPENINGS,
                       help=f"FEN/EPD file of start positions (default: {DEFAULT_OPENINGS.relative_to(REPO_ROOT)})")
    match.add_argument("--random-openings", action="store_true", help="shuffle the openings")
    match.add_argument("--seed", type=int, help="random seed for --random-openings")
    match.add_argument("--max-moves", type=int, default=0,
                       help="adjudicate a draw after this many moves from the opening (default: 0 = off)")
    match.add_argument("--pgn", type=Path, help="append all games to this PGN file")

    sp = p.add_argument_group("SPRT (stop early once a decision is reached)")
    sp.add_argument("--sprt", nargs=2, type=float, metavar=("ELO0", "ELO1"),
                    help="H0: engine 1 is ELO0 stronger, H1: engine 1 is ELO1 stronger, e.g. --sprt 0 5")
    sp.add_argument("--alpha", type=float, default=0.05, help="SPRT false-positive rate (default: 0.05)")
    sp.add_argument("--beta", type=float, default=0.05, help="SPRT false-negative rate (default: 0.05)")

    args = p.parse_args()
    for which in ("engine1", "engine2"):
        path = getattr(args, which).resolve()
        if not path.is_file():
            p.error(f"--{which}: no such file: {path}")
        setattr(args, which, path)
    if args.games < 1 or args.concurrency < 1:
        p.error("--games and --concurrency must be >= 1")
    if args.sprt and args.sprt[0] >= args.sprt[1]:
        p.error("--sprt: ELO0 must be smaller than ELO1")
    return args


def main() -> int:
    args = parse_args()
    # Vabi answers ucinewgame with "Not yet implemented"; don't warn about it
    logging.getLogger("chess.engine").setLevel(logging.ERROR)

    if args.movetime is not None:
        tc = TimeControl(movetime=args.movetime)
    elif args.depth is not None:
        tc = TimeControl(depth=args.depth)
    elif args.nodes is not None:
        tc = TimeControl(nodes=args.nodes)
    else:
        tc = args.tc or parse_tc("10+0.1")

    specs = [
        EngineSpec(args.engine1, args.name1 or default_name(args.engine1), dict(args.option1)),
        EngineSpec(args.engine2, args.name2 or default_name(args.engine2), dict(args.option2)),
    ]
    if specs[0].name == specs[1].name:
        specs[0].name += " (1)"
        specs[1].name += " (2)"
    sprt = Sprt(args.sprt[0], args.sprt[1], args.alpha, args.beta) if args.sprt else None

    openings = load_openings(args.openings)
    if args.random_openings:
        random.Random(args.seed).shuffle(openings)
    num_games = args.games + args.games % 2

    print(f"{specs[0].name} vs {specs[1].name}: {num_games} games, tc {tc.describe()}, "
          f"concurrency {args.concurrency}, {len(openings)} openings from {args.openings}")
    if sprt:
        print(f"SPRT: elo0 {sprt.elo0:g}, elo1 {sprt.elo1:g}, alpha {sprt.alpha:g}, beta {sprt.beta:g}")

    stats = Stats()
    stop = threading.Event()
    pool = EnginePool(specs, args.hash)
    pgn_file = open(args.pgn, "a") if args.pgn else None
    decision = None

    executor = concurrent.futures.ThreadPoolExecutor(max_workers=args.concurrency)
    try:
        futures = [
            executor.submit(play_game, i, openings[(i // 2) % len(openings)], i % 2 == 0, pool, specs, tc,
                            args.time_margin, args.max_moves, stop)
            for i in range(num_games)
        ]
        for future in concurrent.futures.as_completed(futures):
            res = future.result()
            if res is None:
                continue
            if res.engine1_score == 1.0:
                stats.wins += 1
            elif res.engine1_score == 0.0:
                stats.losses += 1
            else:
                stats.draws += 1
            stats.terminations[res.termination] = stats.terminations.get(res.termination, 0) + 1
            if pgn_file:
                print(res.game, file=pgn_file, end="\n\n", flush=True)

            print(f"[{stats.games:>{len(str(num_games))}}/{num_games}] {res.white} - {res.black} "
                  f"{res.result} ({res.termination})  |  {format_status(stats, sprt)}", flush=True)

            if sprt and decision is None:
                llr = stats.llr(sprt.elo0, sprt.elo1)
                if llr >= sprt.upper:
                    decision = f"H1 accepted: {specs[0].name} is likely >= {sprt.elo1:g} Elo stronger"
                elif llr <= sprt.lower:
                    decision = f"H0 accepted: {specs[0].name} is likely not >= {sprt.elo1:g} Elo stronger"
                if decision:
                    stop.set()
    except KeyboardInterrupt:
        print("\ninterrupted, stopping...", file=sys.stderr)
        stop.set()
    finally:
        stop.set()
        executor.shutdown(wait=False, cancel_futures=True)
        pool.close_all()
        executor.shutdown(wait=True)
        if pgn_file:
            pgn_file.close()

    print()
    print(f"Result after {stats.games} games ({specs[0].name} vs {specs[1].name}, tc {tc.describe()}):")
    print(f"  {format_status(stats, sprt)}")
    if stats.terminations:
        print("  " + ", ".join(f"{k}: {v}" for k, v in sorted(stats.terminations.items(), key=lambda kv: -kv[1])))
    if decision:
        print(f"  SPRT {decision}")
    elif sprt:
        print("  SPRT: no decision yet, play more games")
    return 0


if __name__ == "__main__":
    sys.exit(main())
