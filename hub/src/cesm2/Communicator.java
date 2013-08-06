package cesm2;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collections;
import java.util.Comparator;
import java.util.HashMap;
import java.util.HashSet;
import java.util.LinkedList;

import cesm2.messages.SplitRequest;
import cesm2.messages.CommReply;
import cesm2.messages.DataMessage;
import cesm2.messages.DupReply;
import cesm2.messages.GroupMessage;
import cesm2.messages.GroupReply;
import cesm2.messages.ApplicationMessage;

public class Communicator {

    private final static boolean SANITY = true;

    private static final int COMM_FLAG_LOCAL  = (1 << 0);
    private static final int COMM_FLAG_REMOTE = (1 << 1);

    private final int communicator;
    private final Connection [] processes;
    private final int [] coordinatorRanks;
    private final int [] clusterSizes;
    
    private final HashMap<Integer, Connection> pids;
    
    private final int size;
    private final Server parent;

    private final ApplicationMessage [] messages;
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
        final String name;
        final int clusterRank;
        final int coordinatorRank;
        int size;
        
        public ClusterInfo(String name, int clusterRank, int coordinatorRank) {
            super();
            this.name = name;
            this.clusterRank = clusterRank;
            this.coordinatorRank = coordinatorRank;
            size = 1;
        }
 
        public void increaseSize() { 
            size++;
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
            if (o1.source < o2.source) {
                return -1;
            } else if (o1.source > o2.source) {
                return 1;
            }

            // Should never happen ?
            return 0;
        }
    }

    private final ColorComparator colorComparator = new ColorComparator();

    Communicator(Server parent, int communicator, Connection [] processes) {
        
        if (SANITY) { 
            // Make sure the processes array doesn't contain any holes!
            if (processes == null) { 
                Logging.println("ERROR: processes array null at communicator creation!");
                throw new IllegalArgumentException("Processes array null at communicator creation");
            }
           
            for (int i=0;i<processes.length;i++) { 
                if (processes[i] == null) { 
                    Logging.println("ERROR: processes array entry " + i + " null at communicator creation!");
                    throw new IllegalArgumentException("Processes array entry " + i + " null at communicator creation");
                }
            } 
        }
        
        this.communicator = communicator;
        this.processes = processes;
        this.parent = parent;
        this.size = processes.length;
        this.messages = new ApplicationMessage[size];

        pids = new HashMap<Integer, Connection>();
       
        ArrayList<ClusterInfo> info = new ArrayList<ClusterInfo>();
        HashMap<Integer, ClusterInfo> infoMap = new HashMap<Integer, ClusterInfo>();
        
        for (int i=0;i<size;i++) {
            int pid = processes[i].pid;
            pids.put(pid, processes[i]);
            
            int clusterRank = processes[i].clusterRank; 
            
            ClusterInfo tmp = infoMap.get(clusterRank);
            
            if (tmp == null) { 
                tmp = new ClusterInfo(processes[i].clusterName, clusterRank, i);
                info.add(tmp);
                infoMap.put(clusterRank, tmp);
            } else { 
                tmp.increaseSize();
            }
        }
      
        this.coordinatorRanks = new int[info.size()];
        this.clusterSizes = new int[info.size()];
        
        for (int i=0;i<info.size();i++) { 
            ClusterInfo tmp = info.get(i);
            this.coordinatorRanks[i] = tmp.coordinatorRank;
            this.clusterSizes[i] = tmp.size;
        }
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
    
    private int generateFlags(Connection [] procs) {

        if (procs == null || procs.length == 0) {
            Logging.error("generateFlags called for empty list!");
            return 0;
        }

        String name = procs[0].getClusterName();

        for (int i=1;i<procs.length;i++) {
            if (!name.equals(procs[i].getClusterName())) {
                return (COMM_FLAG_LOCAL | COMM_FLAG_REMOTE);
            }
        }

        return COMM_FLAG_LOCAL;
    }
    
    private int [] generateMembers(Connection [] procs) {

        if (procs == null || procs.length == 0) {
            Logging.error("generateMembers called for empty list!");
            return new int[0];
        }

        int [] members = new int[procs.length];

        for (int i=0;i<procs.length;i++) {
            members[i] = procs[i].pid;
        }

        return members;
    }   
    
    // This implements the split operation. Note that dup and create are just
    // special cases of split, and therefore handled by the same code.
    private void split() {

        // First gather all messages sharing a colors together in a list.
        HashMap<Integer, LinkedList<SplitRequest>> tmp =
                new HashMap<Integer, LinkedList<SplitRequest>>();

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

        // Next, sort each list on <preferred rank / existing rank> and send a
        // reply. Note that the color -1 is reserved for machines that don't
        // participate.
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

                    // Next, we gather all connections to the participating machines.
                    Connection [] procs = new Connection[size];

                    int i=0;

                    for (SplitRequest m : l) {
                        procs[i++] = processes[m.source];
                    }

                    // We generate a new 'virtual' communicator.
                    Communicator com = parent.createCommunicator(procs);

                    // Next, we send a reply to all participants, providing them with the new virtual communicator, its size,
                    // and their new rank. In addition, we need to send a color and rank for the split that needs to be performed
                    // on the 'real communicator', and a flag and member set needed by the virtual communicator on the MPI side.

                    // Use a hash map to keep track of the desired local ranks in each of the clusters for each of the 
                    // participants. These are needed to perform the local split.
                    HashMap<String, Integer> localRanks = new HashMap<String, Integer>();

                    // Generate the flags needed by the virtual communicator.
                    int flags = generateFlags(procs);

                    int number = com.getNumber();
                    
                    int [] coordinators = com.getCoordinatorRanks();
                
                    int [] clusterSizes = com.getClusterSizes();
                    
                    int [] members = generateMembers(procs);
                    
                    // Send a reply to each participant, generating the appropriate local rank for each participant.
                    for (int j=0;j<size;j++) {

                        // Get the connection and cluster name we are sending to
                        Connection c = procs[j];
                        String name = c.getClusterName();

                        // Generate a correct local for this specific cluster.
                        Integer key = localRanks.get(name);

                        if (key == null) {
                            key = 0;
                        }

                        localRanks.put(c.getClusterName(), key+1);
                      
                        CommReply reply = new CommReply(communicator, number, j, size, color, key, coordinators.length, flags, 
                                coordinators, clusterSizes, members);
                                                
                        // Send the reply.
                        c.enqueue(reply, false);
                        
                        commReplies++;
                        commReplyBytes += reply.size();
                    }
                } else {
                    // We must also send a reply to all participants with color -1.
                    // As these will not actually create a new virtual communicator,
                    // we can send a simplified reply.
                    for (SplitRequest m : l) {
                        CommReply reply = new CommReply(communicator, -1, -1, 0, -1, 0, 0, 0, null, null, null);                        
                        processes[m.source].enqueue(reply, false);
                        
                        commReplies++;
                        commReplyBytes += reply.size();
                    }
                }
            }
        }
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
    
    private void group() {

        Logging.println("Creating new group from communicator " + communicator);

        int [] group = ((GroupMessage) messages[0]).pids;

        if (SANITY) {
            // Sanity check: all group messages should contain the same ranks array.
            for (int i=1;i<messages.length;i++) {
                if (!Arrays.equals(group, ((GroupMessage) messages[i]).pids)) {
                    Logging.println("ERROR: collective group creation does not have matching parameters! "
                            + Arrays.toString(group) + " != " + Arrays.toString(((GroupMessage) messages[i]).pids));
                    return; // FIXME: This return will hang the program!
                }
            }
        }

        Logging.println("   processes(" + group.length + "): " + printPIDs(group));

        // We gather all connections to the participating machines, and save all connections
        // to the machines that do not participate.
        HashMap<Integer, Connection> tmp = new HashMap<Integer, Connection>();
        tmp.putAll(pids);
        
        Connection [] used = new Connection[group.length];

        for (int i=0;i<group.length;i++) {
            used[i] = tmp.remove(group[i]);
        }
        
        // We now send a reply to all processes. Note that some may not participate in the new communicator.

        // We generate a new 'virtual' communicator.
        Communicator com = parent.createCommunicator(used);

        int number = com.getNumber();
                     
        Logging.println("   new communicator: " + number);

        // Next, we send a reply to all participants, providing them with the new virtual communicator, its size,
        // and their new rank.

        // Generate the flags needed by the virtual communicator.
        int flags = generateFlags(used);

        Logging.println("   flags: " + flags);

        // Generate a correct members array for this cluster.
        int [] members = generateMembers(used);
        int [] coordinators = com.getCoordinatorRanks();
        int [] clusterSizes = com.getClusterSizes();
        
        Logging.println("   group reply: " + number + " " + flags + " " 
                + coordinators.length + " " + Arrays.toString(coordinators) 
                + " " + Arrays.toString(clusterSizes)
                + members.length + " " + flags + " " + printPIDs(members));
        
        // We need to figure out which cluster do and which don't participate. Those that don't do not need to create a local 
        // communicator.
        HashSet<String> participatingCluster = new HashSet<String>();
        
        // Send a reply to each participant, generating the appropriate keys and bitmaps for each participant.
        for (int j=0;j<used.length;j++) {        
            
            // Get the connection and cluster name we are sending to
            Connection c = used[j];
            String name = c.getClusterName();
        
            // Add the cluster to the set of participants.
            participatingCluster.add(name);
            
            Logging.println("        sending group info to " + j + " " + printPID(c.pid) + " at " + name);
            
            GroupReply reply = new GroupReply(communicator, number, j, members.length, coordinators.length, flags, 
                    coordinators, clusterSizes, members); 
            
            // Create and send the reply.
            c.enqueue(reply, false);
            
            commReplies++;
            commReplyBytes += reply.size();
        }

        // Send a reply to each process that does not participate, as they may still need to perform a some local collectives.
        // We check in bitmaps to see if (part of) a cluster is participating in the communicators. We store this result in
        // the size field of the GroupReply
        int j=0;
        
        for (Connection c : tmp.values()) {
            if (c != null) {
                String name = c.getClusterName();
                boolean participant = participatingCluster.contains(name);
            
                Logging.println("        sending participant info to " + j++ + " " + printPID(c.pid) + " at " + name 
                        + "(" + participant + ")");
                
                
                GroupReply reply = new GroupReply(communicator, participant);
                
                c.enqueue(reply, false);

                commReplies++;
                commReplyBytes += reply.size();
            }
        }
    }

    private void dup() {

        Logging.println("Creating dup of communicator " + communicator);

        // We generate a new 'virtual' communicator.
        int number = parent.createCommunicator(processes).getNumber();

        Logging.println("   dup communicator: " + communicator + " -> " + number);

        // Next, we send a reply to all participants, providing them with the new virtual communicator.
        DupReply reply = new DupReply(communicator, number);

        for (int j=0;j<processes.length;j++) {
            processes[j].enqueue(reply, false);
        }
        
        commReplies += processes.length;
        commReplyBytes += processes.length * reply.size();
    }

    public void terminate() {
        Logging.println("Terminating communicator " + communicator + ": " + printStatistics());
    }
    
    private void processMessages() {

        int opcode = messages[0].opcode;

        if (SANITY) {
            // Sanity check: see if all opcodes match
            for (int i=1;i<messages.length;i++) {
                if (messages[i].opcode != opcode) {
                    Logging.error("opcode mismatch in collective communicator operation! " + opcode + " != " + messages[1].opcode);
                    return; //  FIXME: This return will hang the program!
                }
            }
        }

        switch (opcode) {
        case Protocol.OPCODE_COMM:
            split();
            break;

        case Protocol.OPCODE_GROUP:
            group();
            break;

        case Protocol.OPCODE_DUP:
            dup();
            break;

        case Protocol.OPCODE_TERMINATE:
            terminate();
            parent.terminateCommunicator(this);        
            break;

        
        default:
            Logging.error("unknown opcode collective communicator operation! " + opcode);
            return; // FIXME: This return will hang the program!
        }
    }

    private synchronized void process(ApplicationMessage m) {

        // First check the message is legal
        if (m.source < 0 || m.source >= size) {
            Logging.error("Unknown rank " + m.source + " for operation on comm " + communicator);
        }

        commMessages++;
        commBytes += m.size();
        
        messages[m.source] = m;
        participants++;

        // If all participant are in, we perform the operation.
        if (participants == size) {
            processMessages();

            // Reset administration.
            participants = 0;

            for (int i=0;i<size;i++) {
                messages[i] = null;
            }
        }
    }

    private void deliver(DataMessage m) {
        
        // Simply enqueue the message at the destination
        if (m.dest > processes.length) {
            Logging.error("Unable to deliver message to " + m.dest + " on comm " + communicator);
            return;
        }

        processes[m.dest].enqueue(m, true);
        
        dataMessages++;
        dataBytes += m.size();        
    }

    private void bcast(DataMessage m) {
        
        // Enqueue the message at each of the cluster coordinators, 
        // but exclude the cluster of the root.
        
        // FIXME: shouldn't we always use pids here?
        Connection source = processes[m.dest]; // pids.get(m.dest);
        
        if (source == null) {
            Logging.println("ERROR: bcast target " + m.dest + " not found! -- DROPPING MESSAGE!!");
            return;
        }
        
        for (int i=0;i<coordinatorRanks.length;i++) {
            
            Connection c = processes[coordinatorRanks[i]];
            
            if (c.clusterRank != source.clusterRank) { 
                Logging.println("Enqueuing BCAST at cluster coordinator " + printPID(c.pid) + " of comm " + communicator);
                c.enqueue(m, true); 
            } else { 
                Logging.println("SKIP Enqueuing BCAST at cluster coordinator " + printPID(c.pid) + " of comm " + communicator);
            }
        }
        
        bcastMessages++;
        bcastBytes += m.size();        
    }
    
    public void deliver(ApplicationMessage m) {

        switch (m.opcode) {
        case Protocol.OPCODE_COMM:
        case Protocol.OPCODE_GROUP:
        case Protocol.OPCODE_DUP:
        case Protocol.OPCODE_TERMINATE:                        
            process(m);
            break;
        case Protocol.OPCODE_DATA:
            deliver((DataMessage)m);
            break; 
        case Protocol.OPCODE_COLLECTIVE_BCAST:
            bcast((DataMessage)m);
            break;
        default:
            Logging.error("unknown message type " + m.opcode);
        }
    }
    
    private String printStatistics() {         

        StringBuilder sb = new StringBuilder("size: " + size);
        sb.append(" data: " + dataMessages + " / " + dataBytes);
        sb.append(" bcast: " + bcastMessages + " / " + bcastBytes);
        sb.append(" commIn: " + commMessages + " / " + commBytes);
        sb.append(" commOut: " + commReplies + " / " + commReplyBytes);        
        return sb.toString();
    }
}
