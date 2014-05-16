package esalsa;

import java.io.DataInputStream;
import java.io.IOException;

public class DupRequest extends CommunicatorRequest {

    DupRequest(DataInputStream in) throws IOException {
        super(Protocol.OPCODE_DUP, in);
    }
}
