package EngineHandling

import java.io.IOException
import java.util.ArrayList
import java.util.concurrent.TimeoutException

/**
 * Created by vabi on 29.05.17.
 */
data class SearchResult(var eval: Int, var bestMove: String)

class ChessEngine(path: String, workingDirectory: String) {

    var uciEngine: UciEngine

    init {
        uciEngine = UciEngine(path, workingDirectory)
    }

    fun setPosition(fenPosition: String, moveList: List<String>) {
        var putString = "position fen "+fenPosition
        if(moveList.size > 0) {
            putString += " moves"
            for(move in moveList) {
                putString += " "
                putString += move
            }
        }
        uciEngine.put(putString)
    }

    fun search(depth: Int): SearchResult {
        uciEngine.put("go depth "+depth)
        var eval = 0
        var bestMove: String
        while(true) {

            var answer = uciEngine.readAnswer()
            var parts = answer.split(" ")

            if("bestmove".equals(parts[0])) {
                bestMove = parts[1]
                break
            }

            if("info".equals(parts[0])) {
                for (index in parts.indices) {
                    if ("score" == parts[index]) {
                        eval = Integer.parseInt(parts[index + 2])
                    }
                    if ("mate" == parts[index]) {
                        val matein = Integer.parseInt(parts[index + 1])
                        if (matein > 0) {
                            eval = 30000
                        } else {
                            eval = -30000
                        }
                    }
                }
            }
        }
        return SearchResult(eval, bestMove)
    }


    fun runPerft(depth: Int): Long {
        uciEngine.put("perft "+depth)
        var answer = uciEngine.readAnswer()
        answer = answer.substring(12)
        return java.lang.Long.parseLong(answer)
    }

    fun debugSearchOutput(depth: Int): List<String> {
        uciEngine.put("go depth " + depth)
        val ret = ArrayList<String>()
        while (true) {
            val answer = uciEngine.readAnswer()
            ret.add(answer)
            println(answer)
            if (answer.length >= 8 && "bestmove" == answer.substring(0, 8)) {
                break
            }
        }
        return ret
    }

    fun getType(): String  {
        uciEngine.put("info")
        return uciEngine.readAnswer()
    }

    fun close() {
        uciEngine.destroy()
    }

    fun getEval(): Int {
        uciEngine.put("eval")
        val answer = uciEngine.readAnswer()
        val parts = answer.split(" ")
        return Integer.parseInt(parts[1])
    }

}
