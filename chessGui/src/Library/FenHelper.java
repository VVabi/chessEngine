package Library;

import com.sun.xml.internal.ws.util.StringUtils;

/**
 * Created by vabi on 16.03.17.
 */
public class FenHelper {


    public static String mirror(String FEN) {

        String[] parts = FEN.split(" ");

        String[] rows = parts[0].split("/");


        String ret = "";

        for(int ind=0; ind < 8; ind++) {
            ret = ret+ (new StringBuilder(rows[ind]).reverse().toString());

            if(ind < 7) {
                ret = ret+"/";
            }
        }


        ret = ret+" "+parts[1]+" - "+" - "+parts[4]+" "+parts[5];

        return ret;
    }

    public static String changeSide(String FEN){

        String[] parts = FEN.split(" ");

        String[] rows = parts[0].split("/");


        String ret = "";

        for(int ind=7; ind > -1; ind--) {
            ret = ret+ reverseCase(rows[ind]);

            if(ind > 0) {
                ret = ret+"/";
            }
        }


        ret = ret+" ";
        if("w".equals(parts[1])) {
            ret = ret+"b"+" ";
        } else {
            ret = ret+"w"+" ";
        }


        if(parts[2].contains("k")) {
            ret = ret+"K";
        }
        if(parts[2].contains("q")) {
            ret = ret+"Q";
        }
        if(parts[2].contains("K")) {
            ret = ret+"k";
        }
        if(parts[2].contains("Q")) {
            ret = ret+"q";
        }


        //TODO: change the ep field
        ret = ret+" "+parts[3]+" "+parts[4]+" "+parts[5];

        return ret;
    }

    public static String reverseCase(String text)
    {
        char[] chars = text.toCharArray();
        for (int i = 0; i < chars.length; i++)
        {
            char c = chars[i];
            if (Character.isUpperCase(c))
            {
                chars[i] = Character.toLowerCase(c);
            }
            else if (Character.isLowerCase(c))
            {
                chars[i] = Character.toUpperCase(c);
            }
        }
        return new String(chars);
    }


}
