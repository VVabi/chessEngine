package GUI

import javafx.application.Application
import javafx.fxml.FXMLLoader
import javafx.scene.Parent
import javafx.scene.Scene
import javafx.stage.Stage

/**
 * Created by vabi on 31.05.17.
 */


fun launchGui() {
    Application.launch(mainGuiApplication::class.java)
    println("Launched gui")
}

class mainGuiApplication : Application() {
    override fun start(primaryStage: Stage) {
        val loader: FXMLLoader = FXMLLoader(javaClass.getResource("gui.fxml"))
        val root = loader.load<Parent>()
        val scene = Scene(root, 1024.0, 768.0)
        //scene.stylesheets.add(mainGuiApplication::class.java!!.getResource("menuStyle.css").toExternalForm())
        primaryStage.scene = scene
        primaryStage.show()
    }
}
