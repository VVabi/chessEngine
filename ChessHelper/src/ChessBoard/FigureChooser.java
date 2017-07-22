package ChessBoard;

import javafx.application.Platform;
import javafx.beans.property.ObjectProperty;
import javafx.beans.property.SimpleObjectProperty;
import javafx.event.EventHandler;
import javafx.scene.canvas.Canvas;
import javafx.scene.canvas.GraphicsContext;
import javafx.scene.image.Image;
import javafx.scene.input.MouseEvent;
import javafx.scene.paint.Color;
import javafx.scene.paint.CycleMethod;
import javafx.scene.paint.LinearGradient;
import javafx.scene.paint.Stop;

/**
 * Created by fabian on 15.08.15.
 */
public class FigureChooser extends Canvas {

    Image whiteking;
    Image whitequeen;
    Image whiterook;
    Image whitebishop;
    Image whiteknight;
    Image whitepawn;
    Image blackking;
    Image blackqueen;
    Image blackrook;
    Image blackbishop;
    Image blackknight;
    Image blackpawn;
    int fieldsize = 60;
    char chosenfigure = '0';
    int chosenfield = -1;
    char movingside = 'w';
    private ObjectProperty<EventHandler<MouseEvent>> propertyOnAction = new SimpleObjectProperty<EventHandler<MouseEvent>>();

    private void setMouseclickedEvents(){
        setOnMouseClicked(new EventHandler<MouseEvent>() {
            @Override
            public void handle(MouseEvent event) {
                onActionProperty().get().handle(event);

            }
        });
    }

    public FigureChooser(){
        super(2*60, 7*60);
        importPictures();
        draw();
        setMouseclickedEvents();

    }

    public char getChosenFigure(){
        return chosenfigure;

    }

    public final ObjectProperty<EventHandler<MouseEvent>> onActionProperty() {
        return propertyOnAction;
    }

    public final void setOnAction(EventHandler<MouseEvent> handler) {
        propertyOnAction.set(handler);
    }

    public final EventHandler<MouseEvent> getOnAction() {
        return propertyOnAction.get();

    }

    private class figure{
        Image im;
        char type;
        figure(Image i, char t){
            im = i;
            type = t;
        }
    }

    public void setMovingside(char c){
        movingside = c;
        draw();
    }

    public char getMovingside(){
        return movingside;
    }

    public void toggleMovingSide(){
        if(movingside == 'b'){
            movingside = 'w';
        }
        else{
            movingside = 'b';
        }
        draw();
    }


    public void setChosenFigure(int fig){
        char oldchosenfigure = chosenfigure;
        chosenfield = fig;
        switch(fig){
            case 1:
                chosenfigure = 'K';
                break;
            case 2:
                chosenfigure = 'Q';
                break;
            case 3:
                chosenfigure = 'R';
                break;
            case 4:
                chosenfigure = 'B';
                break;
            case 5:
                chosenfigure = 'N';
                break;
            case 6:
                chosenfigure = 'P';
                break;
            case 7:
                chosenfigure = 'k';
                break;
            case 8:
                chosenfigure = 'q';
                break;
            case 9:
                chosenfigure = 'r';
                break;
            case 10:
                chosenfigure = 'b';
                break;
            case 11:
                chosenfigure = 'n';
                break;
            case 12:
                chosenfigure = 'p';
                break;
            default:
                break;

        }


        if(oldchosenfigure == chosenfigure){
            chosenfigure = '0';
        }

        draw();

    }

    private void importPictures(){
        whiteking = new Image(getClass().getResourceAsStream("whiteking.png"));
        whitequeen = new Image(getClass().getResourceAsStream("whitequeen.png"));
        whiterook = new Image(getClass().getResourceAsStream("whiterook.png"));
        whitebishop = new Image(getClass().getResourceAsStream("whitebishop.png"));
        whiteknight= new Image(getClass().getResourceAsStream("whiteknight.png"));
        whitepawn = new Image(getClass().getResourceAsStream("whitepawn.png"));
        blackking = new Image(getClass().getResourceAsStream("blackking.png"));
        blackqueen = new Image(getClass().getResourceAsStream("blackqueen.png"));
        blackrook = new Image(getClass().getResourceAsStream("blackrook.png"));
        blackbishop = new Image(getClass().getResourceAsStream("blackbishop.png"));
        blackknight= new Image(getClass().getResourceAsStream("blackknight.png"));
        blackpawn = new Image(getClass().getResourceAsStream("blackpawn.png"));
    }

    private void draw(){
        Platform.runLater(new Runnable() {

            @Override
            public void run() {
                GraphicsContext gc = getGraphicsContext2D();
                gc.clearRect(0, 0, 2 * fieldsize, 7 * fieldsize);
                if(chosenfigure != '0'){
                    Stop[] stops = new Stop[] { new Stop(0, Color.WHITE), new Stop(1, Color.CORAL)};
                    LinearGradient lg1 = new LinearGradient(0, 0, 1, 1, true, CycleMethod.NO_CYCLE, stops);
                    gc.setFill(lg1);
                    gc.fillRect( ((chosenfield-1)/6)*fieldsize, ((chosenfield-1) % 6)*fieldsize,fieldsize, fieldsize);
                }
                gc.drawImage(whiteking, 0, 0 * fieldsize, fieldsize, fieldsize);
                gc.drawImage(whitequeen, 0, 1 * fieldsize, fieldsize, fieldsize);
                gc.drawImage(whiterook, 0, 2 * fieldsize, fieldsize, fieldsize);
                gc.drawImage(whitebishop, 0, 3 * fieldsize, fieldsize, fieldsize);
                gc.drawImage(whiteknight, 0, 4 * fieldsize, fieldsize, fieldsize);
                gc.drawImage(whitepawn, 0, 5 * fieldsize, fieldsize, fieldsize);
                gc.drawImage(blackking, fieldsize, 0, fieldsize, fieldsize);
                gc.drawImage(blackqueen, fieldsize, 1 * fieldsize, fieldsize, fieldsize);
                gc.drawImage(blackrook, fieldsize, 2 * fieldsize, fieldsize, fieldsize);
                gc.drawImage(blackbishop, fieldsize, 3 * fieldsize, fieldsize, fieldsize);
                gc.drawImage(blackknight, fieldsize, 4 * fieldsize, fieldsize, fieldsize);
                gc.drawImage(blackpawn, fieldsize, 5 * fieldsize, fieldsize, fieldsize);

                Stop[] stops = new Stop[]{new Stop(0, Color.WHITE), new Stop(1, Color.LIGHTGRAY)};
                if (movingside == 'b') {
                    stops = new Stop[]{new Stop(0, Color.DARKGRAY), new Stop(1, Color.BLACK)};
                }
                LinearGradient lg1 = new LinearGradient(0, 0, 1, 1, true, CycleMethod.NO_CYCLE, stops);
                gc.setFill(lg1);
                gc.fillRect(0.5 * fieldsize, 6.4 * fieldsize, 1.0 * fieldsize, 1.0 * fieldsize);

            }
        });



    }

}
