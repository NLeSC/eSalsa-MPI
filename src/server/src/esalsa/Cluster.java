package esalsa;

import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import java.net.InetAddress;
import java.net.Socket;
import java.util.Arrays;
import java.util.LinkedList;

public class Cluster {

    private static class GatewayInfo {

        private final String name;
        private final int rank;
        private final short port;
        private final short streams;
        private final byte [] address;

        public GatewayInfo(String name, int rank, short port, short streams, byte [] address) {
            this.name = name;
            this.rank = rank;
            this.port = port;
            this.streams = streams;
            this.address = address;
        }

        public String getName() {
            return name;
        }
        
        public int getRank() {
            return rank;
        }

        public int getPort() {
            return port;
        }
        
        public short getStreams() {
            return streams;
        }
        
        public byte [] getAddres() {
            return address;
        }
    }

    class SenderThread extends Thread {
        public void run() {
            try {
                boolean more = true;

                while (more) {
                    more = sendMessage();
                }
                
                closeSocket();
            } catch (Exception e) {
                e.printStackTrace(System.err);
            }
        }
    }

    class ReceiverThread extends Thread {
        public void run() {
            try {
                boolean more = true;

                while (more) {
                    more = receiveMessage();
                }
            } catch (Exception e) {
                e.printStackTrace(System.err);
            }
        }
    }

    public static final int MAX_LENGTH_CLUSTER_NAME = 128;

    /** Server to which this cluster belongs. */
    private Server owner;

    /** Unique name of this cluster. */ 
    private final String name;

    /** Rank of this cluster. */
    private final int clusterRank;           

    /** Number of application processes in this cluster. */
    private final int applicationProcesses;  

    /** 
     *  Base port in this cluster. Each gateway in this cluster
     *  should be reachable at port <code>basePort + gatewayRank</code> 
     */
    private final int basePort; 

    /** Information on all gateways available in this cluster */
    private GatewayInfo [] gateways;

    /** Socket connected to the master gateway in this cluster. */
    private Socket socket; 

    /** DataInputStream used to read data from the master gateway. */
    private DataInputStream in;

    /** DataOutputStream used to write data to the master gateway. */
    private DataOutputStream out;

    /** Is this cluster ready ? (has the initialization completed ?) */
    private boolean ready = false;

    /** Is this cluster done ? (has the application terminated ?) */
    private boolean done = false;

    /** Message queue, used to store messages waiting to be send. */
    private final LinkedList<Message> incoming = new LinkedList<Message>();

    /** Number of messages received. */
    private long messagesReceived = 0;

    /** Number of messages sent. */
    private long messagesSent = 0;

    /** Preferred network range */
    private byte [] network;
    
    /** Network mask to use */
    private byte [] netmask;
    
    /** 
     * Constructor for Cluster.
     *  
     * @param owner
     *          Server to which this cluster belongs.
     * @param name
     *          Unique name of this cluster.
     * @param applicationProcesses
     *          Number of application processes in this cluster.
     * @param basePort
     *          Base port for socket connections to gateways of this cluster.
     * @param clusterRank
     *          Rank of this cluster in total set of clusters.
     * @param numberOfGateways
     *          Number of gateways in this cluster.
     * @param cidr
     *          Network to use on this gateway in CIDR format, e.g., "192.168.1.0/24" 
     */
    public Cluster(Server owner, String name, int applicationProcesses, int basePort, int clusterRank, int numberOfGateways, 
            byte [] network, byte [] netmask) {
        this.owner = owner;
        this.name = name;
        this.applicationProcesses = applicationProcesses;
        this.clusterRank = clusterRank;
        this.basePort = basePort;
        this.gateways = new GatewayInfo[numberOfGateways];    
        this.network = network;
        this.netmask = netmask;
    }

    /**
     * Returns the rank of this cluster.
     * 
     * @return
     *          the rank of this cluster.
     */
    public int getRank() {
        return clusterRank;
    }

    /**
     * Returns the number of application processes in this cluster.
     * 
     * @returnGatewayInfo info = gateways[index];


     *          the number of application processes in this cluster.
     */
    public int getApplicationSize() {
        return applicationProcesses;
    }

    /**
     * Returns the name of this cluster. 
     * 
     * @return
     *          the name of this cluster. 
     */
    public String getName() {
        return name;
    }

    /**
     * Set the connection of this cluster consisting of a {@link Socket} connected to the master gateway and its associated  
     * {@link DataInputStream} and {@link DataOutputStream}.    
     * 
     * @param socket
     *          The {@link Socket} connected to the master gateway of this cluster. 
     * @param in
     *          The {@link DataInputStream} associated with the socket.
     * @param out
     *          The {@link DataOutputStream} associated with the socket.
     * @throws Exception
     *          If the connection was set.  
     */
    public synchronized void setConnection(Socket socket, DataInputStream in, DataOutputStream out) throws Exception {

        if (this.socket != null) { 
            throw new Exception("Cluster " + name + " is already connected!");
        }

        this.socket = socket;
        this.in = in;
        this.out = out;

        Logging.println("Cluster " + name + " connected to gateway at " + socket);
    }

    /**
     * Send the initial handshake reply to the master gateway.
     * 
     * This initial handshake reply writes the cluster rank, number of clusters, total application processes, number of gateways 
     * per cluster and size of each cluster to the {@link DataOutputStream}.
     * 
     * @throws IOException
     *          If the handshake reply could not be written.
     */
    private void sendHandshakeReply() throws IOException {

        out.writeInt(Protocol.OPCODE_HANDSHAKE_ACCEPTED);
        out.writeInt(clusterRank);
        out.writeInt(owner.getNumberOfClusters());
        out.writeInt(applicationProcesses);
        out.writeInt(owner.getNumberOfGatewaysPerCluster());

        for (int i=0;i<owner.getNumberOfClusters();i++) {
            out.writeInt(owner.getCluster(i).getApplicationSize());
        }

        out.flush();
    }

    /**
     * Apply a mask to a byte array.
     * 
     * @param in
     *          The input byte array.
     * @param mask
     *          The mask to apply.
     * @return
     *          A new array that contains <code>in & mask</code>.
     */
    private byte [] applyMask(byte [] in, byte [] mask) {
        
        byte [] result = new byte [in.length];
        
        for (int i=0;i<in.length;i++) {
            result[i] = (byte) ((in[i] & mask[i]) & 0xFF);
        }
        
        return result;
    }
    
    /**
     * Test if a given address matches the provided subnet and netmask.
     * 
     * @param address
     *          The address to test.
     * @param subnet
     *          The subnet to test for.
     * @param netmask
     *          The netmask to apply to the subnet.
     * @return
     *          if the address matches the subnet and netmask.
     * @throws Exception 
     *          thrown if the length of the address does not match the length of the subnet or mask. 
     */
    private boolean match(byte [] address, byte [] subnet, byte [] netmask) throws Exception {
        
        if (address.length != subnet.length || address.length != netmask.length) { 
            throw new Exception("Length mismatch in address!");
        }
        
        byte [] tmp1 = applyMask(address, netmask);
        byte [] tmp2 = applyMask(subnet, netmask);
        
        return Arrays.equals(tmp1, tmp2);
    }
    
    /**
     * Select a single one of the addresses available at the provided gateway unsing the provided subnet and netmask information.  
     * 
     * @param info
     *          The gateway from which to select the address.
     * @return
     *          The selected addresses. 
     * @throws Exception
     *          throw if no matching address can be found.  
     */
    private byte [] selectAddress(InetAddress [] addresses) throws Exception {

        for (int i=0;i<addresses.length;i++) {

            byte [] tmp = addresses[i].getAddress();
            
            if (match(tmp, network, netmask)) {
                return tmp;
            }
        }
        
        throw new Exception("No address found that matches subnet and netmask provided ! (" + printNetwork() + ", " 
                + Arrays.toString(addresses) + ")");
    }
    
    /**
     * Receive the connection information from gateway <code>index</code> in the cluster.
     * 
     * This connection information consists of the cluster rank, the gateway rank, the number of IP addresses found at the
     * gateway, and each IP address in binary form.  
     * 
     * @param index
     *          The rank of the gateway (between 0 and numberOfGateways).
     * @throws Exception
     *          If the connection information could not be read.
     */
    private void receiveGatewayInfo(int index) throws Exception {

        Logging.println("Receiving gateway info for gateway " + name + "/" + index);

        // Read the incoming handshake.
        int opcode = in.readInt();

        if (opcode != Protocol.OPCODE_GATEWAY_INFO) {
            throw new Exception("Unexpected opcode " + opcode);
        }

        int clusterRank = in.readInt();

        if (clusterRank != this.clusterRank) { 
            throw new Exception("Invalid cluster rank " + clusterRank + " != " + this.clusterRank);
        }

        int gatewayRank = in.readInt();

        if (gatewayRank != index) { 
            throw new Exception("Unexpected gateway rank " + gatewayRank + " != " + index);
        }

        int ipv4Count = in.readInt();

        if (ipv4Count <= 0 ) { 
            throw new Exception("Invalid number of IP addresses for gateway " + index + " (" + ipv4Count + ")");
        }

        InetAddress [] addresses = new InetAddress[ipv4Count];

        for (int i=0;i<ipv4Count;i++) {
            byte [] tmp = new byte[4];

            in.readFully(tmp);

            // TODO: should select one here!!!
            addresses[i] = InetAddress.getByAddress(tmp);

            Logging.println("  Available IP address: " + addresses[i]);
        }

        byte [] address = selectAddress(addresses);
        
        setGatewayInfo(new GatewayInfo(name, index, (short) (basePort + index * owner.getNumberOfStreams()), 
                (short) owner.getNumberOfStreams(), address));
    }

    private synchronized void setGatewayInfo(GatewayInfo info) {
        gateways[info.getRank()] = info;
        notifyAll();
    }

    private synchronized GatewayInfo getGatewayInfo(int rank) {
        
        while (gateways[rank] == null) { 
            try { 
                wait();
            } catch (InterruptedException e) {
                // ignored
            }
        }
        
        return gateways[rank];
    }
    
    /**
     * Read the gateway information of all gateways, in order of their rank.
     * 
     * @throws Exception
     *          If the gateway info of one gateway could not be read.
     */
    private void receiveAllGatewayInfo() throws Exception {
        for (int i=0;i<gateways.length;i++) { 
            receiveGatewayInfo(i);
        }
    }

   

    /**
     * Send the connection information for one gateway to the master gateway.  
     * 
     * The connection information consists of a single IP address and port number, 
     * and the number of streams to use to connect to this gateway.  
     *  
     * @param index
     *          The index of the gateway to send,
     * @throws Exception
     */
    private void sendGatewayInfo(GatewayInfo info) throws Exception {

        Logging.println("Sending gateway info for gateway " + info.getName() + "/" + info.getRank());

        out.write(info.getAddres());
        out.writeShort(info.getPort());
        out.writeShort(info.getStreams());
    }


    private void sendAllGatewayInfo() throws Exception {
        
        for (int i=0;i<owner.getNumberOfClusters();i++) { 
            
            Cluster c = owner.getCluster(i);
            
            for (int j=0;j<owner.getNumberOfGatewaysPerCluster();j++) {
                
                GatewayInfo info = c.getGatewayInfo(j);
                sendGatewayInfo(info);
            }
        }
        
        out.flush();
    }


    public void performHandshake() throws Exception {

        sendHandshakeReply();
        receiveAllGatewayInfo();
        sendAllGatewayInfo();

        synchronized (this) {
            ready = true;
        }
    }

    public synchronized boolean isReady() {
        return ready;
    }

    private void done() {
        synchronized (incoming) {
            done = true;
            incoming.notifyAll();
        }
        
        owner.clusterDone();
    }

    public void enqueue(Message m) {
        synchronized (incoming) {
            incoming.addLast(m);
            incoming.notifyAll();
        }
    }

    private Message dequeue() {
        synchronized (incoming) {
            while (!done && incoming.size() == 0) {
                try {
                    incoming.wait();
                } catch (InterruptedException e) {
                    // ignored
                }
            }

            // Note: Even if we are done we'll keep returning messages until the queue is empty.
            if (incoming.size() == 0 && done) {               
                Logging.println("Cluster " + name + " is done!");                
                return null;
            } 
            
            return incoming.removeFirst();
        }
    }

    private boolean sendMessage() throws Exception {

        Message m = dequeue();

        if (m == null) {
            return false;
        }

	//        Logging.println("Sending message to Cluster " + name + " with opcode " + m.opcode);
        m.write(out);
        out.flush();
        messagesSent++;        
        return true;
    }

    private boolean receiveMessage() throws Exception {

        int opcode = in.readInt();

        CommunicatorRequest req = null;

//        Logging.println("Cluster " + name + " received opcode " + opcode);

        switch (opcode) {

        case Protocol.OPCODE_SPLIT:
//            Logging.println("Cluster " + name + " - Reading COMM message.");
            req = new SplitRequest(in);
            break;

        case Protocol.OPCODE_GROUP:
//            Logging.println("Cluster " + name +" - Reading GROUP message.");
            req = new GroupRequest(in);
            break;

        case Protocol.OPCODE_DUP:
//            Logging.println("Cluster " + name + " - Reading DUP message.");
            req = new DupRequest(in);
            break;

        case Protocol.OPCODE_FREE:
//            Logging.println("Cluster " + name + " - Reading FREE message.");
            req = new FreeRequest(in);
            break;

        case Protocol.OPCODE_FINALIZE:
//            Logging.println("Cluster " + name + " - Reading FINALIZE message.");
            req = new FinalizeRequest(in);
            break;
        case Protocol.OPCODE_CLOSE_LINK:
//            Logging.println("Cluster " + name + " - Closing link.");
            done();           
            return false;

        default:
            Logging.println("Cluster " + name + " sent illegal opcode " + opcode);
            throw new Exception("Illegal opcode " + opcode + " sent by cluster " + name);
        }

        messagesReceived++;

//        Logging.println("Cluster " + name + " - Deliver request!");

        owner.deliverRequest(req);
        return true;
    }


    /**
     *
     */
    public void startMessaging() {
        // Start the sending and receiving threads for this cluster.
        new SenderThread().start();
        new ReceiverThread().start();
    }

    /**
     * @return
     */
    public int getBasePort() {
        return basePort;
    }

    void closeSocket() {
        // Wait until all clusters are done!
        owner.allClustersDone();

        try {
            out.writeInt(Protocol.OPCODE_CLOSE_LINK);
            out.flush();
        } catch (Exception e) {
            Logging.error("Failed write OPCODE_FINALIZE_REPLY to cluster " + name);
        }

        try {
            in.close();
        } catch (Exception e) {
            Logging.error("Failed to close socket input from cluster " + name);
        }

        try {
            out.close();
        } catch (Exception e) {
            Logging.error("Failed to close socket output to cluster " + name);
        }

        try {
            socket.close();
        } catch (Exception e) {
            Logging.error("Failed to close socket connection to cluster " + name);
        }
    }

    private void printNetwork(byte [] network, StringBuilder target) {

        for (int i=0;i<network.length;i++) {
            target.append(((int) network[i]) & 0xff);

            if (i != network.length-1) {
                target.append(".");
            }
        }
    }

    /**
     * @return
     */
    public String printNetwork() {

        StringBuilder result = new StringBuilder();

        printNetwork(network, result);
        result.append("/");
        printNetwork(netmask, result);
        return result.toString();
    }
}
