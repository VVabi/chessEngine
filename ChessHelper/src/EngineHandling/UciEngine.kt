package EngineHandling


import java.io.*

/**
 * Created by vabi on 29.05.17.
 */


class UciEngine(path: String, workingDirectory: String) {

    val reader: BufferedReader
    val writer: BufferedWriter
    val process: Process

    init {
        val processBuilder = ProcessBuilder(path)
        processBuilder.directory(File(workingDirectory).absoluteFile)
        process = processBuilder.start()
        reader = BufferedReader(InputStreamReader(process.inputStream))
        writer = BufferedWriter(OutputStreamWriter(process.outputStream))

        put("EngineHandling")
        put("isready")

        while(true) {
            val answer = readAnswer()
            if("readyok".equals(answer)) {
                break;
            }
        }

        println("Successfully initialized engine!");
    }

    fun put(message: String) {
        writer.write(message+"\n")
        writer.flush()
    }

    fun readAnswer(): String {
        return reader.readLine()
    }

    fun destroy() {
        process.destroyForcibly()
    }

}