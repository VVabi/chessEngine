package communication.VMPServer;

import communication.messages.message;

/**
 * Created by fabian on 05.07.16.
 */
public class Packet {
    private message m;
    private Header header;

    public Packet(message msg, int loc){
        m = msg;
        header = new Header(m.getSize(), loc);
    }

    public Packet(message msg, Header h){
        m = msg;
        header = h;
    }
    public message getMessage() { return m;}
    public int getTargetLocation() { return header.getTargetLocation(); }
    public Header getHeader() { return header;}
}
