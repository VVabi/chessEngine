package GUI

import ChessBoard.ChessBoard
import EngineHandling.ChessEngine
import javafx.fxml.FXML
import java.io.File
import kotlin.concurrent.thread

class SearchController {
    @FXML
    fun handleSearchAll() {
        thread {
            for (d in 3..7) {
                val lines = File("src/GUI/searchPositions.txt").readLines()

                var totalNodes: Long = 0

                for (line in lines) {
                    val engine = ChessEngine("/home/vabi/code/chessEngine/Cpp/Release/Vabi", "") //TODO
                    engine.setPosition(line, listOf())
                    var res = engine.search(d)

                    totalNodes = totalNodes + res.nodes
                    engine.close()
                }

                println(totalNodes)
            }
        }
    }
}