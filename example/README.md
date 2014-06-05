Example eSalsa-MPI configuration.
---------------------------------

This directory contains an example for eSalsa MPI consisting of:

- [example.c](https://github.com/NLeSC/eSalsa-MPI/tree/develop/example/example.c): an example MPI application.
- [server.config](https://github.com/NLeSC/eSalsa-MPI/tree/develop/example/server.config): a server configuration file.
- [location1.config](https://github.com/NLeSC/eSalsa-MPI/tree/develop/example/location1.config): a confuration file for location 1.
- [location2.config](https://github.com/NLeSC/eSalsa-MPI/tree/develop/example/location2.config): a configuration for location 2.

The example application is very simple; It initializes MPI, prints its rank and the size of MPI_COMM_WORLD, 
and then exists:

     #include <stdio.h>
     #include "mpi.h"

     int main(int argc, char *argv[])
     {
        int rank, size, error;

        error = MPI_Init(&argc, &argv);

        if (error != MPI_SUCCESS) {
           fprintf(stderr, "Init failed! %d\n", error);
        }

        MPI_Comm_size(MPI_COMM_WORLD, &size);
        MPI_Comm_rank(MPI_COMM_WORLD, &rank);
 
        fprintf(stderr, "Process %d of %d\n", rank, size);

        MPI_Finalize();

        return 0;
     }


To compile this example use the `empicc` script. Make sure that the 
`EMPI_HOME` variable set and pointing to your eSalsa-MPI directory. 
Then run the following command:

     $EMPI_HOME/scripts/empicc example.c -o example

When the example is compiled, an eSalsa-MPI configuration must be created to run it. 

### Server configuration

The "server.config" file describes the server setup, the number
of locations used in an experiment, and the configuration used 
in each of those locations. 

In this example, we configure the server to support an experiment 
that is run in two locations called "location1" and "location2". 
These names are only used to identify a location, so any string 
may be used as long as it does not contain whitespace and each 
string is unique.

The server is configured to listnen to incoming connections from 
each location. In our example we will use port 6677. Since only
a single gateway from each location will connect to the server,
so only 2 connections to the server are created.

Each location is configured to use one or more gateway tasks. 
In addition, each gateway tasks is configured to use one or more
TCP streams to connect to a 'peer' gateway in the other location. 
As a result, (gateways * streams_per_gateway) TCP streams will 
be created. In this example, we will use a single gateway task 
and a single TCP stream.

The resulting "server.config" file looks like this:

     # Name of this experiment
     Example 1
     # Port at which the server should listnen. 
     6677
     # Number of locations used in this experiment
     2
     # Number of gateways used per location
     1
     # Number of streams used to connect gateways 
     1

Next, the "server.config" file contains a configuration for each
individual location. This configuration describes the number of 
application tasks to use and the port range and network interface 
used by the gateways. Only the start of the port range needs to 
be specified. On port will be added for each TCP stream on each 
gateway. Therefore the entire port range is:

     N ... N + (gateways * streams_per_gateway)

In this example only one port will be needed. The rest of the 
"server.config" file looks like this:

     # Name of the first location
     location1
     # Number of application tasks
     1
     # Start of TCP port range
     12000
     # Network interface to use on the gateways
     192.168.0.0/24

     # Name of second location
     location2
     # Number of application tasks.
     2
     # Start of port range.
     14000
     # Network interface to use on the gateways
     192.168.0.0/24


### Location configurations

In addition to the "server.config" file, two separate config 
files are needed for each location. These config files are read
by the gateways in each location, and used to contact the server 
and identify itself. 

These config files contain single line:

    <name> <server address> <server port>

In this example the "location1.config" looks like this: 

    location1 192.168.0.5 6677

and the "location2.config" looks like this: 

    location2 192.168.0.5 6677

Note that the "name" in each of these config files should match 
one of the names defined in the "server.config" file.

The "server address server port" should contain the IP address 
and port number at which the server can be reached. 


### Running the example:

To run the example application using the configuration shown above you 
first need to start the eSalsa-MPI server.

     $EMPI_HOME/scripts/empi-server.sh server.config

The server should now start and print something like this:

     0 : Logging started
     5 : Starting eSalsa MPI server for experiment "Example 1"
     5 :    Clusters                 : 2
     5 :    Gateways/clusters        : 1
     5 :    Application processes    : 3
     5 :    Total processes          : 5
     5 :    Parallel streams         : 1
     5 :    Server listening on port : 6677
     5 :    --------------------------
     5 :    Cluster 0 name           : "location1"
     5 :       Application processes : 1
     5 :       Port range in use     : 12000 ... 12001
     5 :       Network to use        : 192.168.0.0/255.255.255.0
     5 :    --------------------------
     5 :    Cluster 1 name           : "location2"
     5 :       Application processes : 2
     5 :       Port range in use     : 14000 ... 14001
     5 :       Network to use        : 192.168.0.0/255.255.255.0
     5 : 
     6 : Waiting for 2 clusters to connect...

Next, start the test application as two separate MPI jobs. For each 
MPI job you must specify which eSalsa-MPI location config file to use.
For example: 

     EMPI_CONFIG=location1.config mpirun -np 2 example
   
and 

     EMPI_CONFIG=location2.config mpirun -np 3 example

Note that the `EMPI_CONFIG` variable is set to a different location config
file in each example. In addition, each "mpirun" command must start 

    (application tasks + gateway tasks) 

MPI tasks in each location. In the example, "location1" needs 2 tasks, and "location2" 
needs 3 tasks. 

It the application starts correctly, it will print something like this for "location1":

    Process 0 of 3

and 

    Process 1 of 3
    Process 2 of 3

for "location2". This shows that eSalsa-MPI has combined the two MPI jobs, 
and presents it as a single 3 task job to the application.

