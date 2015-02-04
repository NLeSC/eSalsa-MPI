package esalsa;

import java.io.BufferedInputStream;
import java.io.BufferedOutputStream;
import java.io.BufferedReader;
import java.io.File;
import java.io.FileReader;
import java.io.IOException;
import java.net.ServerSocket;
import java.net.Socket;
import java.net.SocketTimeoutException;
import java.util.ArrayList;
import java.util.StringTokenizer;

public class Server {

    public static final int MAX_CLUSTERS = 255; // 0xFF
    public static final int MAX_PROCESSES_PER_CLUSTER = 16777215; // OxFFFFFF
    public static final int MAX_LENGTH_CLUSTER_NAME = 128;
    
    class AcceptThread extends Thread { 

        private final Socket s;
        
        AcceptThread(Socket s) throws IOException { 
            this.s = s;
        }
            
        public void run() {
            try { 
                connectionHandshake(s);
            } catch (Exception e) {
                Logging.error("Failed to initiate new connection!" + e);
                e.printStackTrace(System.err);
            }
        }
    }

    private final String experiment;    
    
    private final int port;

    private final int numberOfClusters;
    private final int numberOfApplicationProcesses;
    private final int numberOfGatewaysPerCluster;
    private final int numberOfStreamsPerGatewayConnection;
    
    private final int protocol;
    
    private final int messageSize;
    private final int messageBufferSize;
    
    private final Cluster [] clusters;
    
    private int clustersDone = 0;
    
    private boolean done = false;
    
    private ArrayList<Communicator> communicators = new ArrayList<Communicator>();

    public Server(File file) throws Exception {
        
        BufferedReader r = new BufferedReader(new FileReader(file));

        experiment = readline(r);

        port = Integer.parseInt(readline(r));
        
        if (port <= 1000 || port >= 65535) { 
            throw new Exception("Server port out of bounds! (" + port + ")");
        }
        
        numberOfClusters = Integer.parseInt(readline(r));
        
        if (numberOfClusters <= 1 || numberOfClusters >= MAX_CLUSTERS) { 
            throw new Exception("Number of clusters out of bounds! (" + numberOfClusters + ")");
        }
        
        numberOfGatewaysPerCluster = Integer.parseInt(readline(r));

        // TODO: max is arbitrary ?
        if (numberOfGatewaysPerCluster < 1) { 
            throw new Exception("Number of gateways per clusters out of bounds! (" + numberOfGatewaysPerCluster + ")");
        }
        
        numberOfStreamsPerGatewayConnection = Integer.parseInt(readline(r));

        // TODO: max is arbitrary ?
        if (numberOfStreamsPerGatewayConnection < 1) { 
            throw new Exception("Number of streams per gateway connection out of bounds! (" + numberOfStreamsPerGatewayConnection + ")");
        }
        
        messageSize = Integer.parseInt(readline(r));

        if (messageSize < 64) { 
            throw new Exception("Message size too small (" + messageSize + ")! Must be >= 64.");
        }

        messageBufferSize = Integer.parseInt(readline(r));

        if (messageBufferSize < 1) { 
            throw new Exception("MessageBuffer size too small (" + messageBufferSize + ")!. Must be > 0.");
        }
        
        String prot = readline(r);
        
        if (prot.equalsIgnoreCase("TCP")) { 
            protocol = 0;            
        } else if (prot.equalsIgnoreCase("UDT")) {             
            protocol = 1;                        
        } else { 
            throw new Exception("Unknown WA protocol " + prot);
        }

        clusters = new Cluster[numberOfClusters];

        int tmp = 0;
        
        for (int i=0;i<numberOfClusters;i++) { 
            clusters[i] = readCluster(r, i);
            tmp += clusters[i].getApplicationSize();
        }
        
        numberOfApplicationProcesses = tmp;
        
        Logging.println("Starting eSalsa MPI server for experiment \"" + experiment + "\"");
        Logging.println("   Clusters                 : " + numberOfClusters);
        Logging.println("   Gateways/clusters        : " + numberOfGatewaysPerCluster);
        Logging.println("   Application processes    : " + numberOfApplicationProcesses);
        Logging.println("   Total processes          : " 
                + (numberOfApplicationProcesses + numberOfClusters * numberOfGatewaysPerCluster));
        Logging.println("   Parallel streams         : " + numberOfStreamsPerGatewayConnection);
        Logging.println("   Message size             : " + messageSize);
        Logging.println("   Message Buffer size      : " + messageBufferSize);
        Logging.println("   Wide area protocol       : " + (protocol == 0 ? "TCP" : "UDT"));
        Logging.println("   Server listening on port : " + port);
        
        for (int i=0;i<numberOfClusters;i++) { 
            Logging.println("   --------------------------");
            Logging.println("   Cluster " + i + " name           : \"" + clusters[i].getName() + "\"");
            Logging.println("      Application processes : " + clusters[i].getApplicationSize());
            Logging.println("      Port range in use     : " + clusters[i].getBasePort() + " ... " 
                    + (clusters[i].getBasePort() + (numberOfGatewaysPerCluster * numberOfStreamsPerGatewayConnection)));
            Logging.println("      Network to use        : " + clusters[i].printNetwork());
        }
        
        Logging.println("");
    }

    private String readline(BufferedReader r) throws IOException {
        while (true) {
            String tmp = r.readLine();

            if (tmp == null) { 
                throw new IOException("Unexpected end-of-file!");
            }

            tmp = tmp.trim();

            if (!tmp.isEmpty() && !tmp.startsWith("#")) {
                return tmp;
            }
        }
    }

    private byte [] readNetwork(String ipv4) throws Exception {
        
        byte [] result = new byte[4];
        
        StringTokenizer tok = new StringTokenizer(ipv4.trim(), ".");
        
        if (tok.countTokens() != 4) { 
            throw new Exception("Invalid IPv4 address: " + ipv4);
        }
        
        for (int i=0;i<4;i++) { 
            int tmp = Integer.parseInt(tok.nextToken());
            
            if (tmp < 0 || tmp > 255) { 
                throw new Exception("Invalid IPv4 address: " + ipv4);
            }
            
            result[i] = (byte) (tmp & 0xFF);            
        }
        
        return result;
    }
    
    private byte [] generateNetmask(String numBits) throws Exception {
        
        int bits = Integer.parseInt(numBits);
        
        byte [] result = new byte[4];
        
        int index = 0;
        
        while (bits >= 8) {
            result[index] = (byte) 0xFF;
            index++;
            bits -= 8;
        }

        byte tmp = (byte) (1 << 8);
        
        while (bits > 0) {
            result[index] |= tmp;
            tmp = (byte) (tmp >> 1);
            bits--;
        }
        
        return result;
    }
    
    private Cluster readCluster(BufferedReader r, int index) throws Exception {

        String name = readline(r);
        int size = Integer.parseInt(readline(r));

        if (size <= 0 || size > MAX_PROCESSES_PER_CLUSTER) {
            throw new Exception("Invalid number of processes in cluster " + index + ": " + size);
        }

        int port = Integer.parseInt(readline(r)); 

        if (port <= 1000 || port > 65535) {
            // TODO: take gateway count into account!
            throw new Exception("Invalid base port in cluster " + index + ": " + port);
        }
        
        String cidr = readline(r).trim(); 
        
        int slashIndex = cidr.indexOf("/");
        
        if (slashIndex < 0) { 
            throw new Exception("Invalid CIDR address: " + cidr);
        }
        
        byte [] network = readNetwork(cidr.substring(0, slashIndex));
        byte [] netmask = generateNetmask(cidr.substring(slashIndex+1));
        
        return new Cluster(this, name, size, port, index, numberOfGatewaysPerCluster, network, netmask, messageSize);
    }
 
    private void connectionHandshake(Socket s) throws Exception {
        
        s.setTcpNoDelay(true);

        BufferedInputStream in = new BufferedInputStream(s.getInputStream());
        BufferedOutputStream out = new BufferedOutputStream(s.getOutputStream());
        
        // Read the incoming handshake. We read an integer as 4 separate bytes here to determine the endianness of the machine
        // connecting to us....
        
        byte [] opcode = new byte[4];
        
        int pos = 0; 
        int length = 4;
        int read;
        
        while (pos != length) { 
            read = in.read(opcode, pos, length-pos);
        
            if (read < 0) { 
                throw new Exception("Unexpected EOF while reading opcode");
            }
            
            pos += read;
        }

        EndianDataInputStream din;
        EndianDataOutputStream dout;
        boolean littleEndian;
        
        if (opcode[0] == 42) {
            // Little endian machine on the other side!
            din = new LittleEndianDataInputStream(in);
            dout = new LittleEndianDataOutputStream(out);
            littleEndian = true;
            Logging.println("Got connection from " + s + " (little endian)");
        } else if (opcode[3] == 42) { 
            // Big endian machine on the other side!
            din = new BigEndianDataInputStream(in);
            dout = new BigEndianDataOutputStream(out);
            littleEndian = false;
            Logging.println("Got connection from " + s + " (big endian)");
        } else { 
            Logging.println("Rejecting connection from " + s + " (UNKNOWN OPCODE)");
            throw new Exception("Unexpected opcode " + opcode);
        }

        int len = din.readInt();
        
        if (len <= 0 || len > MAX_LENGTH_CLUSTER_NAME) {
            throw new Exception("Illegal cluster name length " + len);
        }
        
        byte [] tmp = new byte[len];
        
        din.readFully(tmp);
        
        String name = new String(tmp);
        
        Cluster c = getCluster(name);
        
        if (c == null) { 
            // unknown cluster!
            dout.writeInt(Protocol.OPCODE_HANDSHAKE_REJECTED);
            dout.close();
            throw new Exception("Unknown cluster: " + name);
        }
        
        c.performHandshake(s, din, dout, littleEndian);        
        // From this moment on, the cluster gateways should be completely initialized.       
    }
    
    private synchronized Cluster getCluster(String name) {
        
        for (int i=0;i<clusters.length;i++) {
            if (clusters[i].getName().equals(name)) {
                return clusters[i];
            }
        }     
        
        return null;
    }
        
    protected void deliverRequest(CommunicatorRequest req) {

        Communicator c = null;

        synchronized (this) {
            if (req.communicator >= communicators.size()) {
                Logging.error("Cannot deliver message, unknown communicator " + req.communicator);
                return;
            }

            c = communicators.get(req.communicator);
        }

        if (c == null) {
            Logging.error("Cannot deliver message, unknown communicator " + req.communicator);
            return;
        }

        c.deliverRequest(req);
    }

    protected void enqueueReply(int destinationPID, ServerMessage m) throws IOException {

        int destinationCluster = Communicator.getClusterRank(destinationPID);
        //parent.enqueueReply(destinationCluster, m);

        // Sanity check
        if (destinationCluster < 0 || destinationCluster >= clusters.length) {
            Logging.error("Cannot enqueue message, unknown cluster " + destinationCluster);
            new Exception().printStackTrace(System.out);
            return;
        }
        
        clusters[destinationCluster].enqueue(destinationPID, m);
    }
    
    protected Communicator createCommunicator(int [] members) {

        Communicator comm;

        synchronized (this) {
            int c = communicators.size();
            comm = new Communicator(this, c, members);
            communicators.add(c, comm);
        }

        Logging.println("Created new communicator: " + comm.printInfo());
        return comm;
    }

    protected void freeCommunicator(int number) {

        synchronized (this) {
            
            Communicator c = communicators.get(number);
            
            if (c == null) {
                Logging.error("Cannot free communicator "  + number + " as it does not exist!");
                return;
            }

            communicators.set(number, null);
        }

        Logging.println("Freed communicator: " + number);
    }

    
    /*
    protected Communicator createCommunicator(Connection [] processes) {
        Communicator comm;
        
        synchronized (this) {
            int c = communicators.size();
            comm = new Communicator(this, c, processes);
            communicators.add(c, comm);
        }

        Logging.println("Created new communicator: " + comm.printInfo());
        return comm;
    }

    protected void terminateCommunicator(Communicator c) {
        
        if (c.getNumber() == 0) { 
            // Terminating MPI_COMM_WORLD indicates the end of the application!

            synchronized (this) {               
                for (Communicator tmp : communicators) { 
                    if (tmp != null) {
                        tmp.terminate();
                    } 
                } 
            
                communicators.clear();
                done();
            }
        } else {
            // A single communicator is terminated (
            synchronized (this) {
                c.terminate();                
                communicators.set(c.getNumber(), null);
            }
        }
    }
    
    
    synchronized int [] waitUntilSignupComplete() {

        while (!signupComplete) {
            try {
                wait();
            } catch (InterruptedException e) {
                // ignored
            }
        }

        return clusterSizes;
    }

    Cluster getCluster(int clusterRank, int clusterSize, int clusterCount, String clusterName) throws Exception {

        if (this.numberOfClusters != clusterCount) {
            throw new Exception("Number of clusters does not match (" + clusterCount + " != " + this.numberOfClusters + ")");
        }

        Cluster tmp;

        synchronized (clusters) {
            if (clusters[clusterRank] == null) {
                Logging.println("Added cluster " + clusterName + " (" + clusterRank
                        + " of " + clusterCount + ") of size " + clusterSize);
                clusters[clusterRank] = new Cluster(clusterName, clusterSize, clusterRank);
            }

            tmp = clusters[clusterRank];
        }

        return tmp;
    }
*/
    private boolean signupComplete() {

        // All clusters must be ready! 
        for (int i=0;i<clusters.length;i++) {
            if (!clusters[i].isReady()) {
                return false;
            }
        }

        Logging.println("Signup complete -- all clusters are connected!");        
        return true;
    }
    
    int getPID(int clusterRank, int processRank) throws Exception {
        
        if (clusterRank < 0 || clusterRank > MAX_CLUSTERS) { 
            throw new Exception("Invalid cluster rank " + clusterRank);
        }
        
        if (processRank < 0 || processRank > MAX_PROCESSES_PER_CLUSTER) { 
            throw new Exception("Invalid process rank " + processRank);
        }
        
        return ((clusterRank & 0xFF) << 24) | (processRank & 0xFFFFFF);
    }
    
    private void initCommWorld() throws Exception {
        
        // Create the standard communicators MPI_COMM_WORLD, MPI_COMM_SELF and MPI_COMM_NULL.

        Logging.println("Creating communicator 0 with " + numberOfApplicationProcesses + " processes");

        // Start by generating an array containing all PIDs of MPI_COMM_WORLD in the correct order.
        int [] members = new int[numberOfApplicationProcesses];

        int offset = 0;
        
        for (Cluster c : clusters) {

            int clusterRank = c.getRank();
            int processes = c.getApplicationSize();
            
            for (int i=0;i<processes;i++) {
                members[offset + i] = getPID(clusterRank, i);
            }
            
            offset += processes;
        }
        
        Communicator com = new Communicator(this, 0, members);

        synchronized (this) {
            // communicators 0, 1 and 2 are reserved for COMM_WORLD, COMM_SELF and COMM_NULL
            communicators.add(0, com);
            communicators.add(1, null);
            communicators.add(2, null);
        }
    }
    
    private void startClusters() {
        for (Cluster c : clusters) { 
            c.startMessaging();
        }
    }
       
    private void acceptConnections() throws Exception {

        Logging.println("Waiting for " + clusters.length + " clusters to connect...");            
        
        boolean done = false;

        ServerSocket ss = new ServerSocket(port);
        ss.setSoTimeout(1000);

        while (!done) {

            try {
                Socket s = ss.accept();
                new AcceptThread(s).start();
            } catch (SocketTimeoutException e) {
                // ignored!
            }

            done = signupComplete();
        }
        
        try {
            ss.close();
        } catch (Exception e) {
            // ignored
        }
        
        initCommWorld();
        startClusters();
    }

    public synchronized void clusterDone() {
        clustersDone++;
        notifyAll();
    }

    public synchronized void allClustersDone() {
        
        while (clustersDone < numberOfClusters) { 
            try { 
                wait();
            } catch (InterruptedException e) {
                // ignored                
            }
        }
        
        done = true;
        notifyAll();
    }
    
    private synchronized void waitUntilDone() { 

        while (!done) { 
            
            Logging.println("Server waiting until application terminates.");
            
            try { 
                wait(10000);
            } catch (InterruptedException e) {
                // ignored
            }
        }        
    }

    /**
     * @return
     */
    public int getNumberOfClusters() {
        return numberOfClusters;
    }

    /**
     * @return
     */
    public int getNumberOfApplicationProcesses() {
        return numberOfApplicationProcesses;
    }

    /**
     * @return
     */
    public int getNumberOfGatewaysPerCluster() {
        return numberOfGatewaysPerCluster;
    }
    
    /**
     * @return
     */
    public int getMessageSize() {
        return messageSize;
    }
    
    /**
     * @return
     */
    public int getMessageBufferSize() {
        return messageBufferSize;
    }
    
    /**
     * 
     * @param index
     * @return
     */
    public Cluster getCluster(int index) {
        return clusters[index];
    }

    /**
     * @return
     */
    public int getNumberOfStreams() {
        return numberOfStreamsPerGatewayConnection;
    }

    /**
     * @return
     */
    public int getProtocol() {
        return protocol;
    }
    
    public void run() {
        try {         
            acceptConnections();
            waitUntilDone();
        } catch (Exception e) { 
            System.err.println("Server died unexpectedly!");
            e.printStackTrace(System.err);
        }
    }
    
    public static void main(String [] args) {

        try {            
            // Only one parameter is expected: the configuration file!
            if (args.length != 1) { 
                System.err.println("Usage: esalsa.Server <configfile>");
                System.exit(1);
            }

            File file = new File(args[0]);

            if (!file.exists() || !file.isFile() || !file.canRead()) { 
                System.err.println("Config file " + args[0] + " not found!");
                System.exit(1);
            }

            Logging.start();

            Server s = new Server(file);
            s.run();
           
            Logging.stop();

        } catch (Exception e) {
            e.printStackTrace();
        }
    }

       
}
