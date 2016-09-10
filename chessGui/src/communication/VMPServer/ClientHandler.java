package communication.VMPServer;

//import com.sun.deploy.util.SessionState;
import com.sun.xml.internal.bind.v2.runtime.reflect.Lister;
import communication.messages.MessageFactory;
import communication.messages.UnknownMessageException;
import communication.messages.message;
import communication.messages.typeConv;

import java.io.IOException;
import java.util.HashSet;
import java.util.Set;
import java.util.concurrent.LinkedBlockingQueue;

/**
 * Created by fabian on 06.09.15.
 */
public class ClientHandler{
    PacketHandler packetHandler;
    private VMPConnection VMPsocket;
    private volatile LinkedBlockingQueue<message> transmit_list;
    String name;
    Set<Integer> locations = new HashSet<Integer>();
    private int clientID;

    public ClientHandler(PacketHandler p, VMPConnection connection, String nm, int ID){
        clientID = ID;
        name = nm;
        packetHandler = p;
        VMPsocket = connection;
        transmit_list = new LinkedBlockingQueue<message>();

        Runnable receiveRunnable = new Runnable() {
            @Override
            public void run() {
                try {
                    run_receive();
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
        };

       /* Runnable transmitRunnable = new Runnable() {
            @Override
            public void run() {
                try {
                    run_transmit();
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
        };*/

        Thread receiveThread = new Thread(receiveRunnable);
        //Thread transmitThread = new Thread(transmitRunnable);
        receiveThread.start();
        //transmitThread.start();

    }

    public Set<Integer> getLocations(){
        return locations;
    }

    public int getClientID(){
        return clientID;
    }


    public void addLocation(int location){
        locations.add(location);
    }

    public void send_msg(Packet p){

    Header h = p.getHeader();
    message m = p.getMessage();
    byte[] head = h.deflate();
    byte[] binary_msg = m.deflate();
    try {
        VMPsocket.sendBinaryData(head);
        VMPsocket.sendBinaryData(binary_msg);
    } catch (IOException e) {
        e.printStackTrace();
    }

    }


    private void run_receive() throws IOException{
        while(true) {
            Packet packet  = receivePacket();
            packetHandler.onPacket(packet, clientID);
        }

    }

    /*
    DEPTRECATED-DO NOT USE!
    private void run_transmit() throws IOException{

        while(true){
            if(!transmit_list.isEmpty())
            {
                message m = transmit_list.poll();
                byte[] binary_msg = m.deflate();
                VMPsocket.sendBinaryData(binary_msg);

            }
            else{
                //TODO: performance hack; otherwise we need 100% CPU. Should be handled in a better way.
                try {
                    Thread.sleep(20);
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
            }

        }
    }*/


    private int receive_int() throws IOException {
        byte[] mem = new byte[4];
        VMPsocket.receiveBinaryData(mem, 4);
        return typeConv.byte_to_int(mem, 0);
    }

    private Packet receivePacket() throws IOException{
        int length = receive_int();
        int target = receive_int();

        Header h = new Header(length, target);
        byte[] memory = new byte[length];
        VMPsocket.receiveBinaryData(memory, length);

        try {
            return new Packet(MessageFactory.getMessage(memory), h);
        } catch (UnknownMessageException e) {
            e.printStackTrace();
            return null;
        }

    }

}
