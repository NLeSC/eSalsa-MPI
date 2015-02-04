package esalsa;

import java.io.IOException;

public class DupReply extends ServerMessage {

    // This is the new virtual communicator number
    public final int newComm;

    DupReply(int comm, int newComm) {       
        super(Protocol.OPCODE_DUP_REPLY, 4);
        this.newComm = newComm;
    }

    void write(EndianDataOutputStream out) throws IOException {
        super.write(out);
        out.writeInt(newComm);
    }
}
