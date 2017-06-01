package GUI

/**
 * Created by vabi on 31.05.17.
 */
import EngineHandling.ChessEngine
import Tests.runPerftTests
import Tests.testDebugvsRelease
import javafx.fxml.FXML
import javafx.scene.control.Button
import javafx.scene.control.Label
import java.lang.System.exit
import java.util.concurrent.atomic.AtomicBoolean
import kotlin.concurrent.thread


data class testData(val Testfunction: (ChessEngine, ChessEngine) -> Boolean, var isRunning: AtomicBoolean)

class controller {

    @FXML var perft: Button? = null
    @FXML var debugVsRelease: Button? = null
    var testMap: HashMap<String, testData> = HashMap()

    init {
        testMap.put("perft", testData(::runPerftTests, AtomicBoolean(false)))
        testMap.put("debugVsRelease", testData(::testDebugvsRelease, AtomicBoolean(false)))
    }

    @FXML var helloLabel: Label? = null

    @FXML fun handleClickMe() {
        helloLabel?.text = "You clicked!!!"
    }


    @FXML fun handlePerftTests() {
        thread {
            runTest("perft")
        }
    }

    @FXML fun handleDebugVsReleaseTest() {
        thread {
            runTest("debugVsRelease")
        }
    }


    fun runTest(name: String) {
        val data = testMap[name]
        if(data?.isRunning!!.compareAndSet(false, true)) {
            onTestStateChanged(name, testStatus.RUNNING)
            val releaseEngine: ChessEngine = ChessEngine("/home/vabi/code/chessEngine/Cpp/Release/Vabi", "")
            val debugEngine:   ChessEngine = ChessEngine("/home/vabi/code/chessEngine/Cpp/Debug/Vabi",   "")

            if(debugEngine.getType() == releaseEngine.getType()) {
                println("Alleged Release and Debug engines are the same type??");
                exit(1) //to be REALLY verbose about this
            }

            val ret: Boolean = data.Testfunction(releaseEngine, debugEngine)
            if(ret) {
                onTestStateChanged(name, testStatus.SUCCESS)
            } else {
                onTestStateChanged(name, testStatus.FAILED)
            }
            releaseEngine.close()
            data.isRunning.set(false)
        } else {
            println("Test "+name+" already running")
        }
    }

    enum class testStatus {
        RUNNING, SUCCESS, FAILED
    }

    fun onTestStateChanged(name: String, status:testStatus) {
        var colorString: String = ""

        when(status) {
            testStatus.FAILED -> colorString = "ff0000"
            testStatus.RUNNING -> colorString = "ffff00"
            testStatus.SUCCESS -> colorString = "00ff00"
        }

        when(name) {
            "perft" -> {
                    perft?.setStyle("-fx-base: #"+colorString+";")
            }
            "debugVsRelease" -> {
                debugVsRelease?.setStyle("-fx-base: #"+colorString+";")
            }
            else -> {
                println("Unknown test encountered")
            }
        }
    }
}