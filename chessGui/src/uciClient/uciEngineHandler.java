package uciClient;

import java.io.*;
import java.util.ArrayList;
import java.util.List;

/**
 * Created by vabi on 02.10.16.
 */
public class uciEngineHandler {


    Runtime runtime = Runtime.getRuntime();
    Process process;
    BufferedReader reader;
    BufferedWriter writer;
    ArrayList<String> moves = new ArrayList<>();

    private void put(String s) throws IOException{
        //System.out.println(s);
        writer.write(s+"\n");
        writer.flush();

    }
    public uciEngineHandler(String path, String workingDirectory) throws IOException{

        File pathToExecutable = new File(workingDirectory);
        ProcessBuilder pb = new ProcessBuilder(path);
        pb.directory(pathToExecutable.getAbsoluteFile());
        process = pb.start();
        reader = new BufferedReader(new InputStreamReader(process.getInputStream()));
        OutputStreamWriter output = new OutputStreamWriter(process.getOutputStream());
        writer = new BufferedWriter(output);
        put("uci");
        put("isready");

        while(true){

            String answer = reader.readLine();

            if("readyok".equals(answer)){
                break;
            }
        }


    }


    public List<String> debugSearchOutput(int depth) throws IOException {
        put("go depth "+depth);
        List<String> ret = new ArrayList<>();
        while(true) {
            String answer = reader.readLine();
            ret.add(answer);
            System.out.println(answer);

            if((answer.length() >= 8) && ("bestmove".equals(answer.substring(0, 8)))){
                break;
            }
        }
        return ret;
    }


    public int getEval() throws IOException {
        put("eval");
        String answer = reader.readLine();

        String[] parts = answer.split(" ");

        return Integer.parseInt(parts[1]);

    }

    public void setPosition(String fenPosition, List<String> moveList) throws IOException{
        String putStr = "position fen "+fenPosition;
        moves = new ArrayList<>(moveList);
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
        put("go depth 7");
    }

    public int readBestmove(String[] in) throws IOException {

        int eval = 0;
        while(true){

            String answer = reader.readLine();
            if(answer == null) {
                System.out.println("Engine seems to have crashed???");
                return -20000;
            }

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

                    if("mate".equals(splitted[index])){
                        int matein = Integer.parseInt(splitted[index+1]);
                        if(matein > 0) {
                            eval = 20000;
                        } else {
                            eval = -20000;
                        }
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
