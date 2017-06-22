import java.security.SecureRandom

/**
 * Created by vabi on 18.06.17.
 */



data class player(var id: Int, var taggerId: Int)


fun simulateGame(numPlayers: Int): Int{

    var untaggedList = mutableListOf<player>()
    var taggedList   = mutableListOf<player>()
    for(i in 0..numPlayers-1) {
        var p = player(i, -1)
        untaggedList.add(p)
    }

    val random = SecureRandom()
    var numSteps = 0
    while(true) {
        numSteps++
        var catcher = random.nextInt(untaggedList.size)
        var victim  = random.nextInt(untaggedList.size-1)

        if(victim >= catcher) {
            victim += 1
        }

        var tagged = untaggedList[victim]
        var tagger = untaggedList[catcher]
        untaggedList.remove(tagged)
        tagged.taggerId = tagger.id
        var victimId = tagged.id

        val toRemove = taggedList.filter { it.taggerId == victimId }

        taggedList.removeAll(toRemove)
        taggedList.add(tagged)
        untaggedList.addAll(toRemove)


        if(untaggedList.size == 1) {
            break
        }

    }
    //println(numSteps)

    return numSteps


}








fun main(args: Array<String>) {

    var totalSteps = 0
    var numGames = 1000000
    for(i in 1..numGames) {
        totalSteps += simulateGame(7)
    }

    println(totalSteps/numGames.toDouble())
}