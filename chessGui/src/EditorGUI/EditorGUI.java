package EditorGUI;

import PlayingGUI.PlayingController;
import javafx.fxml.FXMLLoader;
import javafx.scene.Parent;
import javafx.scene.Scene;
import javafx.stage.Stage;

import java.io.IOException;
import java.util.concurrent.CountDownLatch;

/**
 * Created by fabian on 15.08.15.
 */
public class EditorGUI {
    Stage stage;
    Parent root;
    Scene scene;
    CountDownLatch countdown;
    PlayingController pcontroller;
    EditorController econtroller;
    public EditorGUI(PlayingController controller, String position){
        pcontroller = controller;

        try {
            FXMLLoader loader = new FXMLLoader(getClass().getResource("editorgui.fxml"));
            root = (Parent) loader.load();
            econtroller = (EditorController) loader.getController();
        } catch (IOException e) {
            e.printStackTrace();
            System.exit(0);
        }
        stage = new Stage();
        root.setId("background");
        stage.setTitle("Vabi2.0");

        scene = new Scene(root, 900,600);
        scene.getStylesheets().add(EditorGUI.class.getResource("menuStyle.css").toExternalForm());
        stage.setResizable(false);
        stage.setScene(scene);
        stage.show();

        econtroller.setPlayController(pcontroller);
        econtroller.setPosition(position);

    }

    public void exit(){

        stage.close();

    }


}
