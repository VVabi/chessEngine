package core;

import PlayingGUI.PlayingGUI;
import communication.VDT.VDTstring;
import communication.VMPServer.VMPServer;
import communication.messages.*;
import core.events.*;
import javafx.stage.Stage;

import java.io.IOException;
import java.util.concurrent.CountDownLatch;
import java.util.concurrent.Executors;
import java.util.concurrent.ScheduledExecutorService;
import java.util.concurrent.TimeUnit;

/**
 * Created by vabi on 08.09.16.
 */
public class Core {
    VMPServer server;
    EEventInterface engineEventInterface;
    PEventInterface playingEventInterface;
    public Core(Stage stage) throws IOException {
        CountDownLatch cd = new CountDownLatch(1);
        server = new VMPServer(9876, 0, cd);
        try {
            cd.await();
        } catch (InterruptedException e) {
            e.printStackTrace();
        }

        playingEventInterface = new PEventInterface();

        PlayingGUI gui = new PlayingGUI(stage, playingEventInterface);

        engineEventInterface = new EEventInterface();

        Runnable r = new Runnable() {
            @Override
            public void run() {
                    eventLoop();
            }
        };

        Runnable playingToEngineRunnable = new Runnable() {
            public void run() {
                ChessEvent event = playingEventInterface.consumeEngineEvent();
                while(event != null) {
                    engineEventInterface.addEngineEvent(event);
                    event = playingEventInterface.consumeEngineEvent();
                }
            }
        };

        ScheduledExecutorService executor = Executors.newScheduledThreadPool(1);
        executor.scheduleAtFixedRate(playingToEngineRunnable, 0, 20, TimeUnit.MILLISECONDS);

        Runnable engineToPlayingRunnable = new Runnable() {
            public void run() {
                ChessEvent gevent = engineEventInterface.consumeGuiEvent();
                while(gevent != null){
                    playingEventInterface.addGuiEvent(gevent);
                    gevent = engineEventInterface.consumeGuiEvent();
                }
            }
        };
        ScheduledExecutorService executor2 = Executors.newScheduledThreadPool(1);
        executor2.scheduleAtFixedRate(engineToPlayingRunnable, 0, 20, TimeUnit.MILLISECONDS);

        ScheduledExecutorService executor3 = Executors.newScheduledThreadPool(1);
        executor3.scheduleAtFixedRate(r, 0, 20, TimeUnit.MILLISECONDS);
    }


    void eventLoop(){
        //TODO: move to another place
        ChessEvent e = engineEventInterface.consumeEngineEvent();
        if(e != null){
            System.out.println("Got engine event");
            if(e instanceof CheckMoveEvent){
                CheckMoveEvent ev = (CheckMoveEvent) e;
                VMPcheckMove mv = new VMPcheckMove(new VDTstring(ev.move.getBytes()));
                server.send(mv, 0);
            }
            else if(e instanceof UndoMoveEvent) {
                VMPundoMove undo = new VMPundoMove((byte) 1);
                server.send(undo, 0);
            }
            else if(e instanceof ForceMoveEvent){
                VMPforceMove force = new VMPforceMove((byte) 1);
                server.send(force, 0);
            }
            else if(e instanceof NewPositionEvent){
                NewPositionEvent ev = (NewPositionEvent) e;
                VMPchessPosition position = new VMPchessPosition(new VDTstring(ev.newPosition.getBytes()));
                server.send(position, 0);
            }

        }
        message m = server.getMessage(MessageIDs.VMPchessPosition_unique_id);
        if(m != null) {

                VMPchessPosition pos = (VMPchessPosition) m;
                String position = new String(pos.position.chars);
                NewPositionEvent newPositionEvent = new NewPositionEvent();
                newPositionEvent.newPosition = position;
                engineEventInterface.addGuiEvent(newPositionEvent);
      }
        m = server.getMessage(MessageIDs.VMPsearchInfo_unique_id);
        if(m != null) {
            VMPsearchInfo info = (VMPsearchInfo) m;
            SearchDebugEvent ev = new SearchDebugEvent(info.nodes, info.time, info.eval, info.depth, new String(info.moveString.chars));
            engineEventInterface.addGuiEvent(ev);
        }
    }
}
