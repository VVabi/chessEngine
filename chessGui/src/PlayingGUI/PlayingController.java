package PlayingGUI;
//r0000000000000000R000000000b0000p0r0pp0p0K00k0000000000000000000wnnnn

import ChessBoard.ChessBoard;

import com.google.gson.Gson;
import com.google.gson.GsonBuilder;
import core.ChessEvent;
import core.PEventInterface;
import core.PlayingEventInterface;
import core.events.*;
import javafx.application.Platform;
import javafx.fxml.FXML;
import javafx.scene.control.Label;
import javafx.scene.control.TextArea;
import javafx.scene.image.ImageView;
import javafx.scene.input.MouseEvent;
import EditorGUI.EditorGUI;

/**
 * Created by fabian on 15.08.15.
 */
public class PlayingController {

    @FXML private TextArea debugArea;
    @FXML private TextArea EvalDebugArea;
    @FXML private ChessBoard chessboard;
    @FXML private ImageView waitImage;
    boolean isClicked = false;
    EditorGUI editor= null;
    @FXML Label evaluationField;
    char sidetomove = 'w';

    int clickedfield = 0;

    core.PlayingEventInterface eventInterface;

    public PlayingController(){


    }

    @FXML
    public void handleEdit(){
        editor = new EditorGUI(this, chessboard.getPosition());
    }

    @FXML
    public void handleChangeSides(){
        eventInterface.addGuiEvent(new TurnBoardEvent());
    }

    public void returnFromEditor(String position){
        editor.exit();
       // evaluationField.setText("");
        NewPositionEvent e = new NewPositionEvent();
        e.newPosition    = position;
        eventInterface.addEngineEvent(e);
    }

    @FXML
    public synchronized void handleForceMove(){
        eventInterface.addEngineEvent(new ForceMoveEvent());
    }

    @FXML
    public void handleNewGame(){
        String position = "RNBQKBNRPPPPPPPP00000000000000000000000000000000pppppppprnbqkbnrwKQkq";
        NewPositionEvent e = new NewPositionEvent();
        e.newPosition    = position;
        eventInterface.addEngineEvent(e);

    }

    public void handleEvalDebugInfo(EvalDebugEvent e) {
        Gson gson = new GsonBuilder().setPrettyPrinting().create();
        String result = gson.toJson(e.evalDebugStruct);

        Platform.runLater(new Runnable() {
            @Override
            public void run() {
                EvalDebugArea.clear();
                EvalDebugArea.setText(result);
                EvalDebugArea.appendText(Integer.toString(e.valid));
            }
        });
    }


    public void handleDebugInfo(SearchDebugEvent e) {

        Platform.runLater(new Runnable() {
            @Override
            public void run() {
                debugArea.clear();
                int eval  =e.eval;
                String evalString = Integer.toString(eval);
                if(eval < -20000){
                    int mateIn = (eval+30000+e.depth)/2;
                    evalString = "Will be mated in "+mateIn;
                }
                if(eval > 20000){
                    int mateIn = (eval-30000+e.depth-1)/2-1;
                    evalString = "Mate in "+mateIn;
                }
                debugArea.appendText("Depth " + e.depth+"\n");
                debugArea.appendText("Eval  " + evalString+" "+e.eval+"\n");
                debugArea.appendText("Nodes " + e.nodes + " in " + e.time + " ms"+"\n");
                debugArea.appendText("NPS " + ((int) (1000.0*((double) e.nodes)/((double) e.time)))+"\n");
                debugArea.appendText("Best move " + e.move+"\n");
            }
        });

    }

    @FXML
    public void handleUndo(){
        eventInterface.addEngineEvent(new UndoMoveEvent());
    }

    @FXML
    public void handleAddToTests(){


    }


    @FXML
    public void handleRunMateTests(){

    }


    private void findMove(){
        /*String newPosition = messenger.find_move();
        chessboard.setPosition(newPosition.substring(0, 64));
        char originalSideToMove = sidetomove;
        sidetomove = newPosition.charAt(64);

        System.out.println(newPosition);

        String[] parts = newPosition.split(":");

        int eval = Integer.parseInt(parts[1]);
        if((originalSideToMove == 'b')){
            eval = -eval;
        }
        String evalString = new Integer(eval).toString();
        if(eval > 50000){
            int mateIn = 100000-eval;
            evalString = "White mates in "+mateIn/2;
        }
        if(eval < -50000){
            int mateIn = 100000+eval;
            evalString = "Black mates in "+mateIn/2;
        }

        if((eval == 99999) || (eval == 100000)){
            evalString = "Black is checkmate";
        }
        if((eval == -99999) || (eval == -100000)){
            evalString = "White is checkmate";
        }

        final String usedEvalString = evalString;
        Platform.runLater(new Runnable() {
            @Override
            public void run() {

                evaluationField.setText(usedEvalString);

            }
        });*/

    }




    @FXML
    public void handleClicked(MouseEvent event){
        double x = event.getX();
        double y = event.getY();
        int field = chessboard.getField(x,y);
        char piece = chessboard.getPieceOnField(field);
        boolean isOccupied = false;
        if(piece != '0'){
            if(Character.isUpperCase(piece) && (sidetomove=='w')){
                isOccupied = true;
            }
            if(Character.isLowerCase(piece) && (sidetomove=='b')){
                isOccupied = true;
            }
        }


        if(isClicked){
            isClicked = false;
            chessboard.clearColors();
            String move = fieldsToMove(clickedfield, field);
            CheckMoveEvent e = new CheckMoveEvent();
            e.move = move;
            System.out.println("Added checked move event " +move);
            eventInterface.addEngineEvent(e);
           /*moveThread T = new moveThread(move);
            T.start();*/

        }
        else if(!isOccupied){
            isClicked = false;
            chessboard.clearColors();
        }
        else{
            isClicked=true;
            clickedfield = field;
            chessboard.colorField(field);
        }

    }

    private String fieldsToMove(int originfield, int targetfield){
        String targetcolumn = Character.toString((char) ('a'+(targetfield % 8)));
        String targetrow = Character.toString((char) ('0'+(targetfield/8)+1));
        String origincolumn;
        String originrow;
        origincolumn= Character.toString((char) ('a'+(originfield % 8)));
        originrow = Character.toString((char) ('0'+originfield/8+1));
        String ownmove = origincolumn+originrow+targetcolumn+targetrow;
        return ownmove;
    }

    private void setPosition(String position) {
        chessboard.setPosition(position);

    }


    public void dispose(){
        //messenger.dispose();
    }

    public void setEventInterface(PlayingEventInterface eventInterface) {
        this.eventInterface = eventInterface;
    }


    Thread eventThread;

    public void reactToEvent(ChessEvent e){

        if(e instanceof TurnBoardEvent){
            chessboard.toggleSideDown();
        }
        if(e instanceof NewPositionEvent){
            sidetomove = ((NewPositionEvent) e).newPosition.charAt(64);
            chessboard.setPosition(((NewPositionEvent) e).newPosition);
        }
        if(e instanceof SearchDebugEvent){
            handleDebugInfo((SearchDebugEvent) e);
        }

        if(e instanceof EvalDebugEvent) {
            handleEvalDebugInfo((EvalDebugEvent) e);
        }



    }

    public void runEventThread(){
        while(true) {
            ChessEvent event = eventInterface.consumeGuiEvent();
            if(event != null){
                reactToEvent(event);
            }
            try {
                Thread.sleep(20);
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }
    }

    public void startEventThread(){
        Runnable r = new Runnable() {
            @Override
            public void run() {
                runEventThread();
            }
        };
        Thread T = new Thread(r);
        T.start();
    }
}
