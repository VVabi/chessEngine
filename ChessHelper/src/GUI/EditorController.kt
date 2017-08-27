package EditorGUI

import ChessBoard.ChessBoard
import ChessBoard.FigureChooser
import EngineHandling.ChessEngine
import javafx.application.Platform
import javafx.fxml.FXML
import javafx.scene.control.CheckBox
import javafx.scene.control.TextArea
import javafx.scene.control.TextField
import javafx.scene.input.MouseEvent
import java.awt.event.ActionEvent
import java.util.concurrent.atomic.AtomicBoolean
import kotlin.concurrent.thread

/**
 * Created by fabian on 15.08.15.
 */
class EditorController {

    @FXML private var chooser: FigureChooser? = null
    @FXML private var chessboard: ChessBoard? = null
    @FXML private var wkcastle: CheckBox? = null
    @FXML private var wqcastle: CheckBox? = null
    @FXML private var bkcastle: CheckBox? = null
    @FXML private var bqcastle: CheckBox? = null
    @FXML private var fentext: TextField? = null
    @FXML private var engineoutput: TextArea? = null
    @FXML private var evaltext: TextArea? = null
    private val engine = ChessEngine("/home/vabi/code/chessEngine/Cpp/Release/Vabi", "/home/vabi")

    @FXML
    fun handleTake() {
        var fen = fentext!!.text
        chessboard?.position = fen
    }

    @FXML
    fun handleGetEval(){
        val localEngine = ChessEngine("/home/vabi/code/chessEngine/Cpp/Release/Vabi", "/home/vabi")
        localEngine.setPosition(chessboard!!.position, mutableListOf())
        var answer = localEngine.getEvalDetails()
        Platform.runLater() {
            evaltext?.text = answer.replace(Regex("( -?[0-9]* )"), "$1\n")
        }
        localEngine.close()
    }


    @FXML
    fun handleClicked(event: MouseEvent) {
        val field = chessboard!!.getField(event.x, event.y)
        val figure = chooser!!.chosenFigure
        chessboard?.addPiece(field, figure)
        fentext?.text = chessboard!!.position
    }

    @FXML
    fun handleClear() {
        chessboard?.position = "8/8/8/8/8/8/8/8 w - - 0 1"
        fentext?.text = chessboard!!.position
    }

    @FXML
    fun handlePlay() {
        val castling = (if (wkcastle!!.isSelected) "K" else "0") + (if (wqcastle!!.isSelected) "Q" else "0") + (if (bkcastle!!.isSelected) "k" else "0") + if (bqcastle!!.isSelected) "q" else "0"
        val ret = position.substring(0, 64) + chooser!!.movingside + castling
        println(ret)
    }

    @FXML
    fun handleUpdate() {
        val castling = (if (wkcastle!!.isSelected) "K" else "0") + (if (wqcastle!!.isSelected) "Q" else "0") + (if (bkcastle!!.isSelected) "k" else "0") + if (bqcastle!!.isSelected) "q" else "0"
        chessboard?.setCastling(castling)
        fentext?.text = chessboard!!.position
    }

    private var engineLock = AtomicBoolean(false) //this is EXTREMELY dirty, but good enough for now
    @FXML
    fun handleAnalyze() {
        thread {
            engine.stop()
            while(engineLock.get());
            engineLock.set(true)
            Platform.runLater() {
                engineoutput?.clear() }
            engine.setPosition(chessboard!!.position, mutableListOf())
            engine.analyze(28, this::addToOutput)
            engineLock.set(false)
        }
    }

    fun addToOutput(result: String){
        Platform.runLater {
        engineoutput?.appendText("\n"+result) }
    }

    var movingSide: Char
        get() = chooser!!.movingside
        set(side) {
            chooser!!.movingside = side
        }


    @FXML
    fun handleChooserClicked(event: MouseEvent) {
        var x = event.x.toInt()
        var y = event.y.toInt()

        y = y / 60 + 1
        x = x / 60
        if (y < 7) {
            chooser!!.setChosenFigure(6 * x + y)
        } else {
            chooser!!.toggleMovingSide()
            chessboard?.updateMovingSide(chooser!!.movingside)
            fentext?.text = chessboard!!.position
        }
    }

    var position: String
        get() = chessboard!!.position
        set(pos) {
            chessboard!!.position = pos

            if (pos[65] == 'K') {
                wkcastle!!.isSelected = true
            }
            if (pos[66] == 'Q') {
                wqcastle!!.isSelected = true
            }
            if (pos[67] == 'k') {
                bkcastle!!.isSelected = true
            }
            if (pos[68] == 'q') {
                bqcastle!!.isSelected = true
            }


        }

}