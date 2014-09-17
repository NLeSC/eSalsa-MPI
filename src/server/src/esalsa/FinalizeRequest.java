package esalsa;

import java.io.IOException;

public class FinalizeRequest extends CommunicatorRequest {
    
    FinalizeRequest(EndianDataInputStream in) throws IOException {
        super(Protocol.OPCODE_FINALIZE, in);
    }
}
