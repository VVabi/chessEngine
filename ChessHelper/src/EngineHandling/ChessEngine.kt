package EngineHandling

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

    fun search(depth: Int, timeoutMillis: Int = 0): SearchResult {
        uciEngine.put("go depth "+depth)
        var current = System.currentTimeMillis()
        var eval = 0
        var bestMove: String
        while(true) {

            if(timeoutMillis > 0) {
                if(System.currentTimeMillis()-current > timeoutMillis) {
                    throw TimeoutException("Search timed out")
                }
            }

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

    fun close() {
        uciEngine.destroy()
    }

}
