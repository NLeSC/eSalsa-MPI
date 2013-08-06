package cesm2.messages;

import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.IOException;

import cesm2.Protocol;

public class TerminateMessage extends ApplicationMessage {

    TerminateMessage(DataInputStream in) throws IOException {
        super(Protocol.OPCODE_TERMINATE, in);
    }

    void write(DataOutputStream out) throws IOException {
        super.write(out);
    }
}
