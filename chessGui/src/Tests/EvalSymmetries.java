package Tests;

import Library.FenHelper;
import uciClient.uciEngineHandler;

import java.io.BufferedReader;
import java.io.FileReader;
import java.io.IOException;
import java.util.ArrayList;
import java.util.LinkedList;

/**
 * Created by vabi on 29.03.17.
 */
public class EvalSymmetries {


    public static TestSuite.TestResult testEvalSymmetry(uciEngineHandler handler) throws IOException {

        TestSuite.TestResult result = new TestSuite.TestResult(true);

        ArrayList<String> fens = new ArrayList<>();
        try (BufferedReader br = new BufferedReader(new FileReader("fen.txt")))

        {
            String line;
            while ((line = br.readLine()) != null) {
                fens.add(line);
            }
        }


        for (String fen : fens) {
                    /*System.out.println(fen);
                    System.out.println(FenHelper.mirror(fen));*/
            handler.setPosition(fen, new LinkedList<>());
            int eval1 = handler.getEval();
            handler.setPosition(FenHelper.changeSide(fen), new LinkedList<>());
            int eval2 = handler.getEval();
            handler.setPosition(FenHelper.mirror(fen), new LinkedList<>());
            int eval3 = handler.getEval();

            if (eval1 != eval2) {
                System.out.println("Unsymmetric black/white eval detected");
                System.out.println(fen);
                System.out.println(FenHelper.changeSide(fen));
                result.result = false;
            }

            if (eval1 != eval3) {
                System.out.println("Asymmetric east/west eval detected");
                System.out.println(fen);
                System.out.println(FenHelper.mirror(fen));
                result.result = false;
            }

        }

        return result;
    }
}
