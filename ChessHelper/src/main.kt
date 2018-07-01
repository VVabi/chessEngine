import EngineHandling.ChessEngine
import EngineHandling.getStockfishEval
import GUI.launchGui
import PositionHelper.PositionDescriptor
import Tools.getLos
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
    for (line in lines) {

        println(line)
        localEngine.setPosition(line, mutableListOf())
        if (!localEngine.isPositionQuiet()) {
            continue
        }

        var stockfisheval = getStockfishEval(line)

        if ((stockfisheval > 200) || (stockfisheval < -200)) {
            continue
        }
        var answer = localEngine.getEvalDetails()
        var values = answer.split(' ')
        csvstring.append("\n")
       // println(answer)
        var first = false
        for (value in values) {
            try {
                var n = Integer.parseInt(value)
                if (first) {
                    csvstring.append(", ")
                }
                first = true
                csvstring.append(n)
            } catch (e: Exception) {

            }
        }

        csvstring.append(", "+stockfisheval.toString())
    }


    var fileWriter = FileWriter("/home/vabi/chess.csv")
    fileWriter.write(csvstring.toString())
    fileWriter.flush()
    fileWriter.close()*/


    launchGui()


}
