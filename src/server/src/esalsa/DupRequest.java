package esalsa;

import java.io.IOException;

public class DupRequest extends CommunicatorRequest {

    DupRequest(EndianDataInputStream in) throws IOException {
        super(Protocol.OPCODE_DUP, in);
    }
}
