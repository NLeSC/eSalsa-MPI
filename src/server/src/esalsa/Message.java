package esalsa;

import java.io.IOException;

public abstract class Message {

    /** Standard message header. */
    
    /** Message opcode */
    public final int opcode;
    
    /** Source PID */ 
    public final int source;
    
    /** Destination PID */
    public final int destination;
    
    /** Message length */
    public final int length;
    
    /**
     * Constructor that reads a Message from a {@link DataInputStream}.
     * 
     * @param opcode
     *          The message opcode (which has already been read).
     * @param in
     *          The {@link DataInputStream} from which the rest of the message must be read.
     * @throws IOException
     *          If an IO error occurred while reading the message. 
     */
    protected Message(int opcode, EndianDataInputStream in) throws IOException {
        this.opcode = opcode;
        source = in.readInt();
        destination = in.readInt();
        length = in.readInt();
    }
    
    /**
     * Constructor that creates a new Message.
     * 
     * @param opcode
     *          The message opcode.
     * @param source
     *          The source PID.
     * @param destination
     *          The destination PID.
     * @param payloadSize
     *          The size of the message payload in bytes (excluding the header). 
     * @param communicator
     *          The communicator to which this message belongs.
     */
    protected Message(int opcode, int source, int destination, int payloadSize) {
        this.opcode = opcode;
        this.source = source;
        this.destination = destination;
        this.length = 4*4 + payloadSize;
    }

    /**
     * Writes the message to the provided {@link DataOutputStream}.
     * 
     * @param out
     *          The {@link DataOutputStream} to write the message to. 
     * @throws IOException
     *          If an IO error occurred while writing the message. 
     */
    void write(EndianDataOutputStream out) throws IOException {
        out.writeInt(opcode);
        out.writeInt(source);
        out.writeInt(destination);
        out.writeInt(length);
    }
}
