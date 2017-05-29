package EngineHandling

import java.util.*
import java.util.concurrent.TimeoutException

/**
 * Created by vabi on 29.05.17.
 */

enum class GameResult {
    ENG1,ENG2,DRAW,TIMEOUT
}


data class GameInfo(val res: GameResult, val moves: List<String>)

class EnginePlayer(path1: String, params1: String, path2: String, params2: String, startPos: String, depth: Int, timeOut: Int) {

    val engine1: ChessEngine
    val engine2: ChessEngine

    var moveList = listOf<String>()
    var start: String
    var timeOut: Int
    var depth: Int
    init {
        engine1 = ChessEngine(path1, params1)
        engine2 = ChessEngine(path2, params2)
        start = startPos
        this.timeOut = timeOut
        this.depth   = depth
    }

    fun play(): GameInfo {

        var moveCounter = 0
        var ret = GameResult.DRAW
        var currentEngine: ChessEngine
        var engine1ToMove = true

        while(true) {

            if(engine1ToMove) {
                currentEngine = engine1
            } else {
                currentEngine = engine2
            }


            currentEngine.setPosition(start, moveList)
            try {
                var res = currentEngine.search(depth, timeOut)
                moveList = moveList+res.bestMove

                if(res.eval > 20000) {
                    if(engine1ToMove) {
                        ret = GameResult.ENG1
                    } else {
                        ret = GameResult.ENG2
                    }
                    break
                }

                if(res.eval < -20000) {
                    if(engine1ToMove) {
                        ret = GameResult.ENG2
                    } else {
                        ret = GameResult.ENG1
                    }
                    break
                }
            } catch(e: TimeoutException) {
                engine1.close()
                engine2.close()
                println("Search timed out")
                return GameInfo(GameResult.TIMEOUT, moveList)
            }

            engine1ToMove = !engine1ToMove

            moveCounter++
            if(moveCounter > 300) {
                break
            }
        }

        for(move in moveList) {
            println(move)
        }

        engine1.close()
        engine2.close()
        return GameInfo(ret, moveList)
    }

    fun close() {
        engine1.close()
        engine2.close()
    }

}