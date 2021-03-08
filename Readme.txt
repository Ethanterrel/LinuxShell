

The implementation of this program is divided into five main structures, being four FIFO queues and one priority queue. These have been implemented by using a linked list code, with each function used to serve the basic idea of of the program in different ways which was, defining time, sorting, and overall stability within organizing the FIFO queus in such a way where the events are generates as correctly as possible.

I assigned the certain jobs in the program certain numbers from 1 to 10 that handle multiple events.That being:handleJobArrival, handleArrivalCPU, handleCPUExit, handleDisk1Exit, handleArrivalDisk1, handleArrivalDisk2,
handleArrivalNetwork1, handleArrivalNetwork2. These events are called from certain times under the varibles I named:
avgResponseTime, maxResponseTime, utilization, throughput, and maxsize.

I included a function called 'createLogfile' which creates my 'log-file' which takes in an event and writes to the log file depending on which event it is. Specifically, 'updatestats' allows for the different properties of programs to be stored through the function 'stats'.

Each time this program is run, it creates two files--a log file that details which of these jobs were processed, detailing the specific event that occurred and the time at which it took place, all in order of occurrence, and a stat-file, which details the statistics of each FIFOqueue calling them each by a value, 1 being CPU, 2 being Disk 1, 3 being Disk 2, and 4 being Network.

To execute, I used many if, else and while statements to implement if a certain job had been completed. Once entering the FIFO queue, jobs are run until any actual defined FIN_TIME. After implementing through the jobs and checking if they have completed the queue, avgResponseTime, maxResponseTime, utilization, throughput, and maxsize are noted and tracked and are detailed on the 'stats-file'.