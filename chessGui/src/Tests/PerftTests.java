package Tests;

import uciClient.uciEngineHandler;

import java.io.BufferedReader;
import java.io.FileReader;
import java.io.IOException;
import java.util.ArrayList;

/**
 * Created by vabi on 29.03.17.
 */
public class PerftTests {


    private static class perftTestData{
        String fen;
        int depth;
        long numNodes;

        public perftTestData(String fen, int depth, long numNodes) {
            this.fen = fen;
            this.depth = depth;
            this.numNodes = numNodes;
        }
    }


    public static TestSuite.TestResult testPerft(uciEngineHandler engine) throws IOException {

        BufferedReader reader = new BufferedReader(new FileReader("src/Tests/perftTests.txt"));

        ArrayList<perftTestData> data = new ArrayList<>();

        String line;
        ArrayList<String> localData = new ArrayList<>();

        while((line = reader.readLine()) != null) {
            localData.add(line);
            if(localData.size() == 3) {
                data.add(new perftTestData(localData.get(0), Integer.parseInt(localData.get(1)), Long.parseLong(localData.get(2))));
                localData.clear();
            }
        }


        TestSuite.TestResult result = new TestSuite.TestResult(true);

        if(data.size() == 0) {
            System.out.println("No perft test data found");
            result.result = false;
        }


        for(perftTestData perftTest: data) {
            System.out.println("Testing "+perftTest.fen+" depth "+perftTest.depth);
            engine.setPosition(perftTest.fen, new ArrayList<>());
            long nodes = engine.perft(perftTest.depth);

            if(nodes != perftTest.numNodes) {
                System.out.println("Wrong perft");
                result.result = false;
                break;
            }
        }



        return result;
    }

}
