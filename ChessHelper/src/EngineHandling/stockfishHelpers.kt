package EngineHandling

import java.io.*

/**
 * Created by vabi on 19.06.17.
 */


fun getStockfishEval(fen: String): Int {
    val processBuilder = ProcessBuilder("stockfish")
    val process = processBuilder.start()
    val reader = BufferedReader(InputStreamReader(process.inputStream))
    val writer = BufferedWriter(OutputStreamWriter(process.outputStream))
    reader.readLine()
    writer.write("position fen "+fen+"\n")
    writer.write("eval\n")
    writer.flush()
    var s = ""
    while((s.length < 16) || !s.substring(0..15).equals("Total Evaluation")) {
        s = reader.readLine()
    }
    var list = s.split(' ')
    process.destroyForcibly()

    var value = (list[2].toDouble()*100).toInt()

    if(list[3] == "(black"){
        value = -value
    }
    return value
}