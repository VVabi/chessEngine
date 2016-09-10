package EditorGUI;

import ChessBoard.FigureChooser;
import ChessBoard.ChessBoard;
import PlayingGUI.PlayingController;
import javafx.fxml.FXML;
import javafx.scene.Parent;
import javafx.scene.input.MouseEvent;
import javafx.stage.Stage;

/**
 * Created by fabian on 15.08.15.
 */
public class EditorController {

    @FXML
    private FigureChooser chooser;
    @FXML
    private ChessBoard chessboard;


    PlayingController pcontroller;
    public EditorController(){



    }



    public void setPlayController(PlayingController p){
        pcontroller = p;

    }
    @FXML
    public void handleClicked(MouseEvent event){
        int field = chessboard.getField(event.getX(), event.getY());
        char figure = chooser.getChosenFigure();
        String position = chessboard.getPosition();
        String newposition = position.substring(0,field)+figure+position.substring(field+1);
        chessboard.setPosition(newposition);

    }

    @FXML
    public void handleClear(){
        chessboard.setPosition("0000000000000000000000000000000000000000000000000000000000000000");

    }

    @FXML
    public void handlePlay(){
        System.out.println(getPosition()+chooser.getMovingside()+"nnnn");
        pcontroller.returnFromEditor(getPosition()+chooser.getMovingside()+"nnnn");

    }

    @FXML
    public void handleChooserClicked(MouseEvent event){
        int x= (int) event.getX();
        int y= (int) event.getY();

        y = y/60+1;
        x = x/60;
        if(y < 7) {
            chooser.setChosenFigure(6 * x + y);
        }
        else{
            chooser.toggleMovingSide();
        }
    }

    public String getPosition(){
        return chessboard.getPosition();
    }

    public void setPosition(String pos){
        chessboard.setPosition(pos);
    }

}
