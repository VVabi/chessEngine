package communication.VMPServer;


import communication.messages.VMPRegisterLocation;
import communication.messages.message;



import java.io.IOException;
import java.net.ServerSocket;
import java.net.Socket;
import java.util.*;
import java.util.concurrent.ConcurrentLinkedQueue;
import java.util.concurrent.CountDownLatch;
import java.util.concurrent.atomic.AtomicInteger;

/**
 * Created by fabian on 06.09.15.
 */
public class VMPServer {


    AtomicInteger nextAvailableId = new AtomicInteger(0);

    private ServerSocket serverSocket;
    private int listenport;
    ConcurrentLinkedQueue<ClientHandler> clientList;
    ConcurrentLinkedQueue<message> messageList;
    PacketHandler packetHandler;
    private Set<Integer> locations = new HashSet<Integer>();




    public int getListenPort(){
        return listenport;
    }

    public VMPServer(int port, Set<Integer> locations, CountDownLatch cd, boolean hub) throws IOException{


        PacketHandler p = new PacketHandler() {
            @Override
            public void onPacket(Packet packet, int clientId) {
                message m = packet.getMessage();
                if(m instanceof VMPRegisterLocation){
                    VMPRegisterLocation loc = (VMPRegisterLocation) m;
                    ClientHandler client = getClientById(clientId);
                    if(client == null){
                        System.out.println("Unknown client "+clientId);
                        System.out.println(m.getUniqueId());
                        for(ClientHandler c: clientList){
                            System.out.println("Have client with ID "+c.getClientID());
                        }
                    }else{
                        client.addLocation(loc.location);
                        System.out.println("Added location "+loc.location);

                    }
                }
                else{
                    System.out.println(m.getUniqueId());
                    System.out.println("Trying to forward...");
                    System.out.println(packet.getTargetLocation());
                    System.out.println(clientId);
                    for(ClientHandler client: clientList){
                        System.out.println(client.getClientID());
                        System.out.println(client.getLocations().toString());
                        if((client.getClientID() != clientId) && client.getLocations().contains(packet.getTargetLocation())) {
                            System.out.println("Forwarding packet...");
                            client.send_msg(packet);
                        }
                    }
                }
            }
        };
        init(port, locations, cd, p);

    }


    public VMPServer(int port, int location, CountDownLatch cd) throws IOException{
        Set<Integer> locations = new HashSet<Integer>();
        locations.add(location);
        PacketHandler p = new PacketHandler() {
            @Override
            public void onPacket(Packet p, int clientId) {
                message m = p.getMessage();
                if(m instanceof VMPRegisterLocation){
                    VMPRegisterLocation loc = (VMPRegisterLocation) m;
                    ClientHandler client = getClientById(clientId);
                    if(client == null){
                        System.out.println("Unknown client "+clientId);
                        System.out.println(m.getUniqueId());
                        for(ClientHandler c: clientList){
                            System.out.println("Have client with ID "+c.getClientID());
                        }
                    }else{
                        client.addLocation(loc.location);
                        System.out.println("Added location "+loc.location);

                    }
                }
                else {

                    messageList.add(m);
                }


            }
        };
        init(port, locations, cd, p);
    }


    public VMPServer(int port, Set<Integer> locs, CountDownLatch cd) throws IOException {
        PacketHandler p = new PacketHandler() {
            @Override
            public void onPacket(Packet p, int clientId) {
                message m = p.getMessage();
                if(m instanceof VMPRegisterLocation){
                    VMPRegisterLocation loc = (VMPRegisterLocation) m;
                    ClientHandler client = getClientById(clientId);
                    if(client == null){
                        System.out.println("Unknown client "+clientId);
                        System.out.println(m.getUniqueId());
                        for(ClientHandler c: clientList){
                            System.out.println("Have client with ID "+c.getClientID());
                        }
                    }else{
                        client.addLocation(loc.location);
                        System.out.println("Added location "+loc.location+ " in " + locations.toString());

                    }
                }
                else {

                    messageList.add(m);
                }


            }
        };
        init(port, locs, cd, p);
    }

    private void init(int port, Set<Integer> locs, CountDownLatch cd, PacketHandler p) throws IOException{
        locations = locs;

        clientList = new ConcurrentLinkedQueue<ClientHandler>();
        messageList = new ConcurrentLinkedQueue<message>();
        packetHandler = p;


        listenport = port;
        serverSocket = new ServerSocket(listenport);
        Runnable ServerListener = new Runnable() {
            @Override
            public void run() {
                while(true) {
                    Socket sock = null;
                    try {
                        sock = serverSocket.accept();
                        VMPConnection connection = new VMPConnection(sock);
                        ClientHandler client = new ClientHandler(packetHandler, connection, "default", nextAvailableId.getAndIncrement());

                        clientList.add(client);
                        Thread.sleep(100);
                        for(Integer location: locations){
                            VMPRegisterLocation registration = new VMPRegisterLocation(location);
                            client.send_msg(new Packet(registration, 0));


                        }
                        System.out.println("Incoming connection");

                    } catch (IOException e) {
                        e.printStackTrace();
                    } catch (InterruptedException e) {
                        e.printStackTrace();
                    }
                }
            }
        };

        Thread serverThread = new Thread(ServerListener);
        serverThread.start();
        cd.countDown();
    }


    public ClientHandler getClientById(int ID){
        for(ClientHandler client: clientList){
            if(client.getClientID() == ID){
                return client;
            }
        }
        return null;
    }

    public void connect(String host, int port){
        try {
            Socket sock = new Socket(host, port);
            VMPConnection connection = new VMPConnection(sock);
            ClientHandler client = new ClientHandler(packetHandler, connection, "default", nextAvailableId.getAndIncrement());

            clientList.add(client);
            Thread.sleep(100);
            for(Integer location: locations){
                VMPRegisterLocation registration = new VMPRegisterLocation(location);
                client.send_msg(new Packet(registration, 0));
            }

        } catch (IOException e) {
            e.printStackTrace();
        } catch (InterruptedException e) {
            e.printStackTrace();
        }

    }

    public void send(message m, int location){

        for(ClientHandler client: clientList){

            //if(client.getLocations().contains(location)){
                client.send_msg(new Packet(m, location));
                System.out.println("sent msg");
           // }

        }

    }

    public message getMessage(int unique_id){
        for (Iterator<message> iterator = messageList.iterator(); iterator.hasNext();) {
            message m = iterator.next();
            if (m.getUniqueId() == unique_id) {
                iterator.remove();
                return m;
            }
        }
        return null;


    }

}
