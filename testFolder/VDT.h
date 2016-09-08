#ifndef VDT
#define VDT
#include <src/DataTypes/fsarray.h>
#include <string.h>
#include "VDTCore.hpp"
#define VDTChessMove_UNIQUE_ID 0

class VDTChessMove:public vdtCoreType{
	public:
	static const uint32_t unique_id = VDTChessMove_UNIQUE_ID;
	uint32_t getUniqueId(){ return unique_id;}
	uint64_t move;
	uint16_t figureType;
	uint16_t captureType;
	void deflate(uint8_t* mem, int offset_correction){
		memcpy(mem+offset_correction, &move, sizeof(uint64_t));
		offset_correction = offset_correction+sizeof(uint64_t);
		memcpy(mem+offset_correction, &figureType, sizeof(uint16_t));
		offset_correction = offset_correction+sizeof(uint16_t);
		memcpy(mem+offset_correction, &captureType, sizeof(uint16_t));
		offset_correction = offset_correction+sizeof(uint16_t);

	}
	VDTChessMove(uint8_t* mem, int offset_correction){
		memcpy(&move,mem+offset_correction,  sizeof(uint64_t));
		offset_correction = offset_correction+sizeof(uint64_t);
		memcpy(&figureType,mem+offset_correction,  sizeof(uint16_t));
		offset_correction = offset_correction+sizeof(uint16_t);
		memcpy(&captureType,mem+offset_correction,  sizeof(uint16_t));
		offset_correction = offset_correction+sizeof(uint16_t);
	}
	uint32_t getSize(){
		uint32_t total_size=0+sizeof(uint64_t)+sizeof(uint16_t)+sizeof(uint16_t);
		return total_size;
	}
	void dispose(){
	}
	VDTChessMove(
		uint64_t move_c,
		uint16_t figureType_c,
		uint16_t captureType_c
	){
		move = move_c;
		figureType = figureType_c;
		captureType = captureType_c;
	}
	VDTChessMove() {
		move=0;
		figureType=0;
		captureType=0;
	}
	VDTChessMove copy(){
		 return VDTChessMove(
			move,
			figureType,
			captureType
);
	}
};
#endif
