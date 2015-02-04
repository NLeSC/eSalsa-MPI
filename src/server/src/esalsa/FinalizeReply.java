package esalsa;

import java.io.IOException;

public class FinalizeReply extends ServerMessage {
    FinalizeReply() {       
        super(Protocol.OPCODE_FINALIZE_REPLY, 0);
    }
    
    void write(EndianDataOutputStream out) throws IOException {
        super.write(out);
    }
}
