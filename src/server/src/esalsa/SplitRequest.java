package esalsa;

import java.io.IOException;

public class SplitRequest extends CommunicatorRequest {

    // The color for this comm-split.
    public final int color;

    // The key for this comm-split.
    public final int key;

    SplitRequest(EndianDataInputStream in) throws IOException {
        super(Protocol.OPCODE_SPLIT, in);
        color = in.readInt();
        key = in.readInt();
    }
}
