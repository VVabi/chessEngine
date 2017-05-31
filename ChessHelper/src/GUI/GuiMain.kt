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
        var loader: FXMLLoader = FXMLLoader(javaClass.getResource("gui.fxml"))
        var root = loader.load<Parent>()
        var scene = Scene(root, 640.0, 480.0)
        primaryStage.scene = scene
        primaryStage.show()
    }
}
