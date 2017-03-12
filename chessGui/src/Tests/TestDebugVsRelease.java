package Tests;

import uciClient.uciEngineHandler;

import java.io.IOException;
import java.util.List;

/**
 * Created by vabi on 11.03.17.
 */
public class TestDebugVsRelease {


    public static boolean testDebugvsRelease(String debugpath, String releasepath) throws IOException {

        uciEngineHandler releaseHandler = new uciEngineHandler(releasepath,  "");
        List<String> releaseSearchOutput = releaseHandler.debugSearchOutput(12);
        uciEngineHandler debugHandler = new uciEngineHandler(debugpath,  "");
        List<String> debugSearchOutput = debugHandler.debugSearchOutput(12);

        if(releaseSearchOutput.size() != debugSearchOutput.size()) {
            System.out.println("Debug and release builds: different output sizes");
            return false;
        }

        for(int ind = 0; ind < releaseSearchOutput.size(); ind++) {
            String r = releaseSearchOutput.get(ind);
            String d = debugSearchOutput.get(ind);

            if(!r.equals(d)) {
                System.out.println("Debug and release builds: different output at line "+ind);
                System.out.println(r);
                System.out.println(d);
                return false;
            }
        }

        return true;
    }




}
