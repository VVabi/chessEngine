package communication.messages;

/**
 * Created by fabian on 06.09.15.
 */
public class UnknownMessageException extends Exception {
    public UnknownMessageException() { super(); }
    public UnknownMessageException(String message) { super(message); }
    public UnknownMessageException(String message, Throwable cause) { super(message, cause); }
    public UnknownMessageException(Throwable cause) { super(cause); }
}
