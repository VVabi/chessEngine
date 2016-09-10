package communication.VMPServer;

import communication.messages.typeConv;

/**
 * Created by fabian on 05.07.16.
 */
public class Header {
    private int totalSize;
    private int targetLocation;

    Header(int sz, int target) {
        totalSize = sz;
        targetLocation = target;
    }

    byte[] deflate(){
        byte[] ret = new byte[8];
        typeConv.int_to_byte(totalSize, ret, 0);
        typeConv.int_to_byte(targetLocation, ret, 4);
        return ret;
    }

    public int getTargetLocation() {
        return targetLocation;
    }
}
