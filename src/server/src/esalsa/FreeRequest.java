package esalsa;

import java.io.IOException;

public class FreeRequest extends CommunicatorRequest {

    FreeRequest(EndianDataInputStream in) throws IOException {
        super(Protocol.OPCODE_FREE, in);
    }
}
