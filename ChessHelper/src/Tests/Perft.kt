package Tests

import EngineHandling.ChessEngine
import java.io.BufferedReader
import java.io.File
import java.io.FileReader

/**
 * Created by vabi on 31.05.17.
 */


private data class PerftTestData(val fen: String, val depth: Int, val numNodes: Long);

fun runPerftTests(engine: ChessEngine, debugEngine: ChessEngine): Boolean {

    var data = ArrayList<PerftTestData>();
    val localData = java.util.ArrayList<String>()
    val dataFile = File("src/Tests/perftTests.txt")
    var lines = dataFile.readLines()


    for(line in lines) {
       localData.add(line)
       if (localData.size == 3) {
           data.add(PerftTestData(localData[0], Integer.parseInt(localData[1]), java.lang.Long.parseLong(localData[2])))
           localData.clear()
       }
    }

    var res = true

    if(localData.size != 0) {
        println("Input file ill-formatted!")
        res = false
    }

    if(data.size == 0) {
        println("No perft test data found!")
        res = false
    }

    for(item in data) {
        println("Testing "+item.fen+" to depth "+item.depth);
        engine.setPosition(item.fen, listOf())
        println("Required nodes "+item.numNodes)
        var perftNodes = engine.runPerft(item.depth)
        println("Actual nodes "+perftNodes)

        if(perftNodes != item.numNodes) {
            println("Wrong perft")
            res = false
            break
        }

    }

    return res
}