package GUI
import EngineHandling.ChessEngine
import Tests.runPerftTests
import Tests.testDebugvsRelease
import Tests.runEvalSymmetryTests
import javafx.fxml.FXML
import javafx.scene.control.Button
import java.util.concurrent.atomic.AtomicBoolean
import kotlin.concurrent.thread

/**
 * Created by vabi on 03.06.17.
 */


data class testData(val Testfunction: (ChessEngine, ChessEngine) -> Boolean, var isRunning: AtomicBoolean)

class testController {

    @FXML var perft: Button? = null
    @FXML var debugVsRelease: Button? = null
    @FXML var evalSymmetry: Button? = null
    var testMap: HashMap<String, testData> = HashMap()

    init {
        testMap.put("perft", testData(::runPerftTests, AtomicBoolean(false)))
        testMap.put("debugVsRelease", testData(::testDebugvsRelease, AtomicBoolean(false)))
        testMap.put("evalSymmetry", testData(::runEvalSymmetryTests, AtomicBoolean(false)))
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

    @FXML fun handleEvalSymmetryTests() {
        thread {
            runTest("evalSymmetry")
        }
    }


    fun runTest(name: String) {
        val data = testMap[name]
        if(data?.isRunning!!.compareAndSet(false, true)) {
            onTestStateChanged(name, testStatus.RUNNING)
            val releaseEngine: ChessEngine = ChessEngine("/home/vabi/code/chessEngine/Cpp/uciChallengeRelease/Vabi", "")
            val debugEngine: ChessEngine = ChessEngine("/home/vabi/code/chessEngine/Cpp/Debug/Vabi",   "")

            if(debugEngine.getType() == releaseEngine.getType()) {
                println("Alleged Release and Debug engines are the same type??")
                System.exit(1) //to be REALLY verbose about this
            }

            val ret: Boolean = data.Testfunction(releaseEngine, debugEngine)
            if(ret) {
                onTestStateChanged(name, testStatus.SUCCESS)
            } else {
                onTestStateChanged(name, testStatus.FAILED)
            }
            releaseEngine.close()
            debugEngine.close()
            data.isRunning.set(false)
        } else {
            println("Test $name already running")
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
                perft?.style = "-fx-base: #$colorString;"
            }
            "debugVsRelease" -> {
                debugVsRelease?.style = "-fx-base: #$colorString;"
            }
            "evalSymmetry" -> {
                evalSymmetry?.style = "-fx-base: #$colorString;"
            }
            else -> {
                println("Unknown test encountered")
            }
        }
    }
}