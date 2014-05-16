package esalsa;

import java.io.DataInputStream;
import java.io.IOException;

public class FinalizeRequest extends CommunicatorRequest {
    
    FinalizeRequest(DataInputStream in) throws IOException {
        super(Protocol.OPCODE_FINALIZE, in);
    }
}
