package uciClient;

import java.io.*;
import java.util.List;

/**
 * Created by vabi on 02.10.16.
 */
public class uciEngineHandler {


    Runtime runtime = Runtime.getRuntime();
    Process process;
    BufferedReader reader;
    BufferedWriter writer;


    private void put(String s) throws IOException{
        //System.out.println(s);
        writer.write(s+"\n");
        writer.flush();

    }
    public uciEngineHandler(String path) throws IOException{

        process = runtime.exec(path);
        reader = new BufferedReader(new InputStreamReader(process.getInputStream()));
        OutputStreamWriter output = new OutputStreamWriter(process.getOutputStream());
        writer = new BufferedWriter(output);
        put("uci");
        put("isready");

        while(true){

            String answer = reader.readLine();
            if("uciok".equals(answer)){
                break;
            }
        }


    }

    public String getCurrentPosition() throws IOException { //This is a dirty hack
        put("getPosition");
        while(true){
            String answer = reader.readLine();
           // System.out.println(answer);
            String[] splitted = answer.split(" ");
            if("currentPosition".equals(splitted[0])){
                return splitted[1];
            }
        }

    }


    public void setPosition(List<String> moveList) throws IOException{
        String putStr = "position startpos";

        if(moveList.size() > 0){
            putStr = putStr+" moves";

            for(String mv: moveList){
                putStr = putStr+" "+mv;

            }

        }
        //System.out.println(putStr);
        put(putStr);
    }


    public void startSearch() throws IOException{
        put("go depth 6");
    }

    public int readBestmove(String[] in) throws IOException {

        int eval = 0;
        while(true){

            String answer = reader.readLine();
            //System.out.println(answer);
            String[] splitted = answer.split(" ");
            if("bestmove".equals(splitted[0])){
                in[0] = splitted[1];
                break;
            }
            if("info".equals(splitted[0])){
                //System.out.println(answer);
                for(int index=0; index < splitted.length; index++){
                    if("score".equals(splitted[index])){
                        eval = Integer.parseInt(splitted[index+2]);

                    }

                }

            }

        }
        return eval;
    }

    public void close(){
        process.destroy();
    }
}
