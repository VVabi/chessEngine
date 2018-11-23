package Tools

import com.google.gson.Gson
import com.google.gson.JsonParser
import com.google.gson.internal.LinkedTreeMap
import java.util.*

/**
 * Created by vabi on 26.08.18.
 */


data class EvalComponentResult(var common: Int, var earlygame: Int, var endgame: Int)

data class DetailedEvaluationResultComponent(var taperingValue: Int, var eval: Int, var components: EvalComponentResult)

data class csvData(var header: String, var line: String)

fun toCsvLine(json: String): csvData {
    var x = HashMap<String, DetailedEvaluationResultComponent>()
    var jsonParser = JsonParser()
    var jsonTree   = jsonParser.parse(json).asJsonObject

    for (t in jsonTree.entrySet()) {
        if (t.key.compareTo("total") != 0) {
            x.put(t.key, Gson().fromJson(t.value, DetailedEvaluationResultComponent::class.java))
        }
    }

    var keys = LinkedList<String>(x.keys)
    keys.sort()

    var ret = ""
    var header = ""
    for (key in keys) {
        var componentResult: DetailedEvaluationResultComponent = x[key]!!

        var tapering = componentResult.taperingValue/256.0
        if(tapering > 1.0 || tapering < 0.0) {
            println("WTF")
        }

        var common = componentResult.components.common/100.0
        var earlygame = componentResult.components.earlygame/100.0*tapering
        var endgame = componentResult.components.endgame/100.0*(1.0-tapering)

        ret  =ret+common.toString()+","+earlygame.toString()+","+endgame.toString()+","
        header = header+key+"_common,"+key+"_earlygame,"+key+"_endgame,"
    }
    return csvData(header, ret)
}