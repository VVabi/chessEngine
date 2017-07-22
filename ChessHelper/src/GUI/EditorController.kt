package EditorGUI

import ChessBoard.ChessBoard
import ChessBoard.FigureChooser
import javafx.fxml.FXML
import javafx.scene.control.CheckBox
import javafx.scene.input.MouseEvent

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

    @FXML
    fun handleClicked(event: MouseEvent) {
        val field = chessboard!!.getField(event.x, event.y)
        val figure = chooser!!.chosenFigure
        var position = chessboard!!.position
        var newposition = position.substring(0, field) + figure + position.substring(field + 1)
        chessboard!!.position = newposition
    }

    @FXML
    fun handleClear() {
        chessboard!!.position = "0000000000000000000000000000000000000000000000000000000000000000"
    }

    @FXML
    fun handlePlay() {
        val castling = (if (wkcastle!!.isSelected) "K" else "0") + (if (wqcastle!!.isSelected) "Q" else "0") + (if (bkcastle!!.isSelected) "k" else "0") + if (bqcastle!!.isSelected) "q" else "0"
        val ret = position.substring(0, 64) + chooser!!.movingside + castling
        println(ret)
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