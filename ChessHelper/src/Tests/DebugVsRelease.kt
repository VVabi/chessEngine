package Tests

import EngineHandling.ChessEngine
import java.io.File

/**
 * Created by vabi on 01.06.17.
 */


fun testDebugvsRelease(releaseEngine: ChessEngine, debugEngine: ChessEngine): Boolean {

    val dataFile = File("src/Tests/debugVsReleaseNodecountTests.txt")
    val lines = dataFile.readLines()

    for(line in lines) {
        println("Testing "+line)
        debugEngine.setPosition(line, listOf())
        releaseEngine.setPosition(line, listOf())
        val debugSearchOutput = debugEngine.debugSearchOutput(8)
        val releaseSearchOutput = releaseEngine.debugSearchOutput(8)

        if (releaseSearchOutput.size != debugSearchOutput.size) {
            println("Debug and release builds: different output sizes")
            return false
        }

        for (ind in releaseSearchOutput.indices) {
            val r = releaseSearchOutput[ind]
            val d = debugSearchOutput[ind]

            val releaseOutputList = r.split(" ")
            val debugOutputList   = d.split(" ")

            if(releaseOutputList.size != debugOutputList.size) {
                println("Differing output: parameters missing somewhere")
                return false
            }

            var skipNext = false

            for(cnt in releaseOutputList.indices) {
                if(skipNext) {
                    skipNext = false
                    continue
                }

                if("nps" == releaseOutputList[cnt]) {
                    skipNext = true
                }

                if(releaseOutputList[cnt] != debugOutputList[cnt]) {
                    println("Debug and release builds: different output at line " + ind)
                    println(r)
                    println(d)
                    return false
                }
            }
        }
    }
    return true
}
