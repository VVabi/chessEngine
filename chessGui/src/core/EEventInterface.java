package core;

import java.util.concurrent.ConcurrentLinkedQueue;

/**
 * Created by vabi on 08.09.16.
 */
public class EEventInterface implements EngineEventInterface {

    ConcurrentLinkedQueue<ChessEvent> engineEventQueue;
    ConcurrentLinkedQueue<ChessEvent> guiEventQueue;

    EEventInterface(){
        engineEventQueue = new ConcurrentLinkedQueue<ChessEvent>();
        guiEventQueue    = new ConcurrentLinkedQueue<ChessEvent>();
    }

    public void addEngineEvent(ChessEvent e) {
        engineEventQueue.add(e);
    }


    public ChessEvent consumeGuiEvent() {
        return guiEventQueue.poll();
    }

    @Override
    public void addGuiEvent(ChessEvent e) {
        guiEventQueue.add(e);
    }

    @Override
    public ChessEvent consumeEngineEvent() {
        return engineEventQueue.poll();
    }
}
