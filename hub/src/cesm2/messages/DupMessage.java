package cesm2.messages;

import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.IOException;

import cesm2.Protocol;

public class DupMessage extends CommunicatorRequest {

    private static final long serialVersionUID = 5466278219055438817L;

    DupMessage(DataInputStream in) throws IOException {
        super(Protocol.OPCODE_DUP, in);
    }

    void write(DataOutputStream out) throws IOException {
        super.write(out);
    }
}
