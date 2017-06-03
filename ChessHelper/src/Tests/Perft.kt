package Tests

import EngineHandling.ChessEngine
import java.io.File

/**
 * Created by vabi on 31.05.17.
 */


private data class PerftTestData(val fen: String, val depth: Int, val numNodes: Long)

fun runPerftTests(engine: ChessEngine, debugEngine: ChessEngine): Boolean {

    val data = ArrayList<PerftTestData>()
    val localData = java.util.ArrayList<String>()
    val dataFile = File("src/Tests/perftTests.txt")
    val lines = dataFile.readLines()


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

    for((fen, depth, numNodes) in data) {
        println("Testing $fen to depth $depth")
        engine.setPosition(fen, listOf())
        println("Required nodes "+ numNodes)
        val perftNodes = engine.runPerft(depth)
        println("Actual nodes "+perftNodes)

        if(perftNodes != numNodes) {
            println("Wrong perft")
            res = false
            break
        }

    }

    return res
}