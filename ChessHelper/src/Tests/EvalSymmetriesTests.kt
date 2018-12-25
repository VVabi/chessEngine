package Tests

import EngineHandling.ChessEngine
import java.io.File

/**
 * Created by vabi on 01.06.17.
 */
fun runEvalSymmetryTests(engine: ChessEngine, debugEngine: ChessEngine): Boolean {

    val dataFile = File("../quiet-labeled.epd")
    val lines = dataFile.readLines()
    var failures = 0

    for (fen in lines) {
        println("Testing "+fen)
        engine.setPosition(fen, listOf())
        val eval1 = engine.getEval()
        engine.setPosition(changeSide(fen), listOf())
        val eval2 = engine.getEval()
        engine.setPosition(mirror(fen), listOf())
        val eval3 = engine.getEval()
        var failed = false
        if (eval1 != -eval2) {
            println("Unsymmetric black/white eval detected")
            println(fen)
            println(eval1)
            println(eval2)
            println(changeSide(fen))
            failed = true
            return false
        }

        if (eval1 != eval3) {
            println("Asymmetric east/west eval detected")
            println(fen)
            println(mirror(fen))
            failed = true
            return false
        }


        if (failed) {
            failures = failures+1
        }
    }

    if (failures > 0) {
        println("Num failures: "+failures)
        return false
    }

    return true
}