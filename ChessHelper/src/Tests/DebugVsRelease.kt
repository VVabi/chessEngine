package Tests

import EngineHandling.ChessEngine
import java.io.IOException

/**
 * Created by vabi on 01.06.17.
 */


fun testDebugvsRelease(releaseEngine: ChessEngine, debugEngine: ChessEngine): Boolean {

    val debugSearchOutput = debugEngine.debugSearchOutput(12)
    val releaseSearchOutput = releaseEngine.debugSearchOutput(12)

    if (releaseSearchOutput.size != debugSearchOutput.size) {
        println("Debug and release builds: different output sizes")
        return false
    }

    for (ind in releaseSearchOutput.indices) {
        val r = releaseSearchOutput.get(ind)
        val d = debugSearchOutput.get(ind)

        val releaseOutputList = r.split(" ")
        val debugOutputList   = d.split(" ")

        if(releaseOutputList.size != debugOutputList.size) {
            println("Differing output: parameters missing somewhere")
            return false
        }

        var skipNext = false

        for(ind in releaseOutputList.indices) {
            if(skipNext) {
                skipNext = false
                continue
            }

            if("nps".equals(releaseOutputList[ind])) {
                skipNext = true;
            }

            if(!releaseOutputList[ind].equals(debugOutputList[ind])) {
                println("Debug and release builds: different output at line " + ind)
                println(r)
                println(d)
                return false
            }
        }
    }
    return true
}
