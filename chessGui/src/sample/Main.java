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
         /*Runnable r = new Runnable() {
            @Override
            public void run() {
                try {
                    UciProcessing matchup1 = new UciProcessing("/home/vabi/code/chessEngine/Cpp/TestRelease/Vabi", "/home/vabi/code/chessEngine/Cpp/Release/Vabi", 100);
                    UciProcessing matchup2 = new UciProcessing("/home/vabi/code/stockfish-7-linux/Linux/stockfish", "/home/vabi/code/chessEngine/Cpp/Release/Vabi", 100);
                    UciProcessing matchup3 = new UciProcessing("/home/vabi/code/chessEngine/Cpp/TestRelease/Vabi", "/home/vabi/code/stockfish-7-linux/Linux/stockfish", 100);



                    Thread T1 = new Thread(new Runnable(){
                        @Override
                        public void run() {
                            try {
                                matchup1.play();
                            } catch (IOException e) {
                                e.printStackTrace();
                            }
                        }
                    }
                    );

                    Thread T2 = new Thread(new Runnable(){
                        @Override
                        public void run() {
                            try {
                                matchup2.play();
                            } catch (IOException e) {
                                e.printStackTrace();
                            }
                        }
                    }
                    );
                    Thread T3 = new Thread(new Runnable(){
                        @Override
                        public void run() {
                            try {
                                matchup3.play();
                            } catch (IOException e) {
                                e.printStackTrace();
                            }
                        }
                    }
                    );

                    T1.start();
                    T2.start();
                    T3.start();

                    try {
                        T1.join();
                    } catch (InterruptedException e) {
                        e.printStackTrace();
                    }

                    double stockfish = 0;
                    double release = 0;
                    double testrelease = 0;

                    double res1 = matchup1.getResult();
                    testrelease = testrelease+res1;
                    release  = release+100-res1;

                    try {
                        T2.join();
                    } catch (InterruptedException e) {
                        e.printStackTrace();
                    }

                    double res2 = matchup1.getResult();
                    stockfish = stockfish+res2;
                    release  = release+100-res2;
                    try {
                        T3.join();
                    } catch (InterruptedException e) {
                        e.printStackTrace();
                    }

                    double res3 = matchup1.getResult();
                    testrelease = testrelease+res3;
                    stockfish  = stockfish+100-res3;
                    System.out.println(res1);
                    System.out.println(res2);
                    System.out.println(res3);
                    System.out.println("STOCKFISH: "+stockfish);
                    System.out.println("TESTRELEASE: "+testrelease);
                    System.out.println("RELEASE: "+release);


                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
        };

        (new Thread(r)).start();*/
    }


    public static void main(String[] args) {
        launch(args);
    }
}
