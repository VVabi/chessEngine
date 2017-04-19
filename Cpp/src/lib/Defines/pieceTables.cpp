/*
 * pieceTables.cpp
 *
 *  Created on: Sep 19, 2016
 *      Author: vabi
 */

#include <stdint.h>


int16_t rawPieceTables[6][32] = {

				//pawns
				{
//#ifdef EXPERIMENTAL
						0,0,0,0, //a1-d1
						0,0,-2,-7, //a2-d2
						0,0,5,5, //a3-d3  etc.
						0,0,10,15,
						0,0,5,10,
						0,0,5,10,
						25,25,25,25,
						0,0,0,0
/*#else
						0,0,0,0, //a1-d1
						0,0,-2,-7, //a2-d2
						0,0,5,5, //a3-d3  etc.
						0,0,10,15,
						0,0,5,10,
						0,0,5,10,
						25,25,25,25,
						0,0,0,0
#endif*/
				},

				//knights
				{

						-30,-20,-15,-10,
						-20,-15,-10,-5,
						-15,-10, 5, 0,
						-10,-2, 7, 15,
						-10,-2, 7, 15,
						-15,-10, 5, 10,
						-20,-15,-10,-5,
						-30,-20,-15,-10
				} ,

				{//bishops
						-20,-17,-17,-12,
						-5,7,2,7,
						-2,2,5,2,
						0,7,5,7,
						0,10,2,5,
						-2,2,5,5,
						-7,-5,-5,-5,
						-20,-12,-7,-7
				},

				{ // rooks
						0,5,10,15,
						-10,-5,0,0,
						-7,-2,0,5,
						-7,-2,0,0,
						-7,-2,0,0,
						-7,-2,0,0,
						15,15,20,20,
						5,10,10,10
				},

				{ //queens
						-5,0,5,5,
						-10,-5,5,7,
						-7,-2,0,5,
						-7,-2,5,15,
						-5,-2,2,10,
						-7,-2,5,7,
						15,15,20,20,
						0,5,10,10
				},

				{ // kings
						10,15,0,-7,
						0,5,-7,-15,
						-12,-12,-15,-22,
						-20,-25,-30,-35,
						-20,-25,-30,-35,
						-25,-35,-35,-35,
						-25,-35,-35,-35,
						-25,-35,-35,-35,
				}

};

int16_t endgameRawPieceTables[6][32] = {
	//pawns
	{

//#ifdef EXPERIMENTAL
			0,0,0,0,
			0,0,-2,-7,
			5,5,5,5,
			10,5,5,5,
			15,10,10,10,
			20,15,15,15,
			25,25,25,25,
			0,0,0,0

/*#else
			0,0,0,0,
			0,0,-2,-7,
			5,5,5,5,
			10,5,5,5,
			15,10,10,10,
			20,15,15,15,
			25,25,25,25,
			0,0,0,0
#endif*/
	},

	//knights
	{
/*#ifdef EXPERIMENTAL
			-30,-20,-15,-10,
			-20,-15,-10,-5,
			-15,-10, 5, 0,
			-10,-2, 4,10,
			-10,-2, 4, 10,
			-15,-10, 5, 10,
			-20,-15,-10,-5,
			-30,-20,-15,-10
#else*/
			-30,-20,-15,-10,
			-20,-15,-10,-5,
			-15,-10, 5, 0,
			-10,-2, 7, 15,
			-10,-2, 7, 15,
			-15,-10, 5, 10,
			-20,-15,-10,-5,
			-30,-20,-15,-10
//#endif

	} ,

	{//bishops
			-15,-7,-7,-5,
			-5,7,2,7,
			-2,2,5,2,
			0,7,5,7,
			0,10,2,5,
			-2,2,5,5,
			-7,-5,-5,-5,
			-20,-12,-7,-7
	},

	{ // rooks
			0,5,10,15,
			-10,-5,0,0,
			-7,-2,0,5,
			-7,-2,0,0,
			-7,-2,0,0,
			-7,-2,0,0,
			5,10,10,10,
			5,10,10,10
	},

	{ //queens
			-5,0,5,5,
			-10,-5,5,7,
			-7,-2,0,5,
			-7,-2,5,15,
			-5,-2,2,10,
			-7,-2,5,7,
			-10,-5,5,7,
			 0,5,10,10
	},

	{ // kings
			-100,-70,-40,-25,
			-70,-50,-30,-10,
			-40,-30,-10, 10,
			-25,-10, 10, 30,
			-25,-10, 10, 30,
			-40,-30,-10, 10,
			-70,-50,-30,-10,
			-100,-70,-40,-25
	}


};


int16_t pieceTables[7][2][64];


int16_t endGamepieceTables[7][2][64];

int32_t completePieceTables[7][2][64];
