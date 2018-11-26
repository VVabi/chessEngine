package Tools

import java.io.BufferedReader
import java.io.InputStreamReader

/**
 * Created by vabi on 02.06.17.
 */

data class LOSdata(val numGames: Int, val winningFraction: Double, val eloDifference: Double, val los: Double)

fun getLos(wins: Int, draws: Int, losses: Int): LOSdata {
    val pb = ProcessBuilder("/home/vabi/Tools/workspace/LOS/Debug/LOS", Integer.toString(wins), Integer.toString(losses), Integer.toString(draws))
    val process: Process
    val reader: BufferedReader
    process = pb.start()
    reader = BufferedReader(InputStreamReader(process.inputStream))
    val numGames        = reader.readLine().toInt()
    val winningFraction = reader.readLine().toDouble()
    val eloDifference   = reader.readLine().toDouble()
    var los = 0.0
    try {
        los = reader.readLine().toDouble()
    } catch(e: NumberFormatException) {

    }

    process.destroy()
    return LOSdata(numGames, winningFraction, eloDifference, los)
    return LOSdata(0,0.0,0.0,0.0)
}