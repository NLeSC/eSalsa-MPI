package cesm2;

import java.util.List;

public class Cluster {

    final String name;
    final int clusterRank; // Rank of cluster
    final int localCount; // Size of cluster

    private int connectionCount;

    private final Connection[] connections;

    Cluster(String name, int localCount, int clusterRank) {
        this.name = name;
        this.localCount = localCount;
        this.clusterRank = clusterRank;
        this.connections = new Connection[localCount];
    }

    void getConnections(List<Connection> out) {
        for (Connection c : connections) {
            out.add(c);
        }
    }

    synchronized void addConnection(int rank, int size, String name,
            Connection c) throws Exception {

        if (rank < 0 || rank >= connections.length) {
            throw new Exception("Illegal rank " + rank + " for cluster " + name
                    + " of size " + localCount);
        }

        if (localCount != size) {
            throw new Exception("Size of cluster does not match (" + size
                    + " != " + localCount + ")");
        }

        if (!this.name.equals(name)) {
            throw new Exception("Name of cluster does not match (" + name
                    + " != " + this.name + ")");
        }

        if (connections[rank] != null) {
            throw new Exception("Rank " + rank + " on cluster " + name
                    + " already in use!");
        }

        connections[rank] = c;
        connectionCount++;

        Logging.println("Added connection " + rank + " to " + name);
    }

    synchronized boolean complete() {
        return connectionCount == localCount;
    }
}
