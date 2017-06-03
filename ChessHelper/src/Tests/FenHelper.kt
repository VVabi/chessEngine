package Tests

/**
 * Created by vabi on 01.06.17.
 */
fun mirror(FEN: String): String {

    val parts = FEN.split(" ".toRegex()).dropLastWhile { it.isEmpty() }.toTypedArray()

    val rows = parts[0].split("/".toRegex()).dropLastWhile { it.isEmpty() }.toTypedArray()


    var ret = ""

    for (ind in 0..7) {
        ret += StringBuilder(rows[ind]).reverse().toString()

        if (ind < 7) {
            ret += "/"
        }
    }


    ret = ret + " " + parts[1] + " - " + " - " + parts[4] + " " + parts[5]

    return ret
}

fun changeSide(FEN: String): String {

    val parts = FEN.split(" ".toRegex()).dropLastWhile { it.isEmpty() }.toTypedArray()

    val rows = parts[0].split("/".toRegex()).dropLastWhile { it.isEmpty() }.toTypedArray()


    var ret = ""

    for (ind in 7 downTo -1 + 1) {
        ret += reverseCase(rows[ind])

        if (ind > 0) {
            ret += "/"
        }
    }


    ret += " "
    if ("w" == parts[1]) {
        ret = ret + "b" + " "
    } else {
        ret = ret + "w" + " "
    }


    if (parts[2].contains("k")) {
        ret += "K"
    }
    if (parts[2].contains("q")) {
        ret += "Q"
    }
    if (parts[2].contains("K")) {
        ret += "k"
    }
    if (parts[2].contains("Q")) {
        ret += "q"
    }


    //TODO: change the ep field
    ret = ret + " " + parts[3] + " " + parts[4] + " " + parts[5]

    return ret
}

fun reverseCase(text: String): String {
    val chars = text.toCharArray()
    for (i in chars.indices) {
        val c = chars[i]
        if (Character.isUpperCase(c)) {
            chars[i] = Character.toLowerCase(c)
        } else if (Character.isLowerCase(c)) {
            chars[i] = Character.toUpperCase(c)
        }
    }
    return String(chars)
}