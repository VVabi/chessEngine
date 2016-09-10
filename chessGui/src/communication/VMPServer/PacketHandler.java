package communication.VMPServer;

import communication.messages.message;

/**
 * Created by fabian on 06.09.15.
 */
public interface PacketHandler {


    public void onPacket(Packet packet, int clientId);

    

}
