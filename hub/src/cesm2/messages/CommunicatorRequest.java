package cesm2.messages;

import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.IOException;

public abstract class CommunicatorRequest extends Message {

    private static final long serialVersionUID = -7831330310635559953L;
    
    public final int communicator;
    public final int source;

    protected CommunicatorRequest(int opcode, DataInputStream in) throws IOException {
        super(opcode, in);        
        communicator = in.readInt();
        source = in.readInt();
    }

    protected CommunicatorRequest(int opcode, long sourcePID, long destinationPID, int comm, int source) {
        super(opcode, sourcePID, destinationPID);
        this.communicator = comm;
        this.source = source;
    }

    void write(DataOutputStream out) throws IOException {
        super.write(out);
        out.writeInt(communicator);
        out.writeInt(source);
    }

    public long headerSize() {
        return super.headerSize() + 8;
    }

    public boolean isData() {
        return false;
    }
}
