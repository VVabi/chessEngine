package core.events;

import communication.VDT.VDTevaluation;
import core.ChessEvent;

/**
 * Created by vabi on 08.10.16.
 */
public class EvalDebugEvent extends ChessEvent {

    public VDTevaluation evalDebugStruct;
    public int valid;

    public EvalDebugEvent(VDTevaluation evalDebugStruct, int valid) {
        this.evalDebugStruct = evalDebugStruct;
        this.valid = valid;
    }

}
