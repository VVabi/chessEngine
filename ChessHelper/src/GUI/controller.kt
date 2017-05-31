package GUI

/**
 * Created by vabi on 31.05.17.
 */
import javafx.fxml.FXML
import javafx.scene.control.Label

class controller {

    @FXML var helloLabel: Label? = null

    @FXML fun handleClickMe() {
        helloLabel?.text = "You clicked!!!"
    }
}