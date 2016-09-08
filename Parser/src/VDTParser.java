
import java.io.IOException;
import java.io.PrintWriter;
import java.util.ArrayList;
import java.util.HashMap;

/**
 * Created by fabian on 21.12.15.
 */
public class VDTParser {

    static HashMap<String,String> typeHash;
    static HashMap<String,String> cppTypeHash;
    static HashMap<String,Integer> typeSizes;

    public static void parse(String javaDirectory, String messageLocation) throws IOException{
        String raw = Parser.readMessages(messageLocation);
        String[] structs = raw.split("struct:");
        ArrayList<Structure> structures = new ArrayList<Structure>();
        PrintWriter IDwriter = new PrintWriter(javaDirectory+"/VdtIDs.java", "UTF-8");
        IDwriter.println("package VDT;\n\npublic class VdtIDs{\n");
        Integer ID = 0;
        //PrintWriter cppWriter = new PrintWriter(cppDirectory+"/VDT.h", "UTF-8");
       // cppWriter.write("#include <src/DataTypes/fsarray.h>\n");
        //cppWriter.write("#include <string.h>\n");
        prepareTypeSizes();
        prepareTypeHash();
        for(int k = 1; k < structs.length; k++){
            Structure struct = new Structure(structs[k], true);
            structures.add(struct);
            PrintWriter writer = new PrintWriter(javaDirectory+"/"+struct.name+".java", "UTF-8");
            writer.print(struct.toJavaClass(typeHash, typeSizes));
            //cppWriter.print(struct.toCppClass(cppTypeHash, typeSizes, ID, true));
            writer.close();
            IDwriter.println("\tpublic static final int " + struct.name + "_unique_id=" + ID.toString() + ";");
            ID++;

        }
       // cppWriter.close();
        IDwriter.println("}\n");
        IDwriter.close();



    }

    private static void prepareTypeSizes(){
        typeSizes = new HashMap<String, Integer>();
        typeSizes.put("byte",1);
        typeSizes.put("short",2);
        typeSizes.put("int",4);
        typeSizes.put("double",8);

    }



    private static void prepareTypeHash(){
        typeHash = new HashMap<String, String>();
        typeHash.put("uint8","byte");
        typeHash.put("int8","byte");
        typeHash.put("uint16","short");
        typeHash.put("int16","short");
        typeHash.put("uint32","int");
        typeHash.put("int32","int");
        typeHash.put("double","double");
        cppTypeHash = new HashMap<String, String>();
        cppTypeHash.put("uint8", "uint8_t");
        cppTypeHash.put("int8", "int8_t");
        cppTypeHash.put("uint16", "uint16_t");
        cppTypeHash.put("int16", "int16_t");
        cppTypeHash.put("uint32", "uint32_t");
        cppTypeHash.put("int32", "int32_t");
        cppTypeHash.put("double", "double");
    }


}
