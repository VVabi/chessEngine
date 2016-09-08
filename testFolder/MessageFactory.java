package messages;
public class MessageFactory{
static int header_size = 4;
	public static message getMessage(byte[] deflated) throws UnknownMessageException{
		int unique_id = typeConv.byte_to_int(deflated, 0);
		message m;
		switch(unique_id){
			case MessageIDs.VMPChesssMove_unique_id:
				m = new VMPChesssMove(deflated);
				break;
			default:
				throw new UnknownMessageException(new Integer(unique_id).toString());
		}
		return m;
	}
}
