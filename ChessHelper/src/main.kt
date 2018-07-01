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

    var f = File("/home/vabi/test.txt")

    var lines = f.readLines()



    var csvstring = ""

    for (line in lines) {
        val localEngine = ChessEngine("/home/vabi/code/chessEngine/Cpp/Release/Vabi", "/home/vabi")

        localEngine.setPosition(line, mutableListOf())
        if (!localEngine.isPositionQuiet()) {
            println("BÖÖP")
        } else {
            println("DOOO")
        }
        var answer = localEngine.getEvalDetails()
        var values = answer.split(' ')
        csvstring = csvstring+"\n"
       // println(answer)
        var first = false
        for (value in values) {
            try {
                var n = Integer.parseInt(value)
                if (first) {
                    csvstring = csvstring+", "
                }
                first = true
                csvstring=csvstring+n.toString()
            } catch (e: Exception) {

            }
        }
    }


    var fileWriter = FileWriter("/home/vabi/chess.csv")
    fileWriter.write(csvstring)
    fileWriter.flush()
    fileWriter.close()


    launchGui()


}
