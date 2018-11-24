package EngineHandling

import java.io.*

/**
 * Created by vabi on 19.06.17.
 */


fun getStockfishEval(fen: String, reader: BufferedReader, writer: BufferedWriter): Int {

    reader.readLine()
    writer.write("position fen "+fen+"\n")
    writer.write("eval\n")
    writer.flush()
    var s = ""
    while((s.length < 16) || !s.substring(0..15).equals("Total Evaluation")) {
        s = reader.readLine()
    }
    var list = s.split(' ')


    var value = (list[2].toDouble()*100).toInt()

    if(list[3] == "(black"){
        value = -value
    }
    return value
}