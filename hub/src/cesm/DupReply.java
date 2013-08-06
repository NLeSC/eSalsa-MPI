package cesm;

import java.io.DataOutputStream;
import java.io.IOException;

public class DupReply extends Message {

    // This is the new virtual communicator number
    public final int newComm;

    DupReply(int comm, int newComm) {
        super(Protocol.OPCODE_DUP_REPLY, comm, -1);
        this.newComm = newComm;
    }

    void write(DataOutputStream out) throws IOException {
        super.write(out);
        out.writeInt(newComm);
    }
    
    public long dataSize() { 
        return 4;
    }
}
