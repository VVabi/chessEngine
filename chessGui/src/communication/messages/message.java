package communication.messages;


import java.io.IOException;

/**
 * Created by fabian on 05.09.15.
 */
public abstract class message {

    public abstract int getUniqueId();
    public abstract byte[] deflate();
    public abstract int getSize();

}
