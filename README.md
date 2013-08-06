eSalsa-MPI
==========

What is it? 
-----------

This project contains the wide area MPI used in the eSalsa project. 

This code is __VERY__ experimental!!

This wide area MPI consists of two parts, an MPI wrapper (written in C), 
and a Hub (written in Java). 

The MPI wrapper uses the MPI profiling interface to intercept calls to a 
_real_ MPI implementation and change their behaviour. 

The Hub acts as a (currently centralized) server that can be used to 
exchange data. 

The combination of the Hub and the MPI wrapper can be used to merge 
several distinct MPI application (possibly running on different sites 
and using different MPI libraries) into one large application.

To do this, each MPI process that uses the MPI wrapper creates a socket 
connection to the Hub when it starts. All MPI calls are then intercepted, 
and the illusion of a single large MPI application is presented to the 
processes. 

For each intercepted call a decision is made if the call can be handled 
locally (for example, an MPI_Send to a different process in the same 
cluster) or if the hub must be involved (for example an MPI_Send between 
clusters or a collective operation that involved all nodes). 

As mentioned above, this code is __VERY__ experimental. However, its has 
already been used to run both the Parallel Ocean Program (POP) and the 
Community Earth System Model (CESM) in a multicluster setting.  
 
POP can be found at <http://climate.lanl.gov/Models/POP/>
CESM can be found at <http://www2.cesm.ucar.edu/>

What is the eSalsa Project?
---------------------------

The eSalsa Project is a cooperation between the Netherlands eScience 
Center (NLeSC), the Institute for Marine and Atmospheric Research (IMAU) 
at Utrecht University, and the Vrije Universiteit Amsterdam (VU). 

The goal of the eSalsa project is to determine to what extent regional sea 
level in the eastern North Atlantic will be affected by changes in ocean 
circulation over the next decades.

During the project we will improve and extend POP with support for 
distributed computing techniques and accelerators (GPUs).

For more information on the eSalsa project see:
 
<http://www.esciencecenter.nl/projects/project-portfolio/climate-research>


The Latest Version
------------------

Details of the latest version can be found on the eSalsa MPI project 
web site at:

<https://github.com/NLeSC/eSalsa-MPI>

