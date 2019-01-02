/*
 * DoardParts.hpp
 *
 *  Created on: Sep 5, 2016
 *      Author: vaDi
 */

#ifndef LIB_DEFINES_DOARDPARTS_HPP_
#define LIB_DEFINES_DOARDPARTS_HPP_
#include "chessFields.hpp"

#define FILEA ((1ULL << 0) | (1ULL << 8) | (1ULL << 16) | (1ULL << 24) | (1ULL << 32) | (1ULL << 40) | (1ULL << 48) | (1ULL << 56))
#define FILEB ((1ULL << 1) | (1ULL << 9) | (1ULL << 17) | (1ULL << 25) | (1ULL << 33) | (1ULL << 41) | (1ULL << 49) | (1ULL << 57))
#define FILEC ((1ULL << 2) | (1ULL << 10) | (1ULL << 18) | (1ULL << 26) | (1ULL << 34) | (1ULL << 42) | (1ULL << 50) | (1ULL << 58))
#define FILED ((1ULL << 3) | (1ULL << 11) | (1ULL << 19) | (1ULL << 27) | (1ULL << 35) | (1ULL << 43) | (1ULL << 51) | (1ULL << 59))
#define FILEE ((1ULL << 4) | (1ULL << 12) | (1ULL << 20) | (1ULL << 28) | (1ULL << 36) | (1ULL << 44) | (1ULL << 52) | (1ULL << 60))
#define FILEF ((1ULL << 5) | (1ULL << 13) | (1ULL << 21) | (1ULL << 29) | (1ULL << 37) | (1ULL << 45) | (1ULL << 53) | (1ULL << 61))
#define FILEG ((1ULL << 6) | (1ULL << 14) | (1ULL << 22) | (1ULL << 30) | (1ULL << 38) | (1ULL << 46) | (1ULL << 54) | (1ULL << 62))
#define FILEH ((1ULL << 7) | (1ULL << 15) | (1ULL << 23) | (1ULL << 31) | (1ULL << 39) | (1ULL << 47) | (1ULL << 55) | (1ULL << 63))

#define FIRSTROW ((1ULL << 0) | (1ULL << 1) |(1ULL << 2) |(1ULL << 3) |(1ULL << 4) |(1ULL << 5) |(1ULL << 6) |(1ULL << 7))
#define LASTROW ((1ULL << 56) | (1ULL << 57) |(1ULL << 58) |(1ULL << 59) |(1ULL << 60) |(1ULL << 61) |(1ULL << 62) |(1ULL << 63))
#define SECONDROW ((1ULL << 8) | (1ULL << 9) |(1ULL << 10) |(1ULL << 11) |(1ULL << 12) |(1ULL << 13) |(1ULL << 14) |(1ULL << 15))
#define SEVENTHROW ((1ULL << 48) | (1ULL << 49) |(1ULL << 50) |(1ULL << 51) |(1ULL << 52) |(1ULL << 53) |(1ULL << 54) |(1ULL << 55))

#define NOTFILEA (~((1ULL << 0) | (1ULL << 8) | (1ULL << 16) | (1ULL << 24) | (1ULL << 32) | (1ULL << 40) | (1ULL << 48) | (1ULL << 56)))
#define NOTFILEH (~((1ULL << 7) | (1ULL << 15) | (1ULL << 23) | (1ULL << 31) | (1ULL << 39) | (1ULL << 47) | (1ULL << 55) | (1ULL << 63)))


#define WHITEKINGSIDECASTLEBLOCKINGFIELDS  ((1ULL << 5) | (1ULL << 6))
#define WHITEQUEENSIDECASTLEBLOCKINGFIELDS ((1ULL << 1) | (1ULL << 2) | (1ULL << 3))
#define BLACKKINGSIDECASTLEBLOCKINGFIELDS  ((1ULL << 61) | (1ULL << 62))
#define BLACKQUEENSIDECASTLEBLOCKINGFIELDS ((1ULL << 57) | (1ULL << 58) | (1ULL << 59))

#define WHITEKINGSIDECASTLEMASK  ((1ULL << 4) | (1ULL << 7))
#define WHITEQUEENSIDECASTLEMASK ((1ULL << 0) | (1ULL << 4))
#define BLACKKINGSIDECASTLEMASK  ((1ULL << 60) | (1ULL << 63))
#define BLACKQUEENSIDECASTLEMASK ((1ULL << 56) | (1ULL << 60))

#define WHITEKINGSIDECASTLEOCCUPANCYCHANGE ((1ULL << 4) | (1ULL << 5) | (1ULL << 6) | (1ULL << 7))
#define WHITEKINGSIDECASTLEROOKMOVE         ((1ULL << 5) | (1ULL << 7))
#define WHITEKINGSIDECASTLEKINGMOVE         ((1ULL << 4) | (1ULL << 6))


#define BLACKKINGSIDECASTLEOCCUPANCYCHANGE ((1ULL << 60) | (1ULL << 61) | (1ULL << 62) | (1ULL << 63))
#define BLACKKINGSIDECASTLEROOKMOVE         ((1ULL << 61) | (1ULL << 63))
#define BLACKKINGSIDECASTLEKINGMOVE         ((1ULL << 60) | (1ULL << 62))

#define WHITEQUEENSIDECASTLEOCCUPANCYCHANGE ((1ULL << 4) | (1ULL << 3) | (1ULL << 2) | (1ULL << 0))
#define WHITEQUEENSIDECASTLEROOKMOVE         ((1ULL << 0) | (1ULL << 3))
#define WHITEQUEENSIDECASTLEQUEENMOVE         ((1ULL << 4) | (1ULL << 2))


#define BLACKQUEENSIDECASTLEOCCUPANCYCHANGE ((1ULL << 60) | (1ULL << 59) | (1ULL << 58) | (1ULL << 56))
#define BLACKQUEENSIDECASTLEROOKMOVE         ((1ULL << 56) | (1ULL << 59))
#define BLACKQUEENSIDECASTLEQUEENMOVE         ((1ULL << 60) | (1ULL << 58))

#define LOWERPART UINT32_MAX
#define HIGHERPART ~LOWERPART

#define CENTER (E4 | D4 | E5 | D5)

#define WIDECENTER (C3 | D3 | E3 | C4 | D4 | E4 | C5 | D5 | E5)

#define BOUNDARY (FILEA | FILEH | FIRSTROW | LASTROW)

#define BLACKFIELDS (A1 | A3 | A5 | A7 | B2 | B4 | B6 | B8 | C1 | C3 | C5 | C7 | D2 | D4 | D6 | D8 | E1 | E3 | E5 | E7 | F2 | F4 | F6 | F8 | G1 | G3 | G5 | G7 | H2 | H4 |  H6 | H8)
#define WHITEFIELDS (UINT64_MAX & (~BLACKFIELDS))

#define SOUTHHALF (0xFFFFFFFFULL)
#define NORTHHALF ((0xFFFFFFFFULL) << 32)

#endif /* LIB_DEFINES_DOARDPARTS_HPP_ */
