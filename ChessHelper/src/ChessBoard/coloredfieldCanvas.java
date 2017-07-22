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
public class coloredfieldCanvas extends Canvas {

    int fieldsize;
    boolean isColored = false;
    int coloredfield = 0;


    public coloredfieldCanvas(int size){
        super(8*size, 8*size);
        fieldsize = size;


    }

    public void onClicked(int field){




    }

    public void clear(){
        isColored = false;
        coloredfield = 0;
        draw();
    }

    public void colorField(boolean whitesidedown, int field){
        if(!whitesidedown){
            field = 63-field;


        }


        if((field >= 0) && (field < 64)){
            coloredfield = field;
            isColored = true;


        }
        draw();


    }

    private void draw(){
        GraphicsContext gc = getGraphicsContext2D();
        gc.clearRect(0,0,8*fieldsize,8*fieldsize);
        if(isColored) {
            int column = coloredfield % 8;
            int row    = 7-coloredfield/8;
            Stop[] stops = new Stop[]{new Stop(0, Color.WHITE), new Stop(1, Color.CORAL)};
            LinearGradient lg1 = new LinearGradient(0, 0, 1, 1, true, CycleMethod.NO_CYCLE, stops);
            gc.setFill(lg1);
            gc.fillRect(column * fieldsize, row * fieldsize, fieldsize, fieldsize);
        }

    }


}
