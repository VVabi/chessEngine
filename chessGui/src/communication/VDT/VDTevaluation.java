package communication.VDT;
import communication.messages.*;
public class VDTevaluation extends coreVDTType{
	public static final int unique_id=VdtIDs.VDTevaluation_unique_id;
	public int getUniqueId(){ return unique_id;}
	public int total;
	public VDTpawnEvaluation pawnEval;
	public int figureEval;
	public int PSQ;
	public int rookOpenFiles;
	public int bishopPair;
	public int mobility;
	public void deflate(byte[] mem, int offset_correction){
		typeConv.int_to_byte(total,mem,0+offset_correction);
		pawnEval.deflate(mem,4+offset_correction);
		offset_correction = offset_correction+pawnEval.getSize();
		typeConv.int_to_byte(figureEval,mem,4+offset_correction);
		typeConv.int_to_byte(PSQ,mem,8+offset_correction);
		typeConv.int_to_byte(rookOpenFiles,mem,12+offset_correction);
		typeConv.int_to_byte(bishopPair,mem,16+offset_correction);
		typeConv.int_to_byte(mobility,mem,20+offset_correction);
}
	public VDTevaluation(byte[] mem, int offset_correction){
		total=typeConv.byte_to_int(mem,0+offset_correction);
		pawnEval= new VDTpawnEvaluation(mem,4+offset_correction);
		offset_correction = offset_correction+pawnEval.getSize();
		figureEval=typeConv.byte_to_int(mem,4+offset_correction);
		PSQ=typeConv.byte_to_int(mem,8+offset_correction);
		rookOpenFiles=typeConv.byte_to_int(mem,12+offset_correction);
		bishopPair=typeConv.byte_to_int(mem,16+offset_correction);
		mobility=typeConv.byte_to_int(mem,20+offset_correction);
	}
	public int getSize(){
		int total_size=0+4+pawnEval.getSize()+4+4+4+4+4;
		return total_size;
	}
	public VDTevaluation(int total_constr,VDTpawnEvaluation pawnEval_constr,int figureEval_constr,int PSQ_constr,int rookOpenFiles_constr,int bishopPair_constr,int mobility_constr){
		total=total_constr;
		pawnEval=pawnEval_constr;
		figureEval=figureEval_constr;
		PSQ=PSQ_constr;
		rookOpenFiles=rookOpenFiles_constr;
		bishopPair=bishopPair_constr;
		mobility=mobility_constr;
	}
}
