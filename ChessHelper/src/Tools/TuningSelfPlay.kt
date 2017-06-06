package Tools

import EngineHandling.EngineDescriptor
import EngineHandling.EnginePlayer
import EngineHandling.GameInfo
import java.io.File
import java.util.concurrent.atomic.AtomicBoolean
import java.util.concurrent.atomic.AtomicInteger
import kotlin.concurrent.thread

/**
 * Created by vabi on 03.06.17.
 */




class SelfPlayTournament(val defenderPath: String, val challengerPath: String, openingPositionsPath: String, depth: Int, numThreads: Int, resultCallback: (Int, Int, Int) -> Unit) {

    val searchDepth = depth
    val threads = numThreads

    var defenderWins = AtomicInteger(0)
    var challengerWins = AtomicInteger(0)
    var draws          = AtomicInteger(0)

    var newResultCallback = resultCallback
    val lines = File(openingPositionsPath).readLines()
    var index = 0
    var continueSelfPlay = AtomicBoolean(true)
    @Synchronized fun getNextStartPosition()  : String {
        if(index >= lines.size) {
            index = 0
        }
        var ret = lines[index]
        while(ret[0] == '%') {
            index++
            if(index >= lines.size) {
                index = 0
            }
            ret = lines[index]
        }
        index++
        println(ret)
        return ret
    }

    fun interrupt() {
        continueSelfPlay.set(false)
    }

    fun launchTournament() {
        for(i in 1..threads) {
            thread {
                while(continueSelfPlay.get()) {
                    val defenderDescriptor = EngineDescriptor(defenderPath, "", "DEFENDER")
                    val challengerDescriptor = EngineDescriptor(challengerPath, "", "CHALLENGER")
                    val startPosition = getNextStartPosition()
                    playSingleGame(defenderDescriptor, challengerDescriptor, searchDepth, startPosition, this::gameFinishedCallback)
                    playSingleGame(challengerDescriptor, defenderDescriptor, searchDepth, startPosition, this::gameFinishedCallback)
                }
            }
        }
    }


    fun playSingleGame(defenderDescriptor: EngineDescriptor, challengerDescriptor: EngineDescriptor, depth: Int,  startPosition: String, callback: (result: GameInfo) -> Unit) {
            val player = EnginePlayer(defenderDescriptor, challengerDescriptor, startPosition, depth)
            callback(player.play())
    }

    fun gameFinishedCallback(result: GameInfo){
        //TODO: the strings are too implicit and defined twice
        if(result.res == "DRAW") {
            draws.incrementAndGet()
        }

        else if(result.res == "DEFENDER") {
            defenderWins.incrementAndGet()
        }

        else if(result.res == "CHALLENGER") {
            challengerWins.incrementAndGet()
        }

       // println(result)
        newResultCallback(challengerWins.get(), defenderWins.get(), draws.get())
    }


}
