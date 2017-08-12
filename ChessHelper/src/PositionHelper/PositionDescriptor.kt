package PositionHelper
//rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1
class PositionDescriptor(fen: String) {

    var position: MutableList<Char> = mutableListOf<Char>()

    init {
        var parts = fen.split(" ")
        var fenPosition = parts[0]
        fenPosition = fenPosition.replace("/", "")
        for(c in fenPosition) {
            if (Character.isDigit(c)) {
                var value:Int = c-'0'
                while(value > 0) {
                    position.add('0')
                    value--
                }
            } else {
                position.add(c)
            }
        }
        if (position.size != 64) {
            println("Invalid fen")
            throw Exception("Invalid fen")
        }

        var toMove = parts[1]
        if (toMove.length != 1) {
            println("Invalid fen")
            throw Exception("Invalid fen")
        }

        position.add(toMove[0])

        var castling = parts[2]

        if (castling.contains('K')) {
            position.add('K')
        } else {
            position.add('0')
        }

        if (castling.contains('Q')) {
            position.add('Q')
        } else {
            position.add('0')
        }

        if (castling.contains('k')) {
            position.add('k')
        } else {
            position.add('0')
        }

        if (castling.contains('q')) {
            position.add('q')
        } else {
            position.add('0')
        }

        /*for(c in position) {
            print(c)
        }*/
   }

   fun addPiece(piece:Char, field:Int) {
       position[field] = piece
   }

    fun pieceAt(field: Int): Char {
        return position[field];
    }

    fun toFen(): String {
        var ret = ""
        var index = 0
        var counter = 0
        while(index < 64) {
            if(position[index] == '0') {
                counter++
            } else {
                if (counter > 0) {
                    ret += counter
                }
                ret += position[index]
                counter = 0
            }

            if((index > 0) && (((index+1) % 8) == 0)) {
                if (counter > 0) {
                    ret += counter
                }
                if(index < 63) {
                    ret += "/"
                }
                counter = 0
            }
            index++
        }

        ret += " "+position[64]+" "

        var castling = "-"

        var pre = ""
        if (position[65] == 'K') {
            pre += 'K'
        }
        if (position[66] == 'Q') {
            pre += 'Q'
        }
        if (position[67] == 'k') {
            pre += 'k'
        }
        if (position[68] == 'q') {
            pre += 'q'
        }

        if(pre.length > 0) {
            castling = pre
        }
        ret += castling+" - 0 0"
        return ret
    }

    fun setCastling(castling: String) {
        position[65] = castling[0]
        position[66] = castling[1]
        position[67] = castling[2]
        position[68] = castling[3]
    }

    fun updateMovingSide(c: Char) {
        if (c == 'w') {
            position[64] = 'w'
        } else {
            position[64] = 'b'
        }
    }
}