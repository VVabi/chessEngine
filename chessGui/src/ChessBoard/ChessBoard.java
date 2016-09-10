package ChessBoard;

import javafx.application.Platform;
import javafx.beans.property.ObjectProperty;
import javafx.beans.property.SimpleObjectProperty;
import javafx.event.EventHandler;
import javafx.scene.canvas.Canvas;
import javafx.scene.input.MouseEvent;
import javafx.scene.layout.Pane;

/**
 * Created by fabian on 14.08.15.
 */
public class ChessBoard extends Pane {

    String position = "RNBQKBNRPPPPPPPP00000000000000000000000000000000pppppppprnbqkbnr";
    boolean whitesidedown = true;
    int fieldsize = 60;
    fieldCanvas fieldcanvas;
    coloredfieldCanvas coloredfieldcanvas;
    figuresCanvas figurescanvas;
    private ObjectProperty<EventHandler<MouseEvent>> propertyOnAction = new SimpleObjectProperty<EventHandler<MouseEvent>>();

    public ChessBoard(){
        setPrefSize(8*fieldsize, 8*fieldsize);
        fieldcanvas = new fieldCanvas(fieldsize);
        coloredfieldcanvas = new coloredfieldCanvas(fieldsize);
        figurescanvas = new figuresCanvas(fieldsize);
        getChildren().addAll(fieldcanvas, coloredfieldcanvas, figurescanvas);
        fieldcanvas.draw();
        drawFigures();
        setMouseclickedEvents();

    }

    public void toggleSideDown(){
        whitesidedown = !whitesidedown;
        drawFigures();
    }

    public String getPosition(){
        return position;

    }


    public void setPosition(String newposition){
        position = newposition;
        drawFigures();

    }

    private void drawFigures(){

        Platform.runLater(new Runnable() {
                @Override
                public void run() {
                    figurescanvas.draw(whitesidedown, position);
            }
        });


    }

    private void setMouseclickedEvents(){
        setOnMouseClicked(new EventHandler<MouseEvent>() {
            @Override
            public void handle(MouseEvent event) {
                onActionProperty().get().handle(event);

            }
        });
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

    public void clearColors(){
        coloredfieldcanvas.clear();


    }

    public void colorField(int field){
        coloredfieldcanvas.colorField(whitesidedown, field);


    }

    public char getPieceOnField(int field){
        return position.charAt(field);
    }

    public int getField(double xcoord, double ycoord){
        int x = (int) xcoord;
        int y = (int) ycoord;
        int field;
        x = x/fieldsize;
        y = y/fieldsize;
        if(whitesidedown){
            field = 8*(7-y)+x;
        }
        else{
            field = 8*y+(7-x);
        }
        return field;
    }


}
