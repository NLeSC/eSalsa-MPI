package esalsa;

import java.io.DataOutputStream;
import java.io.IOException;

public class DupReply extends Message {

    // This is the new virtual communicator number
    public final int newComm;

    DupReply(int destination, int comm, int newComm) {       
        super(Protocol.OPCODE_DUP_REPLY, 0xFFFFFFFF, destination, 4);
        this.newComm = newComm;
    }

    void write(DataOutputStream out) throws IOException {
        super.write(out);
        out.writeInt(newComm);
    }
}
