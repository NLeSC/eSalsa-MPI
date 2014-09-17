package esalsa;

import java.io.IOException;

public class FinalizeReply extends Message {
    FinalizeReply(int destination) {       
        super(Protocol.OPCODE_FINALIZE_REPLY, 0xFFFFFFFF, destination, 0);
    }
    
    void write(EndianDataOutputStream out) throws IOException {
        super.write(out);
    }
}
