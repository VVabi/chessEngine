package GUI

import ChessBoard.ChessBoard
import EngineHandling.ChessEngine
import javafx.fxml.FXML
import java.io.File
import java.util.concurrent.CountDownLatch
import kotlin.concurrent.thread

data class SearchRunResult(var nodecounts: HashMap<Int, Long> )

class SearchController {
    @FXML
    fun handleSearchAll() {
        var depth = 10
        val list = listOf("src/GUI/beginning.txt", "src/GUI/midgame.txt", "src/GUI/endgame.txt")

        val results = HashMap<String, SearchRunResult>()

        var cd = CountDownLatch(list.size)


        thread {
            cd.await()


            var r = SearchRunResult(HashMap<Int, Long>())

            for (d in 3..depth) {
                r.nodecounts.put(d, 0)
            }

            for (res in results.entries) {
                println("Results for "+res.key)
                var previous: Long = -1

                for (x in res.value.nodecounts.entries) {
                    println("Depth "+x.key)
                    println("Nodes "+x.value)
                    var nodes: Long = r.nodecounts!!.get(x.key)!! //TODO
                    nodes += x.value
                    r.nodecounts.put(x.key, nodes)
                    if (previous > 0) {
                        println("Ratio "+x.value/(previous.toDouble()))
                    }
                    previous = x.value;
                }
            }
            println("Total")
            var previous: Long = -1
            for (x in r.nodecounts.entries) {
                println("Depth "+x.key)
                println("Nodes "+x.value)
                if (previous > 0) {
                    println("Ratio "+x.value/(previous.toDouble()))
                }
                previous = x.value;
            }
        }


        for (l in list) {
            results.put(l, SearchRunResult(HashMap<Int, Long>()));
            thread {
                for (d in 3..depth) {
                    val lines = File(l).readLines()

                    var totalNodes: Long = 0

                    for (line in lines) {
                        val engine = ChessEngine("/home/vabi/code/chessEngine/Cpp/Release/Vabi", "") //TODO
                        engine.setPosition(line, listOf())
                        var res = engine.search(d)

                        totalNodes = totalNodes + res.nodes
                        engine.close()
                    }
                    results.get(l)!!.nodecounts.put(d, totalNodes)
                   // println(totalNodes)
                }
                cd.countDown();
            }
        }


    }
}