package cesm;

import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.IOException;

public class TerminateMessage extends Message {

    TerminateMessage(DataInputStream in) throws IOException {
        super(Protocol.OPCODE_TERMINATE, in);
    }

    void write(DataOutputStream out) throws IOException {
        super.write(out);
    }
}
