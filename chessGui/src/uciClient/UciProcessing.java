package uciClient;

import communication.VDT.VDTstring;
import communication.VMPServer.VMPServer;
import communication.messages.VMPchessPosition;
import core.events.NewPositionEvent;

import java.io.BufferedReader;
import java.io.FileReader;
import java.io.IOException;
import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.CountDownLatch;

/**
 * Created by vabi on 02.10.16.
 */
public class UciProcessing {


    VMPServer server;
    uciEngineHandler  vabiHandler;
    uciEngineHandler stockFishHandler;
    double res = 0;

    String engine1;
    String engine2;

    int numGames = 0;
    ArrayList<String> openingPositions;

    public UciProcessing(String engine1, String engine2, int numGames) throws IOException {

        this.numGames = numGames;
        CountDownLatch cd = new CountDownLatch(1);
        /*server = new VMPServer(9999, 0, cd);
        server.connect("127.0.0.1", 9876);
         try {
            cd.await();
        } catch (InterruptedException e) {
            e.printStackTrace();
        }*/
        this.engine1 = engine1;
        this.engine2 = engine2;
        openingPositions = new ArrayList<>();
        readOpeningPositions();


    }


    private void readOpeningPositions() throws IOException {
        FileReader fileReader = new FileReader("openingPositions.txt");
        BufferedReader bufferedReader = new BufferedReader(fileReader);
        String line = null;
        while ((line = bufferedReader.readLine()) != null) {
            if(line.charAt(0) != '%') {
                System.out.println(line);
                openingPositions.add(line);
            }
        }
        bufferedReader.close();

    }

    public double play() throws IOException {

        double total = 0;

        int wins = 0;
        int draws = 0;
        int losses = 0;
        int index = 0;
        String startPos = " ";
        for(int k=0; k < numGames; k++) {
            //stockFishHandler = new uciEngineHandler("/home/vabi/code/stockfish-7-linux/Linux/stockfish");
            vabiHandler = new uciEngineHandler(engine1);
            stockFishHandler = new uciEngineHandler(engine2);

            if((k %2  == 0)){
                startPos = openingPositions.get(index);
                index++;
                if(index >= openingPositions.size()){
                    index = 0;
                }
            }

            System.out.println("Started engines");
            List<String> moves = new ArrayList<>();
            int ret = 0;
            int eval = 0;
            String mv[] = new String[1];

            if((k % 2) == 1){
                stockFishHandler.setPosition(startPos, moves);
                stockFishHandler.startSearch();
                eval = stockFishHandler.readBestmove(mv);
                //System.out.println(eval);
                moves.add(mv[0]);
            }
            while (true) {


                vabiHandler.setPosition(startPos, moves);
                String current = vabiHandler.getCurrentPosition();
                VMPchessPosition position = new VMPchessPosition(new VDTstring(current.getBytes()));
                //server.send(position, 0);
                vabiHandler.startSearch();



                eval = vabiHandler.readBestmove(mv);
                if (eval > 300) {
                    ret = 1;
                    break;
                } else if (eval < -300) {
                    ret = -1;
                    break;
                }

               // System.out.println(eval);
                moves.add(mv[0]);
                vabiHandler.setPosition(startPos, moves);
                current = vabiHandler.getCurrentPosition();
                position = new VMPchessPosition(new VDTstring(current.getBytes()));
               // server.send(position, 0);
                //System.out.println(mv[0]);
                stockFishHandler.setPosition(startPos, moves);
                stockFishHandler.startSearch();
                eval = stockFishHandler.readBestmove(mv);
               // System.out.println(eval);
                moves.add(mv[0]);
                if (moves.size() > 240) {
                    ret = 0;
                    break;
                }
                //System.out.println(mv[0]);
                if ("(none)".equals(mv[0])) {
                    break;
                }
            }

            if (ret == 0) {
                System.out.println("Draw");
                total = total+0.5;
                draws++;
            } else if (ret == 1) {
                System.out.println("Vabi won");
                total = total+1;
                wins++;
            } else {
                System.out.println("Vabi lost");
                losses++;
            }
            stockFishHandler.close();
            vabiHandler.close();
            System.out.println("Wins "+wins + " Draws "+draws+" Losses "+losses);
            double elo_diff = 400*(Math.log10(1-total/((double) (k+1)))-Math.log10(total/(double) (k+1)));
            System.out.println("Elo difference is " +elo_diff);
        }

        System.out.println("Final result: " + total);
        System.out.println("Wins "+wins + " Draws "+draws+" Losses "+losses);
        double elo_diff = 400*(Math.log10(1-total/((double) numGames))-Math.log10(total/(double) numGames));
        System.out.println("Elo difference is " +elo_diff);
        res = total;
        return total;

    }

    public double getResult() {
        return res;
    }



}
