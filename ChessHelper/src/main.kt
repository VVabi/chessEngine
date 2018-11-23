import EngineHandling.ChessEngine
import EngineHandling.getStockfishEval
import GUI.launchGui
import PositionHelper.PositionDescriptor
import Tools.getLos
import Tools.toCsvLine
import java.io.File
import java.net.ServerSocket
import java.io.InputStreamReader
import java.io.BufferedReader
import java.io.FileWriter


/**
 * Created by vabi on 29.05.17.
 */

fun main(args: Array<String>) {


    /*var f = File("/home/vabi/test.txt")

    var lines = f.readLines()



    var csvstring = StringBuilder()
    val localEngine = ChessEngine("/home/vabi/code/chessEngine/Cpp/Release/Vabi", "/home/vabi")
    var answer = localEngine.getEvalComponents()
    csvstring.append(toCsvLine(answer).header)
    csvstring.append("stockfish")
    csvstring.append("\n")
    for (line in lines) {

        println(line)
        localEngine.setPosition(line, mutableListOf())
        if (!localEngine.isPositionQuiet()) {
            continue
        }

        var stockfisheval = getStockfishEval(line)

        if ((stockfisheval > 100) || (stockfisheval < -100)) {
            continue
        }
        var answer = localEngine.getEvalComponents()

        csvstring.append(toCsvLine(answer).line)
        csvstring.append((stockfisheval/100.0).toString())
        csvstring.append("\n")
    }


    var fileWriter = FileWriter("/home/vabi/chess2.csv")
    fileWriter.write(csvstring.toString())
    fileWriter.flush()
    fileWriter.close()*/


    launchGui()


}
