import EngineHandling.ChessEngine
import EngineHandling.EngineDescriptor
import EngineHandling.EnginePlayer

/**
 * Created by vabi on 29.05.17.
 */

fun main(args: Array<String>) {

    var defender = EngineDescriptor("/home/vabi/code/chessEngine/Cpp/uciDefenderRelease/Vabi", "/home/vabi", "Defender")
    var challenger = EngineDescriptor("/home/vabi/code/chessEngine/Cpp/uciChallengeRelease/Vabi", "/home/vabi", "Challenger")
    val player = EnginePlayer(defender, challenger, "rnbqk2r/ppp1bppp/4pn2/3p2B1/2PP4/2N5/PP2PPPP/R2QKBNR w KQkq - 0 5", 7, 1000)
    var res = player.play()
    println(res.res)

    /*for(move in res.moves) {
        println(move)
    }*/
    /*val engine = ChessEngine("/home/vabi/code/chessEngine/Cpp/Release/Vabi", "/home/vabi")
    var result = engine.search(7)
    println(result.bestMove)
    println(result.eval)
    engine.close()*/
}
