package sample;

import Tests.TestDebugVsRelease;
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

import java.io.BufferedReader;
import java.io.File;
import java.io.IOException;
import java.io.InputStreamReader;
import java.math.BigInteger;
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
       /* Runtime runtime = Runtime.getRuntime();
        Process process;
        BufferedReader reader;
        File pathToExecutable = new File("");
        ProcessBuilder pb = new ProcessBuilder("/home/vabi/code/chessEngine/Cpp/uciRelease/Vabi");
        pb.directory(pathToExecutable.getAbsoluteFile());
        process = pb.start();
        reader = new BufferedReader(new InputStreamReader(process.getInputStream()));
        while(true){
            String st = reader.readLine();
            if(st == null){
                break;
            }
            System.out.println(st);
        }*/


       /* String s = "110000010010001000010100000000000";
        s = new StringBuilder(s).reverse().toString();

        for(int ind=0; ind < s.length(); ind++) {
            if(s.charAt(ind) == '1') {
                System.out.println(ind);
            }
        }*/
       /* if(TestDebugVsRelease.testDebugvsRelease("/home/vabi/code/chessEngine/Cpp/Debug/Vabi", "/home/vabi/code/chessEngine/Cpp/Release/Vabi")) {
            System.out.println("Test passed");
        } else {
            System.out.println("Test failed");
        }*/



        /*uciEngineHandler handler = new uciEngineHandler("/home/vabi/code/chessEngine/Cpp/Release/Vabi",  "params1");
        List<String> debugSearchOutput = handler.debugSearchOutput(12);

        for(String s: debugSearchOutput){
            System.out.println(s);
        }*/

        UciProcessing processesList[] = new UciProcessing[3];

        for(int ind=0; ind < 3; ind++) {
            processesList[ind] = new UciProcessing("/home/vabi/code/chessEngine/Cpp/uciDefenderRelease/Vabi", "params1", "/home/vabi/code/chessEngine/Cpp/uciChallengeRelease/Vabi", "params2", 10000);
            //final Core cor = new Core(primaryStage);
            final int index = ind;
            Thread.sleep(1523);
            Runnable r = new Runnable() {
                @Override
                public void run() {
                    try {
                        double res =  processesList[index].play();
                    } catch (IOException e) {
                        e.printStackTrace();
                    }

                }
            };

            (new Thread(r)).start();

        }

        int wins = 0;
        int draws = 0;
        int losses = 0;

        while(wins+draws+losses < 30000) {

            wins = 0;
            draws = 0;
            losses = 0;

            for(int ind=0; ind < 3; ind++) {
                int[] res = processesList[ind].getCurrentResult();
                wins = wins+res[0];
                draws = draws+res[1];
                losses = losses+res[2];
            }

            double numGames = wins+draws+losses;
            double total = wins+0.5*((double) draws);
            double elo_diff = 400*(Math.log10(1-total/((double) numGames))-Math.log10(total/(double) numGames));
            System.out.println(" ");
            System.out.println("Total games "+numGames);
            System.out.println("Wins "+wins+ " Draws "+draws+ " Losses "+losses);
            System.out.println("Elo difference is " +elo_diff);
            System.out.println(" ");


            Thread.sleep(10000);
        }



    }


    public static void main(String[] args) {
        launch(args);
    }
}
