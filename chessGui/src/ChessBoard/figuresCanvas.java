package ChessBoard;

import javafx.scene.canvas.Canvas;
import javafx.scene.canvas.GraphicsContext;
import javafx.scene.image.Image;

/**
 * Created by fabian on 14.08.15.
 */
public class figuresCanvas extends Canvas {
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

    int fieldsize;

    public figuresCanvas(int size){
        super(8*size, 8*size);
        fieldsize = size;
        importPictures();
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


    public void draw(boolean whitesidedown, String position){
        int row;
        int column;
        GraphicsContext gc = getGraphicsContext2D();
        if (position.length() >= 64) {
            gc.clearRect(0, 0, 8 * fieldsize, 8 * fieldsize);
            for (int i = 0; i < 64; i++) {
                row = i / 8;
                column = i % 8;
                int rowindex;
                int columnindex;
                if (whitesidedown) {
                    rowindex = 7 - row;
                    columnindex = column;

                } else {
                    rowindex = row;
                    columnindex = 7 - column;
                }
                switch (position.charAt(i)) {
                    case 'k':
                        gc.drawImage(blackking, columnindex * fieldsize, rowindex * fieldsize, fieldsize, fieldsize);
                        break;
                    case 'q':
                        gc.drawImage(blackqueen, columnindex * fieldsize, rowindex * fieldsize, fieldsize, fieldsize);
                        break;
                    case 'r':
                        gc.drawImage(blackrook, columnindex * fieldsize, rowindex * fieldsize, fieldsize, fieldsize);
                        break;
                    case 'b':
                        gc.drawImage(blackbishop, columnindex * fieldsize, rowindex * fieldsize, fieldsize, fieldsize);
                        break;
                    case 'n':
                        gc.drawImage(blackknight, columnindex * fieldsize, rowindex * fieldsize, fieldsize, fieldsize);
                        break;
                    case 'p':
                        gc.drawImage(blackpawn, columnindex * fieldsize, rowindex * fieldsize, fieldsize, fieldsize);
                        break;
                    case 'K':
                        gc.drawImage(whiteking, columnindex * fieldsize, rowindex * fieldsize, fieldsize, fieldsize);
                        break;
                    case 'Q':
                        gc.drawImage(whitequeen, columnindex * fieldsize, rowindex * fieldsize, fieldsize, fieldsize);
                        break;
                    case 'R':
                        gc.drawImage(whiterook, columnindex * fieldsize, rowindex * fieldsize, fieldsize, fieldsize);
                        break;
                    case 'B':
                        gc.drawImage(whitebishop, columnindex * fieldsize, rowindex * fieldsize, fieldsize, fieldsize);
                        break;
                    case 'N':
                        gc.drawImage(whiteknight, columnindex * fieldsize, rowindex * fieldsize, fieldsize, fieldsize);
                        break;
                    case 'P':
                        gc.drawImage(whitepawn, columnindex * fieldsize, rowindex * fieldsize, fieldsize, fieldsize);
                        break;
                    default:
                        break;
                }


            }
    }

}




}
