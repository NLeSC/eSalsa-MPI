package esalsa;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collections;
import java.util.Comparator;
import java.util.HashMap;
import java.util.HashSet;
import java.util.LinkedList;

//import esalsa.junk.Connection;
//import esalsa.junk.DataMessage;

public class Communicator {

    private final static boolean SANITY = true;

    private static final int COMM_FLAG_LOCAL  = (1 << 0);
    private static final int COMM_FLAG_REMOTE = (1 << 1);

    private final int communicator;
    private final int [] coordinatorRanks;

    private final int [] clusterRanks;
    private final int [] clusterSizes;

    private final int [] members;
    private final int [] localRanks;
    private final int [] memberClusterIndex;

    private final int size;
    private final Server parent;

    private final Message [] messages;
    private int participants = 0;

    private long commMessages;
    private long commReplies;
    private long dataMessages;
    private long bcastMessages;

    private long commBytes;
    private long commReplyBytes;
    private long dataBytes;
    private long bcastBytes;

    private class ClusterInfo {
        final Cluster cluster;
        //final int clusterRank;
        final int coordinatorRank;
        int size;

        public ClusterInfo(Cluster cluster, int clusterRank, int coordinatorRank) {
            this.cluster = cluster;
          //  this.clusterRank = clusterRank;
            this.coordinatorRank = coordinatorRank;
            size = 1;
        }

        public int increaseSize() {
            return size++;
        }
    }

    private class ColorComparator implements Comparator<SplitRequest> {

        @Override
        public int compare(SplitRequest o1, SplitRequest o2) {

            if (o1.key < o2.key) {
                return -1;
            } else if (o1.key > o2.key) {
                return 1;
            }

            // The keys are the same, so compare based on current rank.
            if (o1.rank < o2.rank) {
                return -1;
            } else if (o1.rank > o2.rank) {
                return 1;
            }

            // Should never happen ?
            return 0;
        }
    }

    private final ColorComparator colorComparator = new ColorComparator();

    Communicator(Server parent, int communicator, int [] members) {

//        if (SANITY) {
//            // Make sure the processes array doesn't contain any holes!
//            if (processes == null) {
//                Logging.println("ERROR: processes array null at communicator creation!");
//                throw new IllegalArgumentException("Processes array null at communicator creation");
//            }
//
//            for (int i=0;i<processes.length;i++) {
//                if (processes[i] == null) {
//                    Logging.println("ERROR: processes array entry " + i + " null at communicator creation!");
//                    throw new IllegalArgumentException("Processes array entry " + i + " null at communicator creation");
//                }
//            }
//        }

        this.parent = parent;
        this.communicator = communicator;
        this.members = members;
        this.size = members.length;
        this.messages = new Message[size];

        this.localRanks = new int[size];
        this.memberClusterIndex = new int[size];

        ArrayList<ClusterInfo> info = new ArrayList<ClusterInfo>();
        HashMap<Integer, ClusterInfo> infoMap = new HashMap<Integer, ClusterInfo>();

        for (int i=0;i<size;i++) {

            int clusterRank = getClusterRank(members[i]);

            ClusterInfo tmp = infoMap.get(clusterRank);

            if (tmp == null) {
                // NOTE: the coordinator rank is "i" which is a global rank ??                
                tmp = new ClusterInfo(parent.getCluster(clusterRank), clusterRank, i);
                info.add(tmp);
                infoMap.put(clusterRank, tmp);
                localRanks[i] = 0;
            } else {
                localRanks[i] = tmp.increaseSize();
            }

            memberClusterIndex[i] = info.size()-1;
        }

        this.coordinatorRanks = new int[info.size()];
        this.clusterRanks = new int[info.size()];
        this.clusterSizes = new int[info.size()];

        for (int i=0;i<info.size();i++) {
            ClusterInfo tmp = info.get(i);
            this.coordinatorRanks[i] = tmp.coordinatorRank;
            this.clusterRanks[i] = tmp.cluster.getRank();
            this.clusterSizes[i] = tmp.size;
        }
    }

    private int getClusterRank(int pid) {
        return ((pid & 0xFF000000) >> 24) & 0xFF;
    }
    
    private int getProcessRank(int pid) {
        return (pid & 0xFFFFFF);
    }
    
    public int getNumber() {
        return communicator;
    }

    public int [] getCoordinatorRanks() {
        return coordinatorRanks;
    }

    private int [] getClusterSizes() {
        return clusterSizes;
    }

    private int [] getClusterRanks() {
        return clusterRanks;
    }

    private int [] getMembers() {
        return members;
    }

    private int [] getLocalRanks() {
        return localRanks;
    }

    private int [] getMemberClusterIndex() {
        return memberClusterIndex;
    }

    private int generateFlags(int [] memberPIDs) {

        if (memberPIDs == null || memberPIDs.length == 0) {
            Logging.error("generateFlags called for empty member set!");
            return 0;
        }

        int cluster = getClusterRank(memberPIDs[0]);
        
        for (int i=1;i<memberPIDs.length;i++) {
            if (cluster != getClusterRank(memberPIDs[i])) {
                return (COMM_FLAG_LOCAL | COMM_FLAG_REMOTE);
            }
        }

        return COMM_FLAG_LOCAL;
    }

    private void enqueueReply(Message m) { 
        int destinationCluster = getClusterRank(m.destination);
        
        parent.enqueueReply(destinationCluster, m);
        
        commReplies++;
        commReplyBytes += m.length;
    }


    private String printPID(int pid) {
        return ((pid & 0xFF000000) >> 24) + ":" + (pid & 0xFFFFFF);
    }

    private String printPIDs(int [] pids) {

        StringBuilder sb = new StringBuilder("[ ");

        for (int i=0;i<pids.length;i++) {

            sb.append(printPID(pids[i]));

            if (i != pids.length-1) {
                sb.append(", ");
            }
        }

        sb.append(" ]");

        return sb.toString();
    }

    
//    private int [] generateMembers(Connection [] procs) {
//
//        if (procs == null || procs.length == 0) {
//            Logging.error("generateMembers called for empty list!");
//            return new int[0];
//        }
//
//        int [] members = new int[procs.length];
//
//        for (int i=0;i<procs.length;i++) {
//            members[i] = procs[i].pid;
//        }
//
//        return members;
//    }

    // This implements the split operation. 
    private void processSplit() {

        // First gather all messages sharing a colors together in a list.
        HashMap<Integer, LinkedList<SplitRequest>> tmp = new HashMap<Integer, LinkedList<SplitRequest>>();

        for (int i=0;i<size;i++) {

            SplitRequest m = (SplitRequest) messages[i];

            Integer color = m.color;

            LinkedList<SplitRequest> l = tmp.get(color);

            if (l == null) {
                l = new LinkedList<SplitRequest>();
                tmp.put(color, l);
            }

            l.add(m);
        }

        // Next, sort each list on <preferred rank / existing rank> and send a reply. 
        // Note that the color -1 is reserved for machines that don't participate.
        for (Integer color : tmp.keySet()) {

            LinkedList<SplitRequest> l = tmp.get(color);

            if (l == null || l.isEmpty()) {
                Logging.error("Split created empty list!");
            } else {
                // Create a new communicator, provided the color >= 0 (color -1 is used for non-participating processes).
                int size = l.size();

                if (color >= 0) {

                    // We first sort the participants on their requested rank.
                    Collections.sort(l, colorComparator);

                    // Next, we gather all PIDs of the participating processes.
                    int [] pids = new int[size];
                    
                    int i=0;

                    for (SplitRequest m : l) {
                        pids[i++] = members[m.rank];
                    }

                    Logging.println("Creating split " + color + " from communicator " + communicator);
                    
                    // We generate a new 'virtual' communicator.
                    Communicator com = parent.createCommunicator(pids);

                    // Next, we send a reply to all participants of the new communicator, giving them the new communicator number,
                    // its size, and their new rank within the communicator. In addition, we need to send a color and rank for the
                    // split that needs to be performed on the 'real MPI communicator', and a flag and member set needed by the
                    // virtual communicator on the MPI side.

                    // Use a hash map to keep track of the desired local ranks in each of the clusters for each of the
                    // participants. These are needed to perform the local split.
//                    HashMap<String, Integer> localRanks = new HashMap<String, Integer>();

                    // Generate the flags needed by the virtual communicator.
                    int flags = generateFlags(pids);

                    int number = com.getNumber();

                    int [] coordinators = com.getCoordinatorRanks();
                    int [] clusterSizes = com.getClusterSizes();
                    int [] clusterRanks = com.getClusterRanks();
                    int [] members = com.getMembers();
                    int [] localRanks = com.getLocalRanks();
                    int [] memberClusterIndex = com.getMemberClusterIndex();

                    // Send a reply to each participant, generating the appropriate local rank for each participant.
                    for (int j=0;j<size;j++) {
                        enqueueReply(new SplitReply(pids[j], number, j, size, color, localRanks[j] /* is the key */, 
                                    coordinators.length, flags, coordinators, clusterSizes, members,
                                    clusterRanks, memberClusterIndex, localRanks));
                    }
                } else {
                    // We must also send a reply to all participants with color -1.
                    // As these will not actually create a new virtual communicator, we can send a simplified reply.
                    for (SplitRequest m : l) {
                        enqueueReply(new SplitReply(m.source));
                    }
                }
            }
        }
    }

    private void processGroup() {

        Logging.println("Creating new group from communicator " + communicator);

        int [] group = ((GroupRequest) messages[0]).pids;

        // Sanity check: all group messages should contain the same ranks array.
        for (int i=1;i<messages.length;i++) {
            if (!Arrays.equals(group, ((GroupRequest) messages[i]).pids)) {
                Logging.println("ERROR: collective group creation does not have matching parameters! "
                        + Arrays.toString(group) + " != " + Arrays.toString(((GroupRequest) messages[i]).pids));
                return; // FIXME: This return will hang the program!
            }
        }

        // We gather all connections to the participating machines, and save all connections
        // to the machines that do not participate.
        
        // Add all our members to a HashSet.
        HashSet<Integer> tmp = new HashSet<>();
        
        for (int i=0;i<members.length;i++) {
            tmp.add(members[i]);
        }
                
        // Next create a member array for the new communicator.        
        int [] pids = new int[group.length];
        
        // Then add each group member while removing it from the hash set.         
        for (int i=0;i<group.length;i++) {
            pids[i] = group[i]; 
            tmp.remove(group[i]);
        }

        // We can now create a new communicator using the new member set. 
        // Note that the hash set contains all our members that are not part of the new communicator.
        Communicator com = parent.createCommunicator(pids);

        // We now send a reply to all processes. Note that some may not participate in the new communicator.
        int number = com.getNumber();

        // Next, we send a reply to all participants, providing them with the new virtual communicator, its size,
        // and their new rank.

        // Generate the flags needed by the virtual communicator.
        int flags = generateFlags(pids);

        // Generate a correct members array for this cluster.
        int [] members = com.getMembers();
        int [] coordinators = com.getCoordinatorRanks();
        int [] clusterSizes = com.getClusterSizes();
        int [] clusterRanks = com.getClusterRanks();
        int [] localRanks = com.getLocalRanks();
        int [] memberClusterIndex = com.getMemberClusterIndex();

        // We need to figure out which cluster do and which don't participate. Those that don't do not need to create a local
        // communicator. Clusters that only participate partly, do need to create an new communicator.
        boolean [] activeClusters = new boolean[parent.getNumberOfClusters()];
        Arrays.fill(activeClusters, false);
        
        // Send a reply to each participant, generating the appropriate keys and bitmaps for each participant.
        for (int j=0;j<pids.length;j++) {

            // Get the connection and cluster name we are sending to
            int pid = pids[j];
            
            int destinationCluster = getClusterRank(pid);
            
            // Add the cluster to the set of participants.
            activeClusters[destinationCluster] = true;

            enqueueReply(new GroupReply(pid, number, j, members.length, coordinators.length, flags,
                        coordinators, clusterSizes, members, clusterRanks, memberClusterIndex, localRanks));
        }

        // Send a reply to each process that does not participate, as they may still need to perform a some local collectives.
        // We check the activeClusters array to see if part of a cluster is participating in the communicators. We store this
        // result in the GroupReply.
        for (int pid : tmp) {
            boolean participant = activeClusters[getClusterRank(pid)];
            enqueueReply(new GroupReply(pid, participant));
        }
    }

    private void processDup() {

        Logging.println("Performing DUP of communicator " + communicator);

        // We generate a new 'virtual' communicator.
        int number = parent.createCommunicator(members).getNumber();

        // Next, we send a reply to all participants, providing them with the new virtual communicator.
        for (int j=0;j<members.length;j++) {
            enqueueReply(new DupReply(members[j], communicator, number));            
        }
    }

    private void processFree() {

        Logging.println("Performing FREE of communicator " + communicator);

        // We generate a new 'virtual' communicator.
        parent.freeCommunicator(getNumber());
        
        // Next, we send a reply to all participants.
//        FreeReply reply = new FreeReply(communicator, number);
//
//        for (int j=0;j<processes.length;j++) {
//            processes[j].enqueue(reply, false);
//        }
//
//        commReplies += processes.length;
//        commReplyBytes += processes.length * reply.size();
    }

    private void processFinalize() throws Exception {
        
        if (communicator != 0) { 
            Logging.error("Cannot perform FINALIZE on communicator " + communicator);
            return;
        }

        Logging.println("Performing FINALIZE");

        parent.freeCommunicator(0);

        // We send a reply to all application processes instructing them to FINALIZE.
        for (int j=0;j<members.length;j++) {
            enqueueReply(new FinalizeReply(members[j]));
        }

        // We send a reply to all gateways instructing them to FINALIZE.
        for (int i=0;i<parent.getNumberOfClusters();i++) {

            Cluster c = parent.getCluster(i);            
            int size = c.getApplicationSize();
            
            // We send to non-master gateways first, since the master gateway needs to forward
            // these messages (and therefore remain active until all messages are forwarded).
            for (int j=1;j<parent.getNumberOfGatewaysPerCluster();j++) {
                int pid = parent.getPID(i, size+j-1);
                enqueueReply(new FinalizeReply(pid));
            }
            
            // Finally send to the master gateway.
            enqueueReply(new FinalizeReply(parent.getPID(i, size + parent.getNumberOfGatewaysPerCluster()-1)));
        }
    }
       
    public void terminate() {
        Logging.println("Terminating communicator " + communicator + ": " + printStatistics());
    }

    private void processRequest() throws Exception {

        // See what kind of operation we are intending to perform. 
        int opcode = messages[0].opcode;

        // Sanity check: make sure that all opcodes match
        for (int i=1;i<messages.length;i++) {
            if (messages[i].opcode != opcode) {
                Logging.error("Opcode mismatch in collective communicator operation! " + opcode + " != " + messages[1].opcode);
                return; 
            }
        }
        
        switch (opcode) {
        case Protocol.OPCODE_SPLIT:
            processSplit();
            break;

        case Protocol.OPCODE_GROUP:
            processGroup();
            break;

        case Protocol.OPCODE_DUP:
            processDup();
            break;

        case Protocol.OPCODE_FREE:
            processFree();
            break;

        case Protocol.OPCODE_FINALIZE:
            processFinalize();
            break;
//            terminate();
//            parent.terminateCommunicator(this);
//            break;
        default:
            Logging.error("Unknown opcode collective communicator operation! " + opcode);
            return; // FIXME: This return will hang the program!
        }
    }

//    private synchronized void process(Message m) {
//
//        // First check the message is legal
//        if (m.source < 0 || m.source >= size) {
//            Logging.error("Unknown rank " + m.source + " for operation on comm " + communicator);
//        }
//
//        commMessages++;
//        commBytes += m.size();
//
//        messages[m.source] = m;
//        participants++;
//
//        // If all participant are in, we perform the operation.
//        if (participants == size) {
//            processMessages();
//
//            // Reset administration.
//            participants = 0;
//
//            for (int i=0;i<size;i++) {
//                messages[i] = null;
//            }
//        }
//    }
//
//    private void deliver(DataMessage m) {
//
//        // Simply enqueue the message at the destination
//        if (m.dest > processes.length) {
//            Logging.error("Unable to deliver message to " + m.dest + " on comm " + communicator);
//            return;
//        }
//
//        processes[m.dest].enqueue(m, true);
//
//        dataMessages++;
//        dataBytes += m.size();
//    }
//
//    private void bcast(DataMessage m) {
//
//        // Enqueue the message at each of the cluster coordinators,
//        // but exclude the cluster of the root.
//
//        // FIXME: shouldn't we always use pids here?
//        Connection source = processes[m.dest]; // pids.get(m.dest);
//
//        if (source == null) {
//            Logging.println("ERROR: bcast target " + m.dest + " not found! -- DROPPING MESSAGE!!");
//            return;
//        }
//
//        for (int i=0;i<coordinatorRanks.length;i++) {
//
//            Connection c = processes[coordinatorRanks[i]];
//
//            if (c.clusterRank != source.clusterRank) {
////                Logging.println("Enqueuing BCAST at cluster coordinator " + printPID(c.pid) + " of comm " + communicator);
//                c.enqueue(m, true);
//            } else {
//  //              Logging.println("SKIP Enqueuing BCAST at cluster coordinator " + printPID(c.pid) + " of comm " + communicator);
//            }
//        }
//
//        bcastMessages++;
//        bcastBytes += m.size();
//    }

    public synchronized void deliverRequest(CommunicatorRequest req) {

        System.out.println("Got request " + req.opcode + " " + req.communicator + " " + req.rank);
        
        // First check the message is legal
        if (req.rank < 0 || req.rank >= size) {
            Logging.error("Unknown rank " + req.rank + " for operation on comm " + communicator);
            return;
        }

        // Make sure this is not a duplicate message
        if (messages[req.rank] != null) { 
            Logging.error("Duplicate operation message for rank " + req.rank + " on comm " + communicator);
            return;
        }
        
        // Next. update some stats.
        commMessages++;
        commBytes += req.length;

        // Store the message and increase the message count.
        messages[req.rank] = req;
        participants++;

        // If a message has been received for each communicator member, we are ready to perform the operation.
        if (participants == size) {            
            try { 
                processRequest();
            } catch (Exception e) { 
                Logging.error("Failed to process request!");
                e.printStackTrace(System.out);
            }

            // Reset administration.
            participants = 0;

            for (int i=0;i<size;i++) {
                messages[i] = null;
            }
        }
    }

    private String printStatistics() {

        StringBuilder sb = new StringBuilder("size: " + size);
//        sb.append(" data: " + dataMessages + " / " + dataBytes);
//        sb.append(" bcast: " + bcastMessages + " / " + bcastBytes);
        sb.append(" commIn: " + commMessages + " / " + commBytes);
        sb.append(" commOut: " + commReplies + " / " + commReplyBytes);
        return sb.toString();
    }
    
    public String printInfo() {
        return "COMM(" + communicator + ") = " + printPIDs(members);
    }

}
