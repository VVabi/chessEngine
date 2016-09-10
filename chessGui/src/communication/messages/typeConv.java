/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package communication.messages;

/**
 *
 * @author fabian
 */
public class typeConv {
    
        public static byte[] int_to_byte(int to_convert){
         byte[] result = new byte[4];
         result[0] = (byte) (to_convert & 0xFF);
         result[1] = (byte) ((to_convert >> 8) & 0xFF);
         result[2] = (byte) ((to_convert >> 16) & 0xFF);
         result[3] = (byte) ((to_convert >> 24) & 0xFF);
         return result;
    }
        
    public static int byte_to_int(byte[] memory, int offset){
        return ((memory[offset] & 0xFF) | ( (memory[offset+1] & 0xFF) << 8) | ( (memory[offset+2] & 0xFF) << 16) | ( (memory[offset+3] & 0xFF) << 24));
    }
    
    public static short byte_to_short(byte[] memory, int offset){
        return (short) ((memory[offset] & 0xFF) | ( (memory[offset+1] & 0xFF) << 8));
    }
    public static void int_to_byte(int to_convert, byte[] memory, int offset){
         
         memory[offset]     = (byte) (to_convert & 0xFF);
         memory[offset+1]   = (byte) ((to_convert >> 8) & 0xFF);
         memory[offset+2]   = (byte) ((to_convert >> 16) & 0xFF);
         memory[offset+3]   = (byte) ((to_convert >> 24) & 0xFF);
        
    }
    
        public static void short_to_byte(short to_convert, byte[] memory, int offset){
         
         memory[offset]     = (byte) (to_convert & 0xFF);
         memory[offset+1]   = (byte) ((to_convert >> 8) & 0xFF);

        
    }

    public static void double_to_byte(double to_convert, byte[] memory, int offset){
            long binDouble = Double.doubleToRawLongBits(to_convert);
            long_to_byte(binDouble, memory, offset);


    }

    public static double byte_to_double(byte[] memory, int offset){
        long binDouble = byte_to_long(memory, offset);
        return Double.longBitsToDouble(binDouble);
    }

    public static void long_to_byte(long to_convert, byte[] memory, int offset){

        memory[offset]     = (byte) (to_convert & 0xFF);
        memory[offset+1]   = (byte) ((to_convert >> 8) & 0xFF);
        memory[offset+2]   = (byte) ((to_convert >> 16) & 0xFF);
        memory[offset+3]   = (byte) ((to_convert >> 24) & 0xFF);
        memory[offset+4]   = (byte) ((to_convert >> 32) & 0xFF);
        memory[offset+5]   = (byte) ((to_convert >> 40) & 0xFF);
        memory[offset+6]   = (byte) ((to_convert >> 48) & 0xFF);
        memory[offset+7]   = (byte) ((to_convert >> 56) & 0xFF);
    }

    public static long byte_to_long(byte[] memory, int offset){

        long value = 0;
        for (int i = 0; i < 8; i++)
        {
            value += ((long) memory[i+offset] & 0xffL) << (8 * i);
        }
        return value;
    }


}
