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
        while(true) {
            engine1.setPosition(start, moveList)
            try {
                var res = engine1.search(depth, timeOut)
                moveList = moveList+res.bestMove

                if(res.eval > 20000) {
                    ret = GameResult.ENG1
                    break
                }

                if(res.eval < -20000) {
                    ret = GameResult.ENG2
                    break
                }
            } catch(e: TimeoutException) {
                engine1.close()
                engine2.close()
                println("Search timed out")
                return GameInfo(GameResult.TIMEOUT, moveList)
            }


            engine2.setPosition(start, moveList)
            try {
                var res = engine2.search(depth, timeOut)
                moveList = moveList+res.bestMove

                if(res.eval > 20000) {
                    ret = GameResult.ENG2
                    break
                }

                if(res.eval < -20000) {
                    ret = GameResult.ENG1
                    break
                }
            } catch(e: TimeoutException) {
                engine1.close()
                engine2.close()
                println("Search timed out")
                return GameInfo(GameResult.TIMEOUT, moveList)
            }


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