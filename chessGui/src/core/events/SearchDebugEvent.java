package core.events;

import core.ChessEvent;

/**
 * Created by vabi on 28.09.16.
 */
public class SearchDebugEvent extends ChessEvent{
    public long nodes;
    public int time;
    public int eval;
    public int depth;
    public String move;

    public SearchDebugEvent(long nodes, int time, int eval, int depth, String move) {
        this.nodes = nodes;
        this.time = time;
        this.eval = eval;
        this.depth = depth;
        this.move = move;
    }
}
