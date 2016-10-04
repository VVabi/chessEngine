package uciClient;

import communication.VDT.VDTstring;
import communication.VMPServer.VMPServer;
import communication.messages.VMPchessPosition;
import core.events.NewPositionEvent;

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

    public UciProcessing() throws IOException {

        CountDownLatch cd = new CountDownLatch(1);
        server = new VMPServer(9999, 0, cd);
        server.connect("127.0.0.1", 9876);

        double total = 0;

        int wins = 0;
        int draws = 0;
        int losses = 0;
        for(int k=0; k < 100; k++) {
            //stockFishHandler = new uciEngineHandler("/home/vabi/code/stockfish-7-linux/Linux/stockfish");
            stockFishHandler = new uciEngineHandler("/home/vabi/code/chessEngine/Cpp/TestRelease/Vabi");
            vabiHandler = new uciEngineHandler("/home/vabi/code/chessEngine/Cpp/Release/Vabi");

            System.out.println("Started engines");
            List<String> moves = new ArrayList<>();
            int ret = 0;
            int eval = 0;
            String mv[] = new String[1];

            if((k % 2) == 1){
                stockFishHandler.setPosition(moves);
                stockFishHandler.startSearch();
                eval = stockFishHandler.readBestmove(mv);
                //System.out.println(eval);
                moves.add(mv[0]);
            }
            while (true) {


                vabiHandler.setPosition(moves);
                String current = vabiHandler.getCurrentPosition();
                VMPchessPosition position = new VMPchessPosition(new VDTstring(current.getBytes()));
                server.send(position, 0);
                vabiHandler.startSearch();



                eval = vabiHandler.readBestmove(mv);
                if (eval > 90000) {
                    ret = 1;
                    break;
                } else if (eval < -90000) {
                    ret = -1;
                    break;
                }

                //System.out.println(eval);
                moves.add(mv[0]);
                vabiHandler.setPosition(moves);
                current = vabiHandler.getCurrentPosition();
                position = new VMPchessPosition(new VDTstring(current.getBytes()));
                server.send(position, 0);
                //System.out.println(mv[0]);
                stockFishHandler.setPosition(moves);
                stockFishHandler.startSearch();
                eval = stockFishHandler.readBestmove(mv);
                //System.out.println(eval);
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

        }

        System.out.println("Final result: " + total);
        System.out.println("Wins "+wins + " Draws "+draws+" Losses "+losses);
        double elo_diff = 400*(Math.log10(1-total/100.0)-Math.log10(total/100.0));
        System.out.println("Elo difference is " +elo_diff);


    }





}
