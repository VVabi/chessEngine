

import java.io.IOException;
import java.util.HashMap;

/**
 * Created by fabian on 04.09.15.
 */
public class Field{
    String name;
    String datatype;
    boolean isVDT;
    boolean isArray;

    public Field(String line) throws IOException {
        String[] parts = line.split("\\s+");

        if(parts.length != 2){
            System.out.println("ERROR: a line in the parser contains too many spaces");
            System.out.println(parts.length);
            System.out.println(parts[0]);
            throw new IOException(line+parts[0]);
        }
        name = parts[1];

        if((parts[0].length() > 5) && (parts[0].substring(0,6).equals("array:"))){
            isArray = true;
            parts[0] = parts[0].substring(6);

        }

        if((parts[0].length() > 3) && (parts[0].substring(0,4).equals("vdt:"))){
            isVDT    = true;
            parts[0] = parts[0].substring(4);

        }



        datatype = parts[0];
        if(isVDT){
            datatype = "VDT"+datatype;
        }


    }

    public void printField(){
        if(isArray){
            System.out.print("Array ");
        }

        System.out.print(datatype);
        System.out.print(" ");
        System.out.println(name);
    }

    public String toJavaLine(HashMap<String,String> typeHash){
        String ret = "\t";
        String arr = "";
        if(isArray){
            arr = "[]";
        }

        if(isVDT) {
            ret = ret+"public "+datatype+arr+" "+name+";\n";
        }else {
            ret = ret + "public " + typeHash.get(datatype) + arr + " " + name + ";\n";
        }
        return ret;
    }


    public String toCppLine(HashMap<String, String> typeHash, HashMap<String, Integer> typeSizes){
        String ret = "\t";
        String actualDataType = datatype;
        if(isVDT){

        }
        else{
            actualDataType = typeHash.get(actualDataType);

        }

        if(isArray){
            ret = ret +"fsarray<"+ actualDataType + "> " + name + ";\n";
        }
        else {

            ret = ret + actualDataType + " " + name + ";\n";
        }
        return ret;

    }

}
