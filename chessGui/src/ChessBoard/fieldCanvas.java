package ChessBoard;

import javafx.scene.canvas.Canvas;
import javafx.scene.canvas.GraphicsContext;
import javafx.scene.paint.Color;
import javafx.scene.paint.CycleMethod;
import javafx.scene.paint.LinearGradient;
import javafx.scene.paint.Stop;

/**
 * Created by fabian on 14.08.15.
 */
public class fieldCanvas extends Canvas {
    int fieldsize;

    public fieldCanvas(int size){
        super(8*size, 8*size);
        fieldsize = size;
    }

    public void draw(){
        drawPlayingField();

    }



    private void drawPlayingField(){
        GraphicsContext gc = getGraphicsContext2D();
        for(int i = 0; i < 8; i++){
            for(int j = 0; j < 8; j++){
                if((i+j)%2 == 0){
                    Stop[] stops = new Stop[] { new Stop(0, Color.WHITE), new Stop(1, Color.LIGHTGRAY)};
                    LinearGradient lg1 = new LinearGradient(0, 0, 1, 1, true, CycleMethod.NO_CYCLE, stops);
                    gc.setFill(lg1);
                    gc.fillRect(i*fieldsize, j*fieldsize, fieldsize, fieldsize);
                }
            }
        }

    }


}
