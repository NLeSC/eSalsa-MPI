package cesm;

import java.io.DataOutputStream;
import java.io.IOException;

public class CommReply extends Message {

    // These a the new virtual communicator values.
    public final int newComm;
    public final int rank;
    public final int size;

    // These are needed to correctly split the real communicator on the host.
    public final int color;
    public final int key;

    // These contain info about the distribution of the virtual communicator.
    public final int clusterCount;
    public final int flags;

    // TODO: merge clusterRanks + clusterSizes and memberClusterIndex+localRanks
    public final int [] coordinators;
    public final int [] clusterSizes;
    public final int [] members;

    public final int [] clusterRanks;
    public final int [] memberClusterIndex;
    public final int [] localRanks;

    CommReply(int comm, int newComm,
              int rank, int size,
              int color, int key,
              int clusterCount, int flags,
              int [] coordinators, int [] clusterSizes, int [] members,
              int [] clusterRanks, int [] memberClusterIndex, int [] localRanks) {

        super(Protocol.OPCODE_COMM_REPLY, comm, -1);

        this.newComm = newComm;
        this.rank = rank;
        this.size = size;
        this.color = color;
        this.key = key;

        this.clusterCount = clusterCount;
        this.flags = flags;
        this.coordinators = coordinators;
        this.clusterSizes = clusterSizes;
        this.members = members;

        this.clusterRanks = clusterRanks;
        this.memberClusterIndex = memberClusterIndex;
        this.localRanks = localRanks;
    }

    void write(DataOutputStream out) throws IOException {
        super.write(out);
        out.writeInt(newComm);
        out.writeInt(rank);
        out.writeInt(size);
        out.writeInt(color);
        out.writeInt(key);
        out.writeInt(clusterCount);
        out.writeInt(flags);

        for (int i=0;i<clusterCount;i++) {
            out.writeInt(coordinators[i]);
        }

        for (int i=0;i<clusterCount;i++) {
            out.writeInt(clusterSizes[i]);
        }

        for (int i=0;i<clusterCount;i++) {
            out.writeInt(clusterRanks[i]);
        }

        // NOTE: size may be 0!
        for (int i=0;i<size;i++) {
            out.writeInt(members[i]);
        }

        for (int i=0;i<size;i++) {
            out.writeInt(memberClusterIndex[i]);
        }

        for (int i=0;i<size;i++) {
            out.writeInt(localRanks[i]);
        }
    }

    public long dataSize() {
        return 4*7 + clusterCount*4*2 + size*4;
    }
}
