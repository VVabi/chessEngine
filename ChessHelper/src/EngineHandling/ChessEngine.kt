package EngineHandling

import java.util.ArrayList

/**
 * Created by vabi on 29.05.17. Abstract a chess engine (currently UCI only)
 */
data class SearchResult(var eval: Int, var bestMove: String)

class ChessEngine(path: String, workingDirectory: String) {

    var uciEngine: UciEngine = UciEngine(path, workingDirectory)

    fun setPosition(fenPosition: String, moveList: List<String>) {
        var putString = "position fen "+fenPosition
        if(moveList.isNotEmpty()) {
            putString += " moves"
            for(move in moveList) {
                putString += " "
                putString += move
            }
        }
        uciEngine.put(putString)
    }

    fun stop() {
        uciEngine.put("stop")
    }

    fun analyze(depth:Int, callback: (m: String) -> Unit) {
        uciEngine.put("go depth "+depth)
        while(true) {
            val answer = uciEngine.readAnswer()
            callback(answer)
            val parts = answer.split(" ")
            if ("bestmove" == parts[0]) {
                break
            }
        }
    }

    fun search(depth: Int): SearchResult {
        uciEngine.put("go depth "+depth)
        var eval = 0
        val bestMove: String
        while(true) {

            val answer = uciEngine.readAnswer()
            val parts = answer.split(" ")

            if("bestmove" == parts[0]) {
                bestMove = parts[1]
                break
            }

            if("info" == parts[0]) {
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

    fun getEvalDetails(): String {
        uciEngine.put("eval")
        val answer = uciEngine.readAnswer()
        return answer
    }
}
