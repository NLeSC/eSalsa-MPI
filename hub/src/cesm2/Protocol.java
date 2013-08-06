package cesm2;

public interface Protocol {

    public static final byte OPCODE_HANDSHAKE          = 42;
    public static final byte OPCODE_HANDSHAKE_ACCEPTED = 43;
    public static final byte OPCODE_HANDSHAKE_REJECTED = 44;

    public static final int OPCODE_DATA        = 50;

    public static final int OPCODE_COMM        = 51;
    public static final int OPCODE_COMM_REPLY  = 52;

    public static final int OPCODE_GROUP       = 53;
    public static final int OPCODE_GROUP_REPLY = 54;

    public static final int OPCODE_DUP         = 55;
    public static final int OPCODE_DUP_REPLY   = 56;

    public static final int OPCODE_TERMINATE   = 57;
        
    public static final int OPCODE_COLLECTIVE_BCAST = 60;
    
    public static final int OPCODE_CLOSE_LINK  = 127;      
}
