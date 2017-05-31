package GUI

/**
 * Created by vabi on 31.05.17.
 */
import EngineHandling.ChessEngine
import Tests.runPerftTests
import javafx.fxml.FXML
import javafx.scene.control.Button
import javafx.scene.control.Label
import java.util.concurrent.atomic.AtomicBoolean
import kotlin.concurrent.thread

class controller {

    @FXML var helloLabel: Label? = null

    @FXML fun handleClickMe() {
        helloLabel?.text = "You clicked!!!"
    }

    @FXML var perft: Button? = null

    var perftRunning: AtomicBoolean = AtomicBoolean(false)

    @FXML fun handlePerftTests() {
        if(perftRunning.compareAndSet(false, true)) {
            perft?.setStyle("-fx-base: #ffff00;");
            thread() {
                perft()
            }
        } else {
            println("Tests are already running, not starting again")
        }
    }

    fun perft() {
        var engine = ChessEngine("/home/vabi/code/chessEngine/Cpp/Release/Vabi", "/home/vabi")
        var ret = runPerftTests(engine)
        engine.close()
        println(ret)

        if(ret) {
            perft?.setStyle("-fx-base: #00ff00;")
        } else {
            perft?.setStyle("-fx-base: #ff0000;")
        }
        perftRunning.set(false)
    }

}