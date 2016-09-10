package core;

import java.util.concurrent.ConcurrentLinkedQueue;

/**
 * Created by vabi on 08.09.16.
 */
public class PEventInterface implements PlayingEventInterface {

    ConcurrentLinkedQueue<ChessEvent> engineEventQueue;
    ConcurrentLinkedQueue<ChessEvent> guiEventQueue;

    PEventInterface(){
        engineEventQueue = new ConcurrentLinkedQueue<ChessEvent>();
        guiEventQueue = new ConcurrentLinkedQueue<ChessEvent>();
    }

    @Override
    public void addEngineEvent(ChessEvent e) {
        System.out.println("Adding engine event to pevent queue");
        engineEventQueue.add(e);
    }

    @Override
    public void addGuiEvent(ChessEvent e) {
        guiEventQueue.add(e);
    }

    @Override
    public ChessEvent consumeGuiEvent() {
        return guiEventQueue.poll();
    }

    public ChessEvent consumeEngineEvent()  { return engineEventQueue.poll(); }
}
