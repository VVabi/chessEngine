package core;

/**
 * Created by vabi on 08.09.16.
 */
public interface EngineEventInterface {


        public void addGuiEvent(ChessEvent e);
        public ChessEvent consumeEngineEvent();


}
