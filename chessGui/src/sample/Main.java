package sample;

import Library.FenHelper;
import Tests.EvalSymmetries;
import Tests.PerftTests;
import Tests.TestDebugVsRelease;
import Tests.TestSuite;
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

import java.io.*;
import java.lang.reflect.Array;
import java.math.BigInteger;
import java.util.ArrayList;
import java.util.LinkedList;
import java.util.List;

public class Main extends Application {

    public void printLOS(int wins, int draws, int losses) throws IOException {
        ProcessBuilder pb = new ProcessBuilder("/home/vabi/Tools/workspace/LOS/Debug/LOS", Integer.toString(wins), Integer.toString(losses), Integer.toString(draws));
        Process process;
        BufferedReader reader;
        process = pb.start();
        reader = new BufferedReader(new InputStreamReader(process.getInputStream()));
        System.out.println(reader.readLine());
        System.out.println(reader.readLine());
        System.out.println(reader.readLine());
        System.out.println(reader.readLine());
        process.destroy();

    }



    @Override
    public void start(Stage primaryStage) throws Exception{
        /*uciEngineHandler handler = new uciEngineHandler("/home/vabi/code/chessEngine/Cpp/Release/Vabi", "");
        System.out.println("perft tests");
        TestSuite.TestResult perftResult = PerftTests.testPerft(handler);
        System.out.println(perftResult.result);
        System.out.println("eval symmetry tests");
        TestSuite.TestResult symmetryresult = EvalSymmetries.testEvalSymmetry(handler);
        System.out.println(symmetryresult.result);*/



      /*  ArrayList<String> fens = new ArrayList<>();
        try (BufferedReader br = new BufferedReader(new FileReader("/home/vabi/lateCutoffs.txt"))) {
            String line;
            while ((line = br.readLine()) != null) {
                fens.add(line);
            }
        }

        int values[] = new int[100];

        for(int i=0; i< 100; i++) {
            values[i] = 0;
        }

        int badMoves[][] = new int[15][7];
        for(int i=0; i< 15; i++) {
            for(int j=0; j < 7; j++) {
                badMoves[i][j] = 0;
            }
        }





        for(String f: fens) {
            String[] splitted = f.split(" ");
            String strindex = splitted[4];
            int index = Integer.parseInt(strindex);
            values[index]++;

            if(index > 0) {
                int type = Integer.parseInt(splitted[1]);
                int ctype = Integer.parseInt(splitted[2]);


                badMoves[type][ctype]++;
            }

        }

        int total = 0;

        for(int i = 0; i< 100; i++) {
            total = total+values[i];
        }

        System.out.println(total);

        int subtotal = 0;
        for(int i = 0; i< 100; i++) {
            subtotal = subtotal+values[i];
            System.out.println(i+" "+values[i]+" "+((double)values[i])/((double) total)+" "+((double) subtotal)/(double) total);
        }


        System.out.println("move types");
        for(int i=0; i < 15; i++) {
            for(int j=0; j < 7; j++) {
                System.out.print(badMoves[i][j]);
                System.out.print(" ");
            }
            System.out.println("");
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


       // 1r3rk1/2nbbp1p/2p1p1p1/2PpP2P/pP4P1/p1RNPN2/P4P2/1K3B1R w - -

       UciProcessing processesList[] = new UciProcessing[6];

        for(int ind=0; ind < 6; ind++) {
            processesList[ind] = new UciProcessing("/home/vabi/code/chessEngine/Cpp/uciDefenderRelease/Vabi", "params1", "/home/vabi/code/chessEngine/Cpp/uciChallengeRelease/Vabi", "params2", 5000);
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


            for(int ind=0; ind < 6; ind++) {
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

            printLOS(losses, draws, wins);

            Thread.sleep(10000);
        }



    }


    public static void main(String[] args) {
        launch(args);
    }
}
