package PlayingGUI;

import ChessBoard.ChessBoard;
import core.PlayingEventInterface;
import javafx.fxml.FXML;
import javafx.fxml.FXMLLoader;
import javafx.scene.Parent;
import javafx.scene.Scene;
import javafx.stage.Stage;

import java.io.IOException;

/**
 * Created by fabian on 15.08.15.
 */
public class PlayingGUI {
    Stage stage;
    Parent root;
    Scene scene;
    PlayingController pcontroller;


    public PlayingGUI(Stage primaryStage, PlayingEventInterface eventInterface){
        stage = primaryStage;
        try {
            FXMLLoader loader = new FXMLLoader(getClass().getResource("playinggui.fxml"));
            root = (Parent) loader.load();
            pcontroller = (PlayingController) loader.getController();
            pcontroller.setEventInterface(eventInterface);
            pcontroller.startEventThread();
        } catch (IOException e) {
            e.printStackTrace();
            System.exit(0);
        }
        root.setId("background");
        stage.setTitle("Vabi2.0");
        scene = new Scene(root, 1280,600);
        scene.getStylesheets().add(PlayingGUI.class.getResource("menuStyle.css").toExternalForm());
        //stage.setResizable(false);
        stage.setScene(scene);
        stage.show();

    }

    public void dispose(){
        pcontroller.dispose();

    }

}
