package cesm2.messages;

import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.IOException;

import cesm2.Protocol;

public class SplitRequest extends CommunicatorRequest {

    private static final long serialVersionUID = 4983088229006084306L;
    
    public final int color;
    public final int key;

    SplitRequest(DataInputStream in) throws IOException {
        super(Protocol.OPCODE_COMM, in);
        color = in.readInt();
        key = in.readInt();
    }

    void write(DataOutputStream out) throws IOException {
        super.write(out);
        out.writeInt(color);
        out.writeInt(key);
    }	
    
    public long dataSize() { 
        return 2*4;
    }
}
