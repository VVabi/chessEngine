package sample;

import core.Core;
import javafx.application.Application;
import javafx.application.Platform;
import javafx.fxml.FXMLLoader;
import javafx.scene.Parent;
import javafx.scene.Scene;
import javafx.stage.Stage;
import PlayingGUI.PlayingGUI;
import uciClient.UciProcessing;
import uciClient.uciEngineHandler;

import java.io.IOException;
import java.util.ArrayList;
import java.util.List;

public class Main extends Application {

    @Override
    public void start(Stage primaryStage) throws Exception{
/*        uciEngineHandler vabiHandler = new uciEngineHandler("/home/vabi/code/chessEngine/Cpp/Release/Vabi");

        List<String> moves = new ArrayList<>();
        while(true){
            vabiHandler.setPosition(moves);
            vabiHandler.startSearch();
            String mv = vabiHandler.readBestmove();
            moves.add(mv);
            System.out.println(mv);
            stockfishHandler.setPosition(moves);
            stockfishHandler.startSearch();
            mv = stockfishHandler.readBestmove();
            moves.add(mv);
            System.out.println(mv);
            if("(none)".equals(mv)){
                break;
            }
        }*/


        final Core cor = new Core(primaryStage);
        Runnable r = new Runnable() {
            @Override
            public void run() {
                UciProcessing matchup = null;
                try {
                    matchup = new UciProcessing("/home/vabi/code/chessEngine/Cpp/uciChallengeRelease/Vabi", "/home/vabi/code/chessEngine/Cpp/uciDefenderRelease/Vabi", 20000);
                    double res =  matchup.play();
                } catch (IOException e) {
                    e.printStackTrace();
                }

            }
        };
        System.out.println("Starting...");
        (new Thread(r)).start();



    }


    public static void main(String[] args) {
        launch(args);
    }
}
