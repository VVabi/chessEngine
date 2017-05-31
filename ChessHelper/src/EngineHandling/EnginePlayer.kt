package EngineHandling

import java.util.*
import java.util.concurrent.TimeoutException

/**
 * Created by vabi on 29.05.17.
 */


data class GameInfo(val res: String, val moves: List<String>)

data class EngineDescriptor(val executablePath: String, val workingDirectory: String, val name: String)

data class LaunchedEngine(val engine: ChessEngine, val name: String)

class EnginePlayer(engineDescriptor1: EngineDescriptor, engineDescriptor2: EngineDescriptor, startPos: String, depth: Int) {

    val engine1: LaunchedEngine
    val engine2: LaunchedEngine

    var moveList = listOf<String>()
    var start: String
    var depth: Int
    init {
        this.engine1 = LaunchedEngine(ChessEngine(engineDescriptor1.executablePath, engineDescriptor1.workingDirectory), engineDescriptor1.name)
        this.engine2 = LaunchedEngine(ChessEngine(engineDescriptor2.executablePath, engineDescriptor2.workingDirectory), engineDescriptor2.name)
        start = startPos
        this.depth   = depth
    }

    fun play(): GameInfo {

        var moveCounter = 0
        var ret = "DRAW"
        var currentEngine: LaunchedEngine
        var engine1ToMove = true

        while(true) {

            if(engine1ToMove) {
                currentEngine = engine1
            } else {
                currentEngine = engine2
            }


            currentEngine.engine.setPosition(start, moveList)
            try {
                var res = currentEngine.engine.search(depth)
                moveList = moveList+res.bestMove

                if(res.eval > 20000) {
                    if(engine1ToMove) {
                        ret = engine1.name
                    } else {
                        ret = engine2.name
                    }
                    break
                }

                if(res.eval < -20000) {
                    if(engine1ToMove) {
                        ret = engine2.name
                    } else {
                        ret = engine1.name
                    }
                    break
                }
            } catch(e: TimeoutException) {
                close()
                println("Search timed out")
                return GameInfo("TIMEOUT", moveList)
            }

            engine1ToMove = !engine1ToMove

            moveCounter++
            if(moveCounter > 300) {
                break
            }
        }

        close()
        return GameInfo(ret, moveList)
    }

    fun close() {
        engine1.engine.close()
        engine2.engine.close()
    }

}