package cesm2.messages;

import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import java.io.Serializable;

public abstract class Message implements Serializable {

    private static final long serialVersionUID = 4747708549012506358L;
    
    public final int opcode;
    public final long sourcePID;
    public final long destinationPID;

    public Message(int opcode, long sourcePID, long destinationPID) {
        this.opcode = opcode;
        this.sourcePID = sourcePID;
        this.destinationPID = destinationPID;
    }
    
    public Message(int opcode, DataInputStream in) throws IOException {
        this.opcode = opcode;
        this.sourcePID = in.readLong();
        this.destinationPID = in.readLong();
    }
        
    void write(DataOutputStream out) throws IOException {       
        out.writeInt(opcode);
        out.writeLong(sourcePID);
        out.writeLong(destinationPID);        
    }
    
    public long dataSize() {
        return 0;
    }

    public long headerSize() {
        return 20;
    }

    public long size() {
        return headerSize() + dataSize();
    }
}
