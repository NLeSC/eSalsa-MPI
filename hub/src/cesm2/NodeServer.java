package cesm2;

import java.net.ServerSocket;
import java.net.SocketTimeoutException;

import cesm2.messages.ApplicationMessage;
import cesm2.messages.Message;

public class NodeServer {

    public final static int MAX_NODE    = 0xFFFFFF;
    public final static int MAX_CLUSTER = 0xFF;

    private final int port;
    private final int processes;

    protected final int nodeID;
    protected final int clusterID;
    protected final int clusterCount;

    protected final String clusterName;

    // FIXME: Should be a hashmap ?
    private final Connection [] connections;

    private boolean done = false;

    public NodeServer(int port, int processes, int nodeID, int clusterID, int clusterCount, String clusterName) {
        this.port = port;
        this.processes = processes;
        this.nodeID = nodeID;
        this.clusterID = clusterID;
        this.clusterCount = clusterCount;
        this.clusterName = clusterName;
        this.connections = new Connection[processes];
    }

    protected void deliver(Message m) {

        long dest = m.destinationPID;

        for (int i=0;i<connections.length;i++) {
            if (dest == connections[i].pid) {
                connections[i].enqueue(m, true);
                return;
            }
        }

        Logging.error("Cannot deliver message, unknown target " + dest);
    }

    public void acceptConnections() throws Exception {

        // Accepts a (small) number of local connections.
        ServerSocket ss = new ServerSocket(port);
        ss.setSoTimeout(1000);

        for (int i=0;i<processes;i++) {
            boolean done = false;

            do {
                try {
                    connections[i] = new Connection(this, ss.accept());
                    done = true;
                } catch (SocketTimeoutException e) {
                    // ignored!
                }
            } while (!done);
        }

        try {
            ss.close();
        } catch (Exception e) {
            // ignored
        }
    }

    void registerNodeServer() throws Exception {







    }

    void startConnections() {

        for (int i=0;i<processes;i++) {
            connections[i].start();
        }
    }

    public synchronized void done() {
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

    public void run() throws Exception {
        acceptConnections();
        registerNodeServer();
        startConnections();
        waitUntilDone();
    }

    public static void main(String [] args) {

        try {
            int port = 45678;
            int processes = -1;
            int nodeID = -1;
            int clusterID = -1;
            int clusterCount = -1;
            String clusterName = null;

            for (int i=0;i<args.length;i++) {

                if (args[i].equals("--port")) {
                    if (++i < args.length) {
                        port = Integer.parseInt(args[i]);
                    } else {
                        System.err.println("Missing parameter for --port");
                    }
                } else if (args[i].equals("--processes")) {
                    if (++i < args.length) {
                        processes = Integer.parseInt(args[i]);
                    } else {
                        System.err.println("Missing parameter for --processes");
                    }

                } else if (args[i].equals("--nodeID")) {
                    if (++i < args.length) {
                        nodeID = Integer.parseInt(args[i]);
                    } else {
                        System.err.println("Missing parameter for --nodeID");
                    }

                } else if (args[i].equals("--clusterID")) {
                    if (++i < args.length) {
                        clusterID = Integer.parseInt(args[i]);
                    } else {
                        System.err.println("Missing parameter for --clusterID");
                    }


                } else if (args[i].equals("--clusterCount")) {
                    if (++i < args.length) {
                        clusterCount = Integer.parseInt(args[i]);
                    } else {
                        System.err.println("Missing parameter for --clusterCount");
                    }
                } else if (args[i].equals("--clusterName")) {
                    if (++i < args.length) {
                        clusterName = args[i];
                    } else {
                        System.err.println("Missing parameter for --clusterName");
                    }
                } else {
                    System.err.println("Unknown option " + args[i]);
                }
            }

            if (port <= 0 || port > 65535) {
                System.err.println("Illegal port number " + port);
            }

            if (processes <= 0) {
                System.err.println("Illegal processes count " + processes);
            }

            if (nodeID < 0 || nodeID >= MAX_NODE) {
                System.err.println("Illegal nodeID " + nodeID);
            }

            if (clusterID < 0 || clusterID >= MAX_CLUSTER) {
                System.err.println("Illegal clusterID " + clusterID);
            }

            if (clusterCount < 0 || clusterCount >= MAX_CLUSTER) {
                System.err.println("Illegal clusterCount " + clusterCount);
            }

            Logging.start();

            new NodeServer(port, processes, nodeID, clusterID, clusterCount, clusterName).run();

            Logging.stop();

        } catch (Exception e) {
            e.printStackTrace();
        }
    }
}
