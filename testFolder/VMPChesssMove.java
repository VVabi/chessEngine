package messages;
import VDT.*;
public class VMPChesssMove extends message{
	public static final int unique_id=MessageIDs.VMPChesssMove_unique_id;
	public int getUniqueId(){ return unique_id;}
	public VDTChessMove move;
	public byte[] deflate(){
		int total_size=4+move.getSize();
		byte[] mem = new byte[total_size];
		int offset_correction = 0;
		typeConv.int_to_byte(unique_id,mem,0);
		move.deflate(mem,4+offset_correction);
		offset_correction = offset_correction+move.getSize();
		return mem;
}
	public VMPChesssMove(byte[] mem){
		int offset_correction = 0;
		move= new VDTChessMove(mem,4+offset_correction);
		offset_correction = offset_correction+move.getSize();
	}
	public int getSize(){
		int total_size=4+move.getSize();
		return total_size;
	}
	public VMPChesssMove(VDTChessMove move_constr){
		move=move_constr;
	}
}
