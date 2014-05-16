package esalsa;

import java.io.DataInputStream;
import java.io.IOException;

public class GroupRequest extends CommunicatorRequest {

    // The size of the group.
    public final int size;

    // The group members.
    public final int [] pids;

    GroupRequest(DataInputStream in) throws IOException {

        super(Protocol.OPCODE_GROUP, in);

        size = in.readInt();

        pids = new int[size];

        for (int i=0;i<size;i++) { 
            pids[i] = in.readInt();
        }
    }
//
//    public long dataSize() { 
//        return 2*4 + 4*pids.length;
//    }
}
