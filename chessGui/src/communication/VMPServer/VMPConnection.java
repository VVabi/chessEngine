package communication.VMPServer;

import communication.messages.MessageFactory;
import communication.messages.UnknownMessageException;
import communication.messages.message;
import communication. messages.typeConv;


import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import java.net.Socket;

/**
 * Created by fabian on 06.09.15.
 */
public class VMPConnection {

    Socket sock;
    DataOutputStream streamWriter;
    DataInputStream streamReader;

    public VMPConnection(Socket socket){
        sock = socket;
        try {
            streamWriter = new DataOutputStream(sock.getOutputStream());
            streamReader = new DataInputStream(sock.getInputStream());
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    public void sendBinaryData(byte[] memory) throws IOException {

        streamWriter.write(memory,0,memory.length);
        streamWriter.flush();
    }

    public void receiveBinaryData(byte[] memory, int size) throws IOException{
        int offset = 0;
        int receivedbytes;
        while(size > 0){
            receivedbytes = streamReader.read(memory,offset,size);
            if(receivedbytes < 0){
                //TODO: what does a result < 0 actually mean?
                receivedbytes = 0;
            }
            offset = offset+receivedbytes;
            size = size-receivedbytes;
        }
    }





}
