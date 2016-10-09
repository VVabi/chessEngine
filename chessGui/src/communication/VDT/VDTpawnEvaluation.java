package communication.VDT;
import communication.messages.*;
public class VDTpawnEvaluation extends coreVDTType{
	public static final int unique_id=VdtIDs.VDTpawnEvaluation_unique_id;
	public int getUniqueId(){ return unique_id;}
	public int total;
	public int doubledPawns;
	public int passedPawn;
	public void deflate(byte[] mem, int offset_correction){
		typeConv.int_to_byte(total,mem,0+offset_correction);
		typeConv.int_to_byte(doubledPawns,mem,4+offset_correction);
		typeConv.int_to_byte(passedPawn,mem,8+offset_correction);
}
	public VDTpawnEvaluation(byte[] mem, int offset_correction){
		total=typeConv.byte_to_int(mem,0+offset_correction);
		doubledPawns=typeConv.byte_to_int(mem,4+offset_correction);
		passedPawn=typeConv.byte_to_int(mem,8+offset_correction);
	}
	public int getSize(){
		int total_size=0+4+4+4;
		return total_size;
	}
	public VDTpawnEvaluation(int total_constr,int doubledPawns_constr,int passedPawn_constr){
		total=total_constr;
		doubledPawns=doubledPawns_constr;
		passedPawn=passedPawn_constr;
	}
}
