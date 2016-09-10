package VMPServer;

import java.util.HashMap;
import java.util.Map;

/**
 * Created by fabian on 21.12.15.
 */
public class ProtocolDefinitions {

    static Map<String, Integer> locationDefinitions;



    public static void initProtocol(String definition){
        String[] parts = definition.split(":");

        int locationId = 0;

        locationDefinitions = new HashMap<String, Integer>();

        for(String loc: parts){
            locationDefinitions.put(loc, locationId);
            locationId = locationId+1;
        }


    }

    public static Map<String,Integer> getLocations(){
        return locationDefinitions;
    }


}
