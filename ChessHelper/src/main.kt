import EngineHandling.ChessEngine
import EngineHandling.getStockfishEval
import GUI.launchGui
import Tools.getLos
import java.io.File

/**
 * Created by vabi on 29.05.17.
 */

fun main(args: Array<String>) {

    println(getStockfishEval("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"))
    val releaseEngine: ChessEngine = ChessEngine("/home/vabi/code/chessEngine/Cpp/Release/Vabi", "")

    val dataFile = File("/home/vabi/quiet.txt")
    val lines = dataFile.readLines()

    for(line in lines) {
        releaseEngine.setPosition(line, listOf())
        var stockEval = getStockfishEval(line)
        var vabiEval  = releaseEngine.getEval()

        if((Math.abs(stockEval-vabiEval) > 50) && (Math.abs(stockEval) < 100)) {
            println(line)
            println(stockEval)
            println(vabiEval)
        }
    }
    launchGui()


    /*var defwins = 0
    var challengewins = 0
    var draws = 0
    var engine = ChessEngine("/home/vabi/code/chessEngine/Cpp/Release/Vabi", "/home/vabi")

    var ret = runPerftTests(engine)
    println(ret)*/



   /* for(i in 1..1000) {
        println(i)
        var defender = EngineDescriptor("/home/vabi/code/chessEngine/Cpp/uciDefenderRelease/Vabi", "/home/vabi", "Defender")
        var challenger = EngineDescriptor("/home/vabi/code/chessEngine/Cpp/uciChallengeRelease/Vabi", "/home/vabi", "Challenger")
        val player = EnginePlayer(defender, challenger, "rnbqk2r/ppp1bppp/4pn2/3p2B1/2PP4/2N5/PP2PPPP/R2QKBNR w KQkq - 0 5", 7)
        var res = player.play()
        println(res.res)

        if(res.res == "DRAW") {
            draws++
        } else if(res.res == defender.name) {
            defwins++
        } else {
            challengewins++;
        }

        println("Def "+defwins+" draws "+draws+" Challenger "+challengewins)


    }*/

    /*for(move in res.moves) {
        println(move)
    }*/
    /*val engine = ChessEngine("/home/vabi/code/chessEngine/Cpp/Release/Vabi", "/home/vabi")
    var result = engine.search(7)
    println(result.bestMove)
    println(result.eval)
    engine.close()*/
}
