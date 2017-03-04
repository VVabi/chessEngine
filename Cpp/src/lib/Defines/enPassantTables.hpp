/*
 * enPassantTables.hpp
 *
 *  Created on: Sep 20, 2016
 *      Author: vabi
 */

#ifndef LIB_DEFINES_ENPASSANTTABLES_HPP_
#define LIB_DEFINES_ENPASSANTTABLES_HPP_

#define WHITEFILEA (1ULL << 33)
#define WHITEFILEB ((1ULL << 32) | (1ULL << 34))
#define WHITEFILEC ((1ULL << 33) | (1ULL << 35))
#define WHITEFILED ((1ULL << 34) | (1ULL << 36))
#define WHITEFILEE ((1ULL << 35) | (1ULL << 37))
#define WHITEFILEF ((1ULL << 36) | (1ULL << 38))
#define WHITEFILEG ((1ULL << 37) | (1ULL << 39))
#define WHITEFILEH  (1ULL << 38)

#define BLACKFILEA (1ULL << 25)
#define BLACKFILEB ((1ULL << 24) | (1ULL << 26))
#define BLACKFILEC ((1ULL << 25) | (1ULL << 27))
#define BLACKFILED ((1ULL << 26) | (1ULL << 28))
#define BLACKFILEE ((1ULL << 27) | (1ULL << 29))
#define BLACKFILEF ((1ULL << 28) | (1ULL << 30))
#define BLACKFILEG ((1ULL << 29) | (1ULL << 31))
#define BLACKFILEH  (1ULL << 30)

#endif /* LIB_DEFINES_ENPASSANTTABLES_HPP_ */
