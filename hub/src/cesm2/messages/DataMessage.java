package cesm2.messages;

import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.IOException;

public class DataMessage extends Message {

    private static final long serialVersionUID = -4540832733953628622L;

    public final int communicator;
    public final int source;    
    public final int destination;
    public final int tag;
    public final int count;
    public final int bytes;
    public final byte [] data;

    DataMessage(int opcode, DataInputStream in) throws IOException {
        super(opcode, in);
        communicator = in.readInt();        
        source = in.readInt();        
        destination = in.readInt();
        tag = in.readInt();
        count = in.readInt();
        bytes = in.readInt();

        data = new byte[bytes];

        in.readFully(data);
    }

    void write(DataOutputStream out) throws IOException {
        super.write(out);
        out.writeInt(communicator);
        out.writeInt(source);        
        out.writeInt(destination);
        out.writeInt(tag);
        out.writeInt(count);
        out.writeInt(bytes);
        out.write(data);
    }	

    public long dataSize() { 
        return bytes; 
    }

    public long headerSize() { 
        return super.headerSize() + 24; 
    }
}
