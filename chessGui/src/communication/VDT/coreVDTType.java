package communication.VDT;

/**
 * Created by fabian on 21.12.15.
 */
abstract public class coreVDTType {

    public static int getArraySize(coreVDTType[] arr){
        int ret = 0;
        for(int i=0; i < arr.length; i++){
            ret = ret+arr[i].getSize();
        }
        return ret;
    }

    abstract public int getSize();



}
