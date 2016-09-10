package core;

import java.util.concurrent.ConcurrentLinkedQueue;

/**
 * Created by vabi on 08.09.16.
 */
public interface PlayingEventInterface {




    public void addEngineEvent(ChessEvent e);
    public void addGuiEvent(ChessEvent e);

    public ChessEvent consumeGuiEvent();


}
