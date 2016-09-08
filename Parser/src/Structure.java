

import com.sun.org.apache.bcel.internal.generic.RET;
import com.sun.xml.internal.bind.v2.model.core.ID;

import java.io.IOException;
import java.util.ArrayList;
import java.util.HashMap;

/**
 * Created by fabian on 04.09.15.
 */

//TODO: Too long, refactor
//TODO: performance-wise, 4-byte aligning the data would probably be much faster (and cleaner)
public class Structure{
    boolean hasArray;
    ArrayList<Field> fieldlist;
    String name;
    int header_size = 4;
    boolean vdtFlag;
    boolean isStatic = true;

    public Structure(String info, boolean isVDT) throws IOException {
        String[] lines = info.split("\n");
        vdtFlag = isVDT;
        if(vdtFlag){
            name = "VDT" + lines[0].replace("\n", "");
        }else {
            name = "VMP" + lines[0].replace("\n", "");
        }
        String line = lines[1].replace("\n","");
        fieldlist = new ArrayList<Field>();
        int cnt =1;
        while(!"endstruct".equals(line)){
            Field f = new Field(line);
            if(f.isArray){
                hasArray = true;
            }
            fieldlist.add(f);
            cnt++;
            line = lines[cnt].replace("\n","");

        }


    }

    public void printStruct(){
        System.out.println(name);
        for(int k = 0; k < fieldlist.size(); k++){
            fieldlist.get(k).printField();
        }


    }

    public String toJavaClass(HashMap<String,String> typeHash, HashMap<String,Integer> typeSizes){
        String javaclass;
        if(!vdtFlag) {
            javaclass = "package messages;\nimport VDT.*;\npublic class " + name + " extends message{\n\tpublic static final int unique_id=MessageIDs." + name + "_unique_id;\n";
        }else{
            javaclass = "package VDT;\nimport messages.*;\npublic class " + name + " extends coreVDTType{\n\tpublic static final int unique_id=VdtIDs." + name + "_unique_id;\n";
        }

        javaclass = javaclass + "\tpublic int getUniqueId(){ return unique_id;}\n";
        for(int k = 0; k < fieldlist.size(); k++){
            Field current_field = fieldlist.get(k);
            javaclass = javaclass + current_field.toJavaLine(typeHash);

        }
        javaclass = javaclass+ javaDeflateGenerator(typeHash, typeSizes);
        javaclass = javaclass+ javaInflateGenerator(typeHash, typeSizes);
        //if(vdtFlag) {
            javaclass = javaclass + javaGetSizeGenerator(typeHash, typeSizes, vdtFlag);
        //}
        javaclass = javaclass+ javaConstructorGenerator(typeHash);
        javaclass = javaclass+"}\n";

        return javaclass;

    }

    public String cppDisposeGenerator(HashMap<String, String> typeHash, HashMap<String, Integer> typeSizes){
        String ret = "\tvoid dispose(){\n";

        for(Field field: fieldlist){

            if(field.isVDT && field.isArray) {
                ret = ret+"\t\tfor(uint32_t i=0; i<"+field.name+".length; i++){\n";
                ret = ret+"\t\t\t"+field.name+"[i].dispose();\n";
                ret = ret+"\t\t}\n";
            } else if(field.isVDT ){
                ret = ret+"\t\t"+field.name+".dispose();\n";
            }

            if(field.isArray){
                ret = ret+"\t\t"+field.name+".free_array();\n";
            }


        }
        ret = ret+"\t}\n";
        return ret;
    }

    public String cppCopyGenerator(HashMap<String, String> typeHash, HashMap<String, Integer> typeSizes) {
        String ret ="\t"+name+" copy(){\n";
        ret = ret+"\t\t return "+name+"(\n";
        for(Field field: fieldlist) {

            if(field.isArray && field.isVDT){
                ret = ret+"\t\t\t"+field.name+".vdt_copy(),\n";
            } else if(field.isVDT){
                ret = ret+"\t\t\t"+field.name+".copy(),\n";
            } else if(field.isArray){
                ret = ret+"\t\t\t"+field.name+".shallow_copy(),\n";
            } else {
                ret = ret+"\t\t\t"+field.name+",\n";
            }


        }
        ret = ret.substring(0, ret.length()-2);
        ret = ret+"\n);\n";
        ret = ret+"\t}\n";
        return ret;

    }

    public String cppConstructorGenerator(HashMap<String, String> typeHash, HashMap<String, Integer> typeSizes){
        String ret = "\t"+name+"(\n";

        for(Field field : fieldlist){
            //TODO: this is the task of the field class really
            String datatype;
            if(field.isVDT) {
                datatype = field.datatype;
            }
            else {
                datatype = typeHash.get(field.datatype);
            }

            if(field.isArray) {
                datatype = "fsarray<"+datatype+">";
            }

            ret = ret+"\t\t"+datatype+" "+field.name+"_c,\n";

        }
        //C++ doesnt allow comma after last function argument
        ret = ret.substring(0, ret.length()-2);
        ret = ret+"\n";
        ret = ret+"\t){\n";
        for(Field field : fieldlist){
            ret = ret+"\t\t"+field.name+" = "+field.name+"_c;\n";
        }


        ret = ret+"\t}\n";

        ret = ret+"\t"+name+"() {\n";
        for(Field field : fieldlist){
            if(!field.isVDT && !field.isArray){
                ret = ret+"\t\t"+field.name+"=0;\n";
            }
        }
        ret = ret+"\t}\n";

        return ret;
    }

    public String cppGetSizeGenerator(HashMap<String, String> typeHash, HashMap<String, Integer> typeSizes){
        String ret = "\tuint32_t getSize(){\n";
        String total_size = "0";
        ret = ret + "\t\tuint32_t total_size=";
        for (int k = 0; k < fieldlist.size(); k++) {
            Field current_field = fieldlist.get(k);
            if(current_field.isVDT){
                if (current_field.isArray) {
                    total_size = total_size + "+getArraySize("+current_field.name+".data, "+current_field.name+".length)+4";
                }else{
                    total_size = total_size+ "+" +current_field.name+".getSize()";
                }
            }
            else {
                String sizeof = "sizeof("+typeHash.get(current_field.datatype)+")";
                total_size = total_size + "+" + sizeof;
                if (current_field.isArray) {
                    total_size = total_size + "*" + current_field.name + ".length+4";
                }
            }
        }
        ret = ret + total_size + ";\n";
        ret = ret+ "\t\treturn total_size;\n\t}\n";
        return ret;

    }


    public String javaGetSizeGenerator(HashMap<String, String> typeHash, HashMap<String, Integer> typeSizes, boolean vdtFlag){
        String ret = "\tpublic int getSize(){\n";
        String total_size = "0"; //this is WITHOUT the header!!
        if(!vdtFlag) {
            total_size = "4";
        }
        ret = ret + "\t\tint total_size="; //the unique id
        for (int k = 0; k < fieldlist.size(); k++) {
            Field current_field = fieldlist.get(k);
            if(current_field.isVDT){
                if (current_field.isArray) {
                    total_size = total_size + "+coreVDTType.getArraySize("+current_field.name+")+4";
                }else{
                    total_size = total_size+ "+" +current_field.name+".getSize()";
                }
            }
            else {
                String sizeof = typeSizes.get(typeHash.get(current_field.datatype)).toString();
                total_size = total_size + "+" + sizeof;
                if (current_field.isArray) {
                    total_size = total_size + "*" + current_field.name + ".length+4";
                }
            }
        }
        ret = ret + total_size + ";\n";
        ret = ret+ "\t\treturn total_size;\n\t}\n";
        return ret;

    }

    public String toCppClass(HashMap<String, String> typeHash, HashMap<String, Integer> typeSizes, int ID){
        String Cppclass = "#define "+name+"_UNIQUE_ID "+ID+"\n\n";
        if(vdtFlag) {
            Cppclass = Cppclass + "class " + name + ":public vdtCoreType{\n";
        }
        else{
            Cppclass = Cppclass + "class " + name + ":public message{\n";
        }
        Cppclass = Cppclass+"\tpublic:\n";
        Cppclass = Cppclass+"\tstatic const uint32_t unique_id = "+name+"_UNIQUE_ID;\n";
        Cppclass = Cppclass+"\tuint32_t getUniqueId(){ return unique_id;}\n";

        for(int k = 0; k < fieldlist.size(); k++){
            Field current_field = fieldlist.get(k);
            Cppclass = Cppclass + current_field.toCppLine(typeHash, typeSizes);

        }

        Cppclass = Cppclass+ CppDeflateGenerator(typeHash);
        Cppclass = Cppclass + CppInflateGenerator(typeHash);
        Cppclass = Cppclass + cppGetSizeGenerator(typeHash, typeSizes);
        Cppclass = Cppclass + cppDisposeGenerator(typeHash, typeSizes);
        Cppclass = Cppclass + cppConstructorGenerator(typeHash, typeSizes);
        Cppclass = Cppclass + cppCopyGenerator(typeHash, typeSizes);
        if(!vdtFlag) {
            Cppclass = Cppclass + "\t~" + name + "(){ dispose();}\n";
        }
        //Cppclass = Cppclass  + "\t"+name+"(){}\n";
        Cppclass = Cppclass+"};\n";
        return Cppclass;

    }

    public String javaDeflateGenerator(HashMap<String, String> typeHash, HashMap<String, Integer> typeSizes) {
        String ret;
        if(!vdtFlag) {
            ret = "\tpublic byte[] deflate(){\n";
        }else{
            ret = "\tpublic void deflate(byte[] mem, int offset_correction){\n";
        }
        String total_size = "4"; //this is WITHOUT the header!!
        int size_counter = 0;
        if(!vdtFlag){
            ret = ret + "\t\tint total_size="; //the unique id
            for (int k = 0; k < fieldlist.size(); k++) {
                Field current_field = fieldlist.get(k);
                if(current_field.isVDT){

                    if (current_field.isArray) {
                        total_size = total_size + "+coreVDTType.getArraySize("+current_field.name+")+4;\n";
                    }else{
                        total_size = total_size+ "+" +current_field.name+".getSize()";
                    }
                }
                else {
                    String sizeof = typeSizes.get(typeHash.get(current_field.datatype)).toString();
                    total_size = total_size + "+" + sizeof;
                    if (current_field.isArray) {
                        total_size = total_size + "*" + current_field.name + ".length+4";
                    }
                }
            }

        ret = ret + total_size + ";\n";
        }
        if(!vdtFlag) {
            ret = ret + "\t\tbyte[] mem = new byte[total_size];\n";
            ret = ret + "\t\tint offset_correction = 0;\n";
            ret = ret + "\t\ttypeConv.int_to_byte(unique_id,mem,0);\n";
            size_counter = 4;
        }



        //TODO: this is WITH the header. Somehow this is inconsistent




        for (int k = 0; k < fieldlist.size(); k++) {

            Field current_field = fieldlist.get(k);
            String current_name = current_field.name;

            if (current_field.isVDT) {


                if (current_field.isArray) {
                    //ret = ret+"\t\tint struct_size = "+current_field.name+".getSize();\n";
                    ret = ret + "\t\ttypeConv.int_to_byte(" + current_name + ".length,mem," + size_counter + "+offset_correction);\n";
                    size_counter = size_counter + 4;
                    ret = ret + "\t\tfor(int i = 0; i < " + current_name + ".length; i++){\n";
                    ret = ret + "\t\t\t"+current_name+"[i].deflate(mem," + size_counter + "+offset_correction);\n";
                    ret = ret + "\t\t\toffset_correction = offset_correction+"+current_field.name+"[i].getSize();\n";


                    ret = ret + "\t\t}\n";
                }
                else{
                    ret=ret+"\t\t"+current_field.name+".deflate(mem,"+size_counter+"+offset_correction);\n";
                    ret=ret+"\t\toffset_correction = offset_correction+"+current_field.name+".getSize();\n";
                }

            } else {
                int size = typeSizes.get(typeHash.get(fieldlist.get(k).datatype));
                String type = typeHash.get(current_field.datatype);
                if (current_field.isArray) {
                    ret = ret + "\t\ttypeConv.int_to_byte(" + current_name + ".length,mem," + size_counter + "+offset_correction);\n";
                    ret = ret + "\t\tfor(int i = 0; i < " + current_name + ".length; i++){\n";
                    size_counter = size_counter + 4;
                    if (type.equals("int")) {
                        ret = ret + "\t\t\ttypeConv.int_to_byte(" + current_name + "[i],mem," + size_counter + "+offset_correction);\n";
                        ret = ret + "\t\t\toffset_correction = offset_correction+4;\n";
                    } else if (type.equals("short")) {
                        ret = ret + "\t\t\ttypeConv.short_to_byte(" + current_name + "[i],mem," + size_counter + "+offset_correction);\n";
                        ret = ret + "\t\t\toffset_correction = offset_correction+2;\n";
                    } else if (type.equals("byte")) {
                        ret = ret + "\t\t\tmem[" + size_counter + "+offset_correction]=" + current_name + "[i];\n";
                        ret = ret + "\t\t\toffset_correction = offset_correction+1;\n";

                    }
                    else if (type.equals("double")) {
                        ret = ret + "\t\t\ttypeConv.double_to_byte(" + current_name + "[i],mem," + size_counter + "+offset_correction);\n";
                        ret = ret + "\t\t\toffset_correction = offset_correction+8;\n";
                    }
                    ret = ret + "\t\t}\n";
                } else {
                    if (type.equals("int")) {
                        ret = ret + "\t\ttypeConv.int_to_byte(" + current_name + ",mem," + size_counter + "+offset_correction);\n";
                    } else if (type.equals("short")) {
                        ret = ret + "\t\ttypeConv.short_to_byte(" + current_name + ",mem," + size_counter + "+offset_correction);\n";
                    } else if (type.equals("byte")) {
                        ret = ret + "\t\tmem[offset_correction + " + size_counter + "]=" + current_name + ";\n";
                    }
                    else if (type.equals("double")) {
                        ret = ret + "\t\ttypeConv.double_to_byte(" + current_name + ",mem," + size_counter + "+offset_correction);\n";
                    }

                    size_counter = size_counter + size;

                }

            }
        }
        if(!vdtFlag) {
            ret = ret + "\t\treturn mem;\n";
        }
        ret = ret+ "}\n";
        return ret;


        }


    public String javaInflateGenerator(HashMap<String, String> typeHash, HashMap<String, Integer> typeSizes){
        String ret = "";
        if(!vdtFlag) {
            ret = "\tpublic " + name + "(byte[] mem){\n";
            ret = ret+"\t\tint offset_correction = 0;\n";
        }else{
            ret = "\tpublic " + name + "(byte[] mem, int offset_correction){\n";
        }

        //ret = ret+"\t\tunique_id =MessageIDs."+name+"_unique_id;\n";

        //TODO: we are parsing without the header. Somehow this is inconsistent
        int size_counter = 0;
        if(!vdtFlag){
            size_counter = 4;
        }
        if(hasArray){
            ret = ret+"\t\tint array_size = 0;\n";
        }
        for (int k = 0; k < fieldlist.size(); k++) {

            Field current_field = fieldlist.get(k);

            String current_name = current_field.name;

            if(current_field.isVDT){
                if (current_field.isArray) {
                    //ret = ret+"\t\tint struct_size = "+current_field.datatype+".getSize();\n";
                    ret = ret + "\t\tarray_size=typeConv.byte_to_int(mem," + size_counter + "+offset_correction);\n";
                    size_counter = size_counter + 4;
                    ret = ret + "\t\t\t" + current_name + "= new " + current_field.datatype + "[array_size];\n";
                    ret = ret + "\t\tfor(int i=0;i<array_size;i++){\n";
                    ret = ret + "\t\t\t" + current_name + "[i] = new "+current_field.datatype+"(mem," + size_counter + "+offset_correction);\n";
                    ret = ret + "\t\toffset_correction = offset_correction+"+current_field.name+"[i].getSize();\n";
                    ret = ret + "\t\t}\n";
                }
                else {
                    ret = ret + "\t\t" + current_name + "= new " + current_field.datatype + "(mem," + size_counter + "+offset_correction);\n";
                    ret = ret + "\t\toffset_correction = offset_correction+"+current_field.name+".getSize();\n";
                }
            }
            else {
                int size = typeSizes.get(typeHash.get(fieldlist.get(k).datatype));
                String type = typeHash.get(current_field.datatype);
                if (current_field.isArray) {
                    ret = ret + "\t\tarray_size=typeConv.byte_to_int(mem," + size_counter + "+offset_correction);\n";
                    size_counter = size_counter + 4;
                    ret = ret + "\t\t\t" + current_name + "= new " + type + "[array_size];\n";
                    ret = ret + "\t\tfor(int i=0;i<array_size;i++){\n";
                    if (type.equals("int")) {
                        ret = ret + "\t\t\t" + current_name + "[i] = typeConv.byte_to_int(mem," + size_counter + "+offset_correction);\n";
                        ret = ret + "\t\t\toffset_correction = offset_correction+4;\n";
                    } else if (type.equals("short")) {
                        ret = ret + "\t\t\t" + current_name + "[i] = typeConv.byte_to_short(mem," + size_counter + "+offset_correction);\n";
                        ret = ret + "\t\t\toffset_correction = offset_correction+2;\n";
                    } else if (type.equals("byte")) {
                        ret = ret + "\t\t\t" + current_name + "[i] = mem[" + size_counter + "+offset_correction];\n";
                        ret = ret + "\t\t\toffset_correction = offset_correction+1;\n";

                    }
                    else if (type.equals("double")) {
                        ret = ret + "\t\t\t" + current_name + "[i] = typeConv.byte_to_double(mem," + size_counter + "+offset_correction);\n";
                        ret = ret + "\t\t\toffset_correction = offset_correction+8;\n";
                    }

                ret = ret + "\t\t}\n";
                } else {
                    if (type.equals("int")) {
                        ret = ret + "\t\t" + current_name + "=typeConv.byte_to_int(mem," + size_counter + "+offset_correction);\n";
                    } else if (type.equals("short")) {
                        ret = ret + "\t\t" + current_name + "=typeConv.byte_to_short(mem," + size_counter + "+offset_correction);\n";
                    } else if (type.equals("byte")) {
                        ret = ret + "\t\t" + current_name + "=mem[" + size_counter + "+offset_correction];\n";

                    } else if (type.equals("double")) {
                        ret = ret + "\t\t" + current_name + "=typeConv.byte_to_double(mem," + size_counter + "+offset_correction);\n";
                    }
                    size_counter = size_counter + size;
                }
            }
        }
        ret = ret+"\t}\n";
        return ret;
    }

    public String javaConstructorGenerator(HashMap<String, String> typeHash){
        String ret = "";
        ret = "\tpublic "+name+"(";
        for (int k = 0; k < fieldlist.size(); k++) {
            Field current_field = fieldlist.get(k);

            if(current_field.isVDT){
                ret = ret + current_field.datatype+" " +current_field.name + "_constr";
                if (current_field.isArray) {
                    ret = ret + "[]";
                }
            }
            else {
                String type = typeHash.get(current_field.datatype);
                String current_name = current_field.name;
                ret = ret + type;
                if (current_field.isArray) {
                    ret = ret + "[]";
                }
                ret = ret + " " + current_name + "_constr";

            }
            if (k < fieldlist.size() - 1) {
                ret = ret + ",";
            }
        }
        ret = ret+"){\n";
        for (int k = 0; k < fieldlist.size(); k++) {
            Field current_field = fieldlist.get(k);
            if(current_field.isVDT){
                ret = ret + "\t\t" +current_field.name + "="+current_field.name+"_constr;\n";
            }
            else {
                String type = typeHash.get(current_field.datatype);
                String current_name = current_field.name;
                ret = ret + "\t\t" + current_name + "=" + current_name + "_constr;\n";
            }
        }


        ret = ret+"\t}\n";
        return ret;
    }


    public String CppDeflateGenerator(HashMap<String, String> typeHash) {
        String ret = "";


        ret = "\tvoid deflate(uint8_t* mem, int offset_correction){\n";
        if(!vdtFlag) {
            ret = ret + "\t\tuint32_t x_unique_id = unique_id;\n";
            ret = ret + "\t\tmemcpy(mem+offset_correction, &x_unique_id, 4);\n";
            ret = ret + "\t\toffset_correction = offset_correction+4;\n";
        }

        //int size_counter = header_size;



        for (int k = 0; k < fieldlist.size(); k++) {

            Field current_field = fieldlist.get(k);
            String current_name = current_field.name;


            if (current_field.isVDT) {


                if (current_field.isArray) {
                   // ret = ret+"\t\tuint32_t struct_size = getArraySize("+current_field.name+".data, "+current_field.name+".length);\n";
                    ret = ret + "\t\tmemcpy(mem+offset_correction, &"+current_name+".length, 4);\n";
                    ret = ret + "\t\toffset_correction = offset_correction+4;\n";

                    ret  = ret+"\t\tfor(uint32_t ind=0; ind<"+current_name+".length; ind++){\n";

                    ret = ret+"\t\t\t"+current_name+"[ind].deflate(mem, offset_correction);\n";
                    ret = ret+"\t\t\toffset_correction = offset_correction+"+current_name+"[ind].getSize();\n";
                    //ret = ret + "\t\t\tmemcpy(mem+"+size_counter+"+offset_correction, " + current_name+ ".data, struct_size*"+current_name+".length);\n";
                    //ret = ret + "\t\t\toffset_correction = offset_correction+struct_size*"+current_name+".length;\n";

                    ret = ret+"\t\t}\n";




                }
                else{
                    ret=ret+"\t\t"+current_field.name+".deflate(mem,offset_correction);\n";
                    ret=ret+"\t\toffset_correction = offset_correction+"+current_field.name+".getSize();\n";
                }

            } else {

                if (current_field.isArray) {
                    ret = ret + "\t\tmemcpy(mem+offset_correction, &"+current_name+".length, 4);\n";
                    ret = ret + "\t\toffset_correction = offset_correction+4;\n";
                    ret = ret + "\t\tmemcpy(mem+offset_correction, "+current_name+".data, sizeof("+typeHash.get(current_field.datatype)+")*"+current_name+".length);\n";
                    ret = ret + "\t\toffset_correction = offset_correction+sizeof("+typeHash.get(current_field.datatype)+")*"+current_name+".length;\n";
                } else {
                    ret = ret + "\t\tmemcpy(mem+offset_correction, &"+current_name+", sizeof("+typeHash.get(current_field.datatype)+"));\n";
                    ret=ret+"\t\toffset_correction = offset_correction+sizeof("+typeHash.get(current_field.datatype)+");\n";
                }

            }
        }

        ret = ret+"\n\t}\n";

        return ret;


    }

    public String CppInflateGenerator(HashMap<String, String> typeHash){
        String ret = "";


            ret = "\t" + name + "(uint8_t* mem, int offset_correction){\n";


        //ret = ret+"\t\tunique_id =MessageIDs."+name+"_unique_id;\n";

        //TODO: we are parsing without the header. Somehow this is inconsistent
        //int size_counter = 4;
        if(hasArray){
            ret = ret+"\t\tuint32_t array_size = 0;\n";
        }
        for (int k = 0; k < fieldlist.size(); k++) {

            Field current_field = fieldlist.get(k);

            String current_name = current_field.name;

            if(current_field.isVDT){
                if (current_field.isArray) {
                    //ret = ret+"\t\tuint32_t struct_size = getArraySize("+current_field.name+".data, "+current_field.name+".length);\n";
                    ret = ret + "\t\tmemcpy(&array_size, mem+offset_correction, 4);\n";
                    ret = ret + "\t\toffset_correction = offset_correction+4;\n";
                    ret = ret + "\t\t" + current_name + "= fsarray<" + current_field.datatype + ">(array_size);\n";
                    ret = ret + "\t\tfor(uint32_t i=0;i<array_size;i++){\n";
                    ret = ret + "\t\t\t" + current_name + "[i] = "+current_field.datatype+"(mem, offset_correction);\n";
                    ret = ret + "\t\t\toffset_correction = offset_correction+"+current_name+"[i].getSize();\n";
                    ret = ret + "\t\t}\n";
                }
                else {
                    ret = ret + "\t\t" + current_name + "= " + current_field.datatype + "(mem, offset_correction);\n";
                    ret=ret+"\t\toffset_correction = offset_correction+"+current_field.name+".getSize();\n";
                }
            }
            else {

                if (current_field.isArray) {
                    ret = ret + "\t\tmemcpy(&array_size, mem+offset_correction, 4);\n";
                    ret = ret + "\t\toffset_correction = offset_correction+4;\n";
                    ret = ret + "\t\t" + current_name + "= fsarray< " + typeHash.get(current_field.datatype) + ">(array_size);\n";
                    ret = ret + "\t\tmemcpy("+current_name+".data, mem+offset_correction, array_size*sizeof("+typeHash.get(current_field.datatype)+"));\n";
                    ret = ret + "\t\toffset_correction = offset_correction+array_size*sizeof("+typeHash.get(current_field.datatype)+");\n";

                } else {
                    ret = ret + "\t\tmemcpy(&"+current_name+",mem+offset_correction,  sizeof("+typeHash.get(current_field.datatype)+"));\n";
                    ret = ret + "\t\toffset_correction = offset_correction+sizeof("+typeHash.get(current_field.datatype)+");\n";
                }
            }
        }
        ret = ret+"\t}\n";
        return ret;
    }


}
