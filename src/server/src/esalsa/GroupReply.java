package esalsa;

import java.io.IOException;

public class GroupReply extends ServerMessage {

    public static final int TYPE_ACTIVE     = 0;
    public static final int TYPE_SEPERATIST = 1;
    public static final int TYPE_IDLE       = 2;

    // These a the new virtual communicator values.
    public final int newComm;
    public final int rank;
    public final int size;

    // This field indicates if this group is active, is splitting off, or is idle. 
    public final int type;

    // These contain info about the distribution of the virtual communicator.
    public final int clusterCount;
    public final int flags;

    public final int [] coordinators;
    public final int [] clusterSizes;
    public final int [] members;

    public final int [] clusterRanks;
    public final int [] memberClusterIndex;
    public final int [] localRanks;


    GroupReply(boolean overlap) {

        super(Protocol.OPCODE_GROUP_REPLY, 6*4);

        this.newComm = -1;
        this.rank = -1;
        this.size = -1;
        this.flags = -1;
        this.clusterCount = -1;
        this.members = null;
        this.clusterSizes = null;
        this.coordinators = null;
        this.clusterRanks = null;
        this.memberClusterIndex = null;
        this.localRanks = null;

        if (overlap) {
            this.type = TYPE_SEPERATIST;
        } else {
            this.type = TYPE_IDLE;
        }
    }

    GroupReply(int newComm, int rank, int size, int clusterCount, int flags,
            int [] coordinators, int [] clusterSizes, int [] members,
            int [] clusterRanks, int [] memberClusterIndex, int [] localRanks) {
       
        super(Protocol.OPCODE_GROUP_REPLY, 6*4 + clusterCount*4*3 + size*4*3);

        this.newComm = newComm;
        this.rank = rank;
        this.size = size;
        this.type = TYPE_ACTIVE;
        this.clusterCount = clusterCount;
        this.flags = flags;

        this.coordinators = coordinators;
        this.clusterSizes = clusterSizes;
        this.members = members;

        this.clusterRanks = clusterRanks;
        this.memberClusterIndex = memberClusterIndex;
        this.localRanks = localRanks;
    }

    void write(EndianDataOutputStream out) throws IOException {

        System.err.println("WRITING GROUP REPLY of size " + (8*4 + clusterCount*3*4 + (type == TYPE_ACTIVE ? (size*3*4) : 0)) + 
                " " + clusterCount + " " + size);
        
        super.write(out);
        out.writeInt(newComm);
        out.writeInt(rank);
        out.writeInt(size);
        out.writeInt(type);
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

       if (type == TYPE_ACTIVE) {
            
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
    }
//
//    public long dataSize() {
//        return 4*5 + clusterCount*4*2 + size*4;
//    }
}
