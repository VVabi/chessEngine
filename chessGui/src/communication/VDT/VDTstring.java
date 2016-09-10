package communication.VDT;
import communication.messages.*;
public class VDTstring extends coreVDTType{
	public static final int unique_id=VdtIDs.VDTstring_unique_id;
	public int getUniqueId(){ return unique_id;}
	public byte[] chars;
	public void deflate(byte[] mem, int offset_correction){
		typeConv.int_to_byte(chars.length,mem,0+offset_correction);
		for(int i = 0; i < chars.length; i++){
			mem[4+offset_correction]=chars[i];
			offset_correction = offset_correction+1;
		}
}
	public VDTstring(byte[] mem, int offset_correction){
		int array_size = 0;
		array_size=typeConv.byte_to_int(mem,0+offset_correction);
			chars= new byte[array_size];
		for(int i=0;i<array_size;i++){
			chars[i] = mem[4+offset_correction];
			offset_correction = offset_correction+1;
		}
	}
	public int getSize(){
		int total_size=0+1*chars.length+4;
		return total_size;
	}
	public VDTstring(byte[] chars_constr){
		chars=chars_constr;
	}
}
