/*
 * boardParts.hpp
 *
 *  Created on: Sep 5, 2016
 *      Author: vabi
 */

#ifndef LIB_DEFINES_BOARDPARTS_HPP_
#define LIB_DEFINES_BOARDPARTS_HPP_

#define FILEA ((1UL << 0) | (1UL << 8) | (1UL << 16) | (1UL << 24) | (1UL << 32) | (1UL << 40) | (1UL << 48) | (1UL << 56))
#define FILEB ((1UL << 1) | (1UL << 9) | (1UL << 17) | (1UL << 25) | (1UL << 33) | (1UL << 41) | (1UL << 49) | (1UL << 57))
#define FILEC ((1UL << 2) | (1UL << 10) | (1UL << 18) | (1UL << 26) | (1UL << 34) | (1UL << 42) | (1UL << 50) | (1UL << 58))
#define FILED ((1UL << 3) | (1UL << 11) | (1UL << 19) | (1UL << 27) | (1UL << 35) | (1UL << 43) | (1UL << 51) | (1UL << 59))
#define FILEE ((1UL << 4) | (1UL << 12) | (1UL << 20) | (1UL << 28) | (1UL << 36) | (1UL << 44) | (1UL << 52) | (1UL << 60))
#define FILEF ((1UL << 5) | (1UL << 13) | (1UL << 21) | (1UL << 29) | (1UL << 37) | (1UL << 45) | (1UL << 53) | (1UL << 61))
#define FILEG ((1UL << 6) | (1UL << 14) | (1UL << 22) | (1UL << 30) | (1UL << 38) | (1UL << 46) | (1UL << 54) | (1UL << 62))
#define FILEH ((1UL << 7) | (1UL << 15) | (1UL << 23) | (1UL << 31) | (1UL << 39) | (1UL << 47) | (1UL << 55) | (1UL << 63))

#define FIRSTROW ((1UL << 0) | (1UL << 1) |(1UL << 2) |(1UL << 3) |(1UL << 4) |(1UL << 5) |(1UL << 6) |(1UL << 7))
#define LASTROW ((1UL << 56) | (1UL << 57) |(1UL << 58) |(1UL << 59) |(1UL << 60) |(1UL << 61) |(1UL << 62) |(1UL << 63))
#define SECONDROW ((1UL << 8) | (1UL << 9) |(1UL << 10) |(1UL << 11) |(1UL << 12) |(1UL << 13) |(1UL << 14) |(1UL << 15))
#define SEVENTHROW ((1UL << 48) | (1UL << 49) |(1UL << 50) |(1UL << 51) |(1UL << 52) |(1UL << 53) |(1UL << 54) |(1UL << 55))

#define NOTFILEA (~( (1UL << 0) | (1UL << 8) | (1UL << 16) | (1UL << 24) | (1UL << 32) | (1UL << 40) | (1UL << 48) | (1UL << 56)))
#define NOTFILEH (~( (1UL << 7) | (1UL << 15) | (1UL << 23) | (1UL << 31) | (1UL << 39) | (1UL << 47) | (1UL << 55) | (1UL << 63)))


#define WHITEKINGSIDECASTLEBLOCKINGFIELDS  ((1UL << 5) | (1UL << 6))
#define WHITEQUEENSIDECASTLEBLOCKINGFIELDS ((1UL << 1) | (1UL << 2) | (1UL << 3))
#define BLACKKINGSIDECASTLEBLOCKINGFIELDS  ((1UL << 61) | (1UL << 62))
#define BLACKQUEENSIDECASTLEBLOCKINGFIELDS ((1UL << 57) | (1UL << 58) | (1UL << 59))

#define WHITEKINGSIDECASTLEMASK  ((1UL << 4) | (1UL << 7))
#define WHITEQUEENSIDECASTLEMASK ((1UL << 0) | (1UL << 4))
#define BLACKKINGSIDECASTLEMASK  ((1UL << 60) | (1UL << 63))
#define BLACKQUEENSIDECASTLEMASK ((1UL << 56) | (1UL << 60))

#define WHITEKINGSIDECASTLEOCCUPANCYCHANGE ((1UL << 4) | (1UL << 5) | (1UL << 6) | (1UL << 7))
#define WHITEKINGSIDECASTLEROOKMOVE         ((1UL << 5) | (1UL << 7))
#define WHITEKINGSIDECASTLEKINGMOVE         ((1UL << 4) | (1UL << 6))


#define BLACKKINGSIDECASTLEOCCUPANCYCHANGE ((1UL << 60) | (1UL << 61) | (1UL << 62) | (1UL << 63))
#define BLACKKINGSIDECASTLEROOKMOVE         ((1UL << 61) | (1UL << 63))
#define BLACKKINGSIDECASTLEKINGMOVE         ((1UL << 60) | (1UL << 62))

#define WHITEQUEENSIDECASTLEOCCUPANCYCHANGE ((1UL << 4) | (1UL << 3) | (1UL << 2) | (1UL << 0))
#define WHITEQUEENSIDECASTLEROOKMOVE         ((1UL << 0) | (1UL << 3))
#define WHITEQUEENSIDECASTLEQUEENMOVE         ((1UL << 4) | (1UL << 2))


#define BLACKQUEENSIDECASTLEOCCUPANCYCHANGE ((1UL << 60) | (1UL << 59) | (1UL << 58) | (1UL << 56))
#define BLACKQUEENSIDECASTLEROOKMOVE         ((1UL << 56) | (1UL << 59))
#define BLACKQUEENSIDECASTLEQUEENMOVE         ((1UL << 60) | (1UL << 58))

#define LOWERPART UINT32_MAX
#define HIGHERPART ~LOWERPART


#endif /* LIB_DEFINES_BOARDPARTS_HPP_ */
