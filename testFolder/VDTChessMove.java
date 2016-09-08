package VDT;
import messages.*;
public class VDTChessMove extends coreVDTType{
	public static final int unique_id=VdtIDs.VDTChessMove_unique_id;
	public int getUniqueId(){ return unique_id;}
	public long move;
	public short figureType;
	public short captureType;
	public void deflate(byte[] mem, int offset_correction){
		typeConv.short_to_byte(figureType,mem,8+offset_correction);
		typeConv.short_to_byte(captureType,mem,10+offset_correction);
}
	public VDTChessMove(byte[] mem, int offset_correction){
		figureType=typeConv.byte_to_short(mem,8+offset_correction);
		captureType=typeConv.byte_to_short(mem,10+offset_correction);
	}
	public int getSize(){
		int total_size=0+8+2+2;
		return total_size;
	}
	public VDTChessMove(long move_constr,short figureType_constr,short captureType_constr){
		move=move_constr;
		figureType=figureType_constr;
		captureType=captureType_constr;
	}
}
