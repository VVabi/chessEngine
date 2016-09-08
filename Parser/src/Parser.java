



import com.sun.xml.internal.bind.v2.model.core.ID;

import java.io.BufferedReader;
import java.io.FileReader;
import java.io.IOException;
import java.io.PrintWriter;
import java.util.ArrayList;
import java.util.HashMap;

public class Parser {

    String dataTypes[] = {"byte", "short", "int", "long"};

    String msgDataTypes[] = {"uint8","int8", "uint16","int16","uint32","int32"};

    HashMap<String,String> typeHash;
    HashMap<String,String> cppTypeHash;
    HashMap<String,Integer> typeSizes;
    ArrayList<Structure> structures;
    ArrayList<Structure> vdtStructures;

    public static void main(String[] args) throws IOException {
        Parser P = new Parser("Parser/src/messages.txt", "Parser/src/vdt.txt");
        String vdtLocation     = "Parser/src/vdt.txt";
        String messageDirectory= "Parser/src/vdt.txt";

        System.out.println(args[0]);
        System.out.println(args[1]);
        System.out.println(args[2]);
        System.out.println(args[3]);

        try {
            P.parseJava("testFolder", "testFolder");
            P.parseCpp("Cpp/src/communication/gen", "Cpp/src/communication/gen");
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    public Parser(String messageLocation, String vdtLocation) throws IOException {
        prepareTypeHash();
        prepareTypeSizes();
        String raw = readMessages(messageLocation);
        String[] structs = raw.split("struct:");
        structures = new ArrayList<Structure>();
        for(int k = 1; k < structs.length; k++) {
            Structure struct = new Structure(structs[k], false);
            structures.add(struct);
        }

        String vdtRaw = readMessages(vdtLocation);
        String[] vdtStructs = vdtRaw.split("struct:");
        vdtStructures = new ArrayList<Structure>();
        for(int k = 1; k < vdtStructs.length; k++) {
            Structure struct = new Structure(vdtStructs[k], true);
            vdtStructures.add(struct);
        }

    }





    public void parseJava(String messageDirectory, String vdtDirectory) throws IOException {

        PrintWriter IDwriter = new PrintWriter(messageDirectory+"/MessageIDs.java", "UTF-8");
        IDwriter.println("package messages;\n\npublic class MessageIDs{\n");
       // PrintWriter cppWriter = new PrintWriter(cppDirectory+"/VMPmessages.h");
       // cppWriter.write("#include <src/DataTypes/fsarray.h>\n");
       // cppWriter.write("#include <string.h>\n");
        Integer ID = 0;
        for(Structure struct: structures){
            PrintWriter writer = new PrintWriter(messageDirectory+"/"+struct.name+".java", "UTF-8");
            writer.print(struct.toJavaClass(typeHash, typeSizes));
            writer.close();
            IDwriter.println("\tpublic static final int " + struct.name + "_unique_id=" + ID.toString() + ";");
            ID++;

        }
        ID = 0;
        IDwriter.println("}\n");
        IDwriter.close();


        IDwriter = new PrintWriter(vdtDirectory+"/VdtIDs.java", "UTF-8");
        IDwriter.println("package VDT;\n\npublic class VdtIDs{\n");
        for(Structure struct: vdtStructures){
            PrintWriter writer = new PrintWriter(vdtDirectory+"/"+struct.name+".java", "UTF-8");
            writer.print(struct.toJavaClass(typeHash, typeSizes));
            writer.close();
            IDwriter.println("\tpublic static final int " + struct.name + "_unique_id=" + ID.toString() + ";");
            ID++;
        }

        IDwriter.println("}\n");
        IDwriter.close();
        //cppWriter.close();


        generateMessageFactory(messageDirectory);



    }


    public void parseCpp(String messageDirectory, String vdtDirectory) throws IOException {
        PrintWriter cppWriter = new PrintWriter(messageDirectory+"/VMPmessages.h");
        cppWriter.write("#ifndef VMP\n");
        cppWriter.write("#define VMP\n");
        cppWriter.write("#include <DataTypes/fsarray.h>\n");
        cppWriter.write("#include <string.h>\n");
        cppWriter.write("#include \"VDT.h\"\n");
        cppWriter.write("#include \"../Network/message.h\"\n");
        Integer ID = 0;
        for(Structure struct: structures){

            cppWriter.print(struct.toCppClass(cppTypeHash, typeSizes, ID));
            ID++;

        }
        cppWriter.write("#endif\n");
        cppWriter.close();
        cppWriter = new PrintWriter(vdtDirectory+"/VDT.h");
        cppWriter.write("#ifndef VDT\n");
        cppWriter.write("#define VDT\n");
        cppWriter.write("#include <DataTypes/fsarray.h>\n");
        cppWriter.write("#include <string.h>\n");
        cppWriter.write("#include \"VDTCore.hpp\"\n");
        ID = 0;
        for(Structure struct: vdtStructures){

            cppWriter.print(struct.toCppClass(cppTypeHash, typeSizes, ID));
            ID++;

        }
        cppWriter.write("#endif\n");
        cppWriter.close();



    }


    private void generateMessageFactory(String directory) throws IOException{
        String factory = "package messages;\n";
        factory = factory+"public class MessageFactory{\n";
        factory = factory+"static int header_size = 4;\n";
        factory = factory+"\tpublic static message getMessage(byte[] deflated) throws UnknownMessageException{\n";
        factory = factory+"\t\tint unique_id = typeConv.byte_to_int(deflated, 0);\n";
        factory = factory+"\t\tmessage m;\n";
        factory = factory+"\t\tswitch(unique_id){\n";
        for(int k = 0; k < structures.size(); k++){
            Structure currentStruct = structures.get(k);
            factory = factory+"\t\t\tcase MessageIDs."+currentStruct.name+"_unique_id:\n";
            factory = factory+"\t\t\t\tm = new "+currentStruct.name+"(deflated);\n";
            factory = factory+"\t\t\t\tbreak;\n";
        }
        factory = factory+"\t\t\tdefault:\n";
        factory = factory+"\t\t\t\tthrow new UnknownMessageException(new Integer(unique_id).toString());\n";
        factory = factory +"\t\t}\n";
        factory = factory+"\t\treturn m;\n";
        factory = factory+"\t}\n";
        factory = factory+"}\n";
        //System.out.println(factory);
        PrintWriter writer = new PrintWriter(directory+"/MessageFactory.java", "UTF-8");
        writer.write(factory);
        writer.close();



    }


   private void prepareTypeSizes(){
      typeSizes = new HashMap<String, Integer>();
       typeSizes.put("byte",1);
       typeSizes.put("short",2);
       typeSizes.put("int",4);
       typeSizes.put("double",8);
       typeSizes.put("long",8);

   }



    private void prepareTypeHash(){
        typeHash = new HashMap<String, String>();
        typeHash.put("uint8","byte");
        typeHash.put("int8","byte");
        typeHash.put("uint16","short");
        typeHash.put("int16","short");
        typeHash.put("uint32","int");
        typeHash.put("int32","int");
        typeHash.put("double","double");
        typeHash.put("uint64", "long");
        cppTypeHash = new HashMap<String, String>();
        cppTypeHash.put("uint8", "uint8_t");
        cppTypeHash.put("int8", "int8_t");
        cppTypeHash.put("uint16", "uint16_t");
        cppTypeHash.put("int16", "int16_t");
        cppTypeHash.put("uint32", "uint32_t");
        cppTypeHash.put("int32", "int32_t");
        cppTypeHash.put("double", "double");
        cppTypeHash.put("uint64", "uint64_t");
    }


    public static String readMessages(String messageLocation) throws IOException{
        BufferedReader br = new BufferedReader(new FileReader(messageLocation));
        try {
            StringBuilder sb = new StringBuilder();
            String line = br.readLine();

            while (line != null) {
                sb.append(line);
                sb.append(System.lineSeparator());
                line = br.readLine();
            }
            String everything = sb.toString();
            return everything;
        } finally {
            br.close();
        }




    }


}
