package esalsa;

import java.io.DataInputStream;
import java.io.IOException;

public class FreeRequest extends CommunicatorRequest {

    FreeRequest(DataInputStream in) throws IOException {
        super(Protocol.OPCODE_FREE, in);
    }
}
