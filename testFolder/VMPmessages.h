#ifndef VMP
#define VMP
#include <src/DataTypes/fsarray.h>
#include <string.h>
#include "VDT.h"
#include "../Network/message.h"
#define VMPChesssMove_UNIQUE_ID 0

class VMPChesssMove:public message{
	public:
	static const uint32_t unique_id = VMPChesssMove_UNIQUE_ID;
	uint32_t getUniqueId(){ return unique_id;}
	VDTChessMove move;
	void deflate(uint8_t* mem, int offset_correction){
		uint32_t x_unique_id = unique_id;
		memcpy(mem+offset_correction, &x_unique_id, 4);
		offset_correction = offset_correction+4;
		move.deflate(mem,offset_correction);
		offset_correction = offset_correction+move.getSize();

	}
	VMPChesssMove(uint8_t* mem, int offset_correction){
		move= VDTChessMove(mem, offset_correction);
		offset_correction = offset_correction+move.getSize();
	}
	uint32_t getSize(){
		uint32_t total_size=0+move.getSize();
		return total_size;
	}
	void dispose(){
		move.dispose();
	}
	VMPChesssMove(
		VDTChessMove move_c
	){
		move = move_c;
	}
	VMPChesssMove() {
	}
	VMPChesssMove copy(){
		 return VMPChesssMove(
			move.copy()
);
	}
	~VMPChesssMove(){ dispose();}
};
#endif
