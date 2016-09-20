/*
 * enPassantTables.hpp
 *
 *  Created on: Sep 20, 2016
 *      Author: vabi
 */

#ifndef LIB_DEFINES_ENPASSANTTABLES_HPP_
#define LIB_DEFINES_ENPASSANTTABLES_HPP_

#define WHITEFILEA (1UL << 33)
#define WHITEFILEB ((1UL << 32) | (1UL << 34))
#define WHITEFILEC ((1UL << 33) | (1UL << 35))
#define WHITEFILED ((1UL << 34) | (1UL << 36))
#define WHITEFILEE ((1UL << 35) | (1UL << 37))
#define WHITEFILEF ((1UL << 36) | (1UL << 38))
#define WHITEFILEG ((1UL << 37) | (1UL << 39))
#define WHITEFILEH  (1UL << 38)

#define BLACKFILEA (1UL << 25)
#define BLACKFILEB ((1UL << 24) | (1UL << 26))
#define BLACKFILEC ((1UL << 25) | (1UL << 27))
#define BLACKFILED ((1UL << 26) | (1UL << 28))
#define BLACKFILEE ((1UL << 27) | (1UL << 29))
#define BLACKFILEF ((1UL << 28) | (1UL << 30))
#define BLACKFILEG ((1UL << 29) | (1UL << 31))
#define BLACKFILEH  (1UL << 30)

#endif /* LIB_DEFINES_ENPASSANTTABLES_HPP_ */
