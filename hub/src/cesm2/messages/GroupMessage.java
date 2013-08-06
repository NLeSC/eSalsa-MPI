package cesm2.messages;

import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.IOException;

import cesm2.Protocol;

public class GroupMessage extends CommunicatorRequest {

    private static final long serialVersionUID = -6300107887174713579L;
    
    public final int [] pids;

    GroupMessage(DataInputStream in) throws IOException {

        super(Protocol.OPCODE_GROUP, in);

        int size = in.readInt();

        pids = new int[size];

        for (int i=0;i<size;i++) { 
            pids[i] = in.readInt();
        }
    }

    void write(DataOutputStream out) throws IOException {
        super.write(out);
        out.writeInt(pids.length);

        for (int i=0;i<pids.length;i++) { 
            out.writeInt(pids[i]);
        }
    }	
    
    public long dataSize() { 
        return 4 * pids.length;
    }
}
