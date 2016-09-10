package communication.VMPServer;


import javafx.stage.Stage;

import java.io.*;
import java.net.ServerSocket;
import java.net.Socket;
import java.nio.charset.Charset;

public class Main {

    public static void main(String[] args) throws IOException {

       /* String s = "test_string\nfurther test";
        byte[] b = s.getBytes(Charset.forName("UTF-8"));
        String t = new String(b);
        System.out.println(t);*/

        /*int[] arr = new int[8];
        byte[] lash = new byte[8];
        for(int i=0; i<8; i++){
            arr[i] = i*i;
            lash[i] = (byte) (2*i);

        }

        NEXT_MSG n = new NEXT_MSG(arr, (short) 100, (byte) 32, lash, 789);

        byte[] deflated = n.deflate();
        System.out.println(deflated.length);
        System.out.println(deflated[4]);
        System.out.println(deflated[5]);
        System.out.println(deflated[6]);
        System.out.println(deflated[7]);
        System.out.println("sending...");*/

        /*VMPServer s1 = new VMPServer(5001);
        VMPServer s2 = new VMPServer(5002);
        s1.connect("localhost", 5002);

        TEST_DATA td = new TEST_DATA(17,981);
        TEST_DATA td2 = new TEST_DATA(356, -157);
        TEST_DATA[] arr = new TEST_DATA[37];
        arr[0] = td;
        arr[1] = td2;
        for(int i =2; i< 37; i++){
            arr[i] = td2;

        }
        ABC abc = new ABC((short) 56, arr, (byte)111);
        s1.send(abc);*/




        



    }
}
