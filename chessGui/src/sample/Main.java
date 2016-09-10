package sample;

import core.Core;
import javafx.application.Application;
import javafx.application.Platform;
import javafx.fxml.FXMLLoader;
import javafx.scene.Parent;
import javafx.scene.Scene;
import javafx.stage.Stage;
import PlayingGUI.PlayingGUI;
public class Main extends Application {

    @Override
    public void start(Stage primaryStage) throws Exception{
        final Core cor = new Core(primaryStage);
    }


    public static void main(String[] args) {
        launch(args);
    }
}
