package GUI

/**
 * Created by vabi on 03.06.17.
 */
import Tools.SelfPlayTournament
import Tools.getLos
import javafx.fxml.FXML
import javafx.scene.control.TextArea
import javafx.scene.control.TextField
import javafx.stage.FileChooser
import javafx.stage.Stage


class tuningController {
    //TODO: unify! the file choosing code is thruplicated
    @FXML var engine1: TextField? = null
    @FXML var engine2: TextField? = null
    @FXML var openingFile: TextField? = null
    @FXML var depthField: TextField? = null
    @FXML var threadField: TextField? = null
    @FXML var resultArea: TextArea? = null
    @FXML fun handleChooseDefender() {
       val fileChooser = FileChooser()
       val file        = fileChooser.showOpenDialog(Stage())
        engine1?.text = file.absolutePath
    }

    @FXML fun handleChooseChallenger() {
        val fileChooser = FileChooser()
        val file        = fileChooser.showOpenDialog(Stage())
        engine2?.text = file.absolutePath
    }

    @FXML fun handleChooseOpening() {
        val fileChooser = FileChooser()
        val file        = fileChooser.showOpenDialog(Stage())
        openingFile?.text = file.absolutePath
    }

    var selfPlayer: SelfPlayTournament? = null


    @FXML fun handleLaunchTournament() {
        selfPlayer = SelfPlayTournament(engine1!!.text, engine2!!.text, openingFile!!.text, depthField!!.text.toInt(), threadField!!.text.toInt(), this::updateResults)
        selfPlayer?.launchTournament()
    }

    @FXML fun handleStopTuning() {
        selfPlayer?.interrupt()
    }

    @Synchronized fun updateResults(challengerWins: Int, defenderWins: Int, draws: Int) {
        var updateText = ""
        updateText += "Challenger $challengerWins Defender $defenderWins Draws $draws\n"
        val losData = getLos(challengerWins, draws, defenderWins)
        updateText += "Win percentage "+losData.winningFraction+"\nELO difference "+losData.eloDifference+ "\nLOS "+losData.los
        try {
            resultArea?.text = updateText
        } catch(e: Exception) {
            println(updateText)
        }
    }
}