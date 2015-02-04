package esalsa;

import java.io.IOException;

public abstract class ServerMessage {

    /** Standard message header. */
    
    /** Message opcode */
    public final int opcode;
        
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
    protected ServerMessage(int opcode, EndianDataInputStream in) throws IOException {
        this.opcode = opcode;
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
    protected ServerMessage(int opcode, int payloadSize) {
        this.opcode = opcode;
        this.length = 2*4 + payloadSize;
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
        out.writeInt(length);
    }
}
