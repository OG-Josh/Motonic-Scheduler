## PROJECT 4 - RATE MONO TONIC SCHEDULER
Student: Joshua Yang
Class: CPSC 380
Professor: Hansen
Due date: May 11th, 2021 of Spring 2021 Term

### How to compile
- Using Makefile.
- I'm running on Ubuntu 20.4 LTS machine. With G++ version: 9.3.0
- Using c++11 or higher and pthread for compile the project

```bash
$ g++ --version
g++ (Ubuntu 9.3.0-17ubuntu1~20.04) 9.3.0
Copyright (C) 2019 Free Software Foundation, Inc.
This is free software; see the source for copying conditions.  There is NO
warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
```

```bash
$ make
rm -f proj4 *.o core*
g++ -g -Wall -pthread project4.cpp -o proj4
```

### How to run

This is example to run the program.

```bash
$ ./proj4 
Used: ./proj4 <use case> 
Use case:
        : 1 - Normal running
        : 2 - Thread 2 overrun
        : 3 - Thread 3 overrun

$ ./proj4 1
Running mode Normal
Interval periods 10 milliseconds.
The thread information: 
        Thread 1 has a period of 1 unit and run DoWork() 100 times.
        Thread 2 has a period of 2 unit and run DoWork() 200 times.
        Thread 3 has a period of 4 unit and run DoWork() 400 times.
        Thread 4 has a period of 16 unit and run DoWork() 1600 times.
Scheduler create.
Set CPU core to Scheduler: 3. Scheduler Priority 99.
Thread1created.
        Set CPU core to Thread1 : 3. 
Simulation start.
        Set Priority set to Thread1 : 98.
Thread2created.
        Set CPU core to Thread2 : 3. 
        Set Priority set to Thread2 : 97.
Thread3created.
        Set CPU core to Thread3 : 3. 
        Set Priority set to Thread3 : 96.
Thread4created.
        Set CPU core to Thread4 : 3. 
        Set Priority set to Thread4 : 95.
Begin Simulation
Simulation end.
                 Thread 1  Thread 2  Thread 3  Thread 4
       Started:       160        80        40        10
      Finished:       160        80        40        10
 Miss Deadline:         0         0         0         0
 
$ ./proj4 2
Running mode Thread 2 overrun
Interval periods 10 milliseconds.
The thread information: 
        Thread 1 has a period of 1 unit and run DoWork() 100 times.
        Thread 2 has a period of 2 unit and run DoWork() 50000 times.
        Thread 3 has a period of 4 unit and run DoWork() 400 times.
        Thread 4 has a period of 16 unit and run DoWork() 1600 times.
Scheduler create.
Set CPU core to Scheduler: 3. Scheduler Priority 99.
Thread1created.
        Set CPU core to Thread1 : 3. 
Simulation start.       Set Priority set to Thread
1 : 98.
Thread2created.
        Set CPU core to Thread2 : 3. 
        Set Priority set to Thread2 : 97.
Thread3created.
        Set CPU core to Thread3 : 3. 
        Set Priority set to Thread3 : 96.
Thread4created.
        Set CPU core to Thread4 : 3. 
        Set Priority set to Thread4 : 95.
Begin Simulation
Simulation end.
                 Thread 1  Thread 2  Thread 3  Thread 4
       Started:       160        50        40        10
      Finished:       160        49        40        10
 Miss Deadline:         0        80         0         0
 
$ ./proj4 2
Running mode Thread 2 overrun
Interval periods 10 milliseconds.
The thread information: 
        Thread 1 has a period of 1 unit and run DoWork() 100 times.
        Thread 2 has a period of 2 unit and run DoWork() 50000 times.
        Thread 3 has a period of 4 unit and run DoWork() 400 times.
        Thread 4 has a period of 16 unit and run DoWork() 1600 times.
Scheduler create.
Set CPU core to Scheduler: 3. Scheduler Priority 99.
Thread1created.
        Set CPU core to Thread1 : 3. 
        Set Priority set to Thread1 : 98.
Thread2created.
Simulation start.
        Set CPU core to Thread2 : 3. 
        Set Priority set to Thread2 : 97.
Thread3created.
        Set CPU core to Thread3 : 3. 
        Set Priority set to Thread3 : 96.
Thread4created.
        Set CPU core to Thread4 : 3. 
        Set Priority set to Thread4 : 95.
Begin Simulation
Simulation end.
                 Thread 1  Thread 2  Thread 3  Thread 4
       Started:       160        51        40        10
      Finished:       160        50        40        10
 Miss Deadline:         0        80         0         0
 
$ ./proj4 3
Running mode Thread 3 overrun
Interval periods 10 milliseconds.
The thread information: 
        Thread 1 has a period of 1 unit and run DoWork() 100 times.
        Thread 2 has a period of 2 unit and run DoWork() 200 times.
        Thread 3 has a period of 4 unit and run DoWork() 50000 times.
        Thread 4 has a period of 16 unit and run DoWork() 1600 times.
Scheduler create.
Set CPU core to Scheduler: 3. Scheduler Priority 99.
Thread1created.
Simulation start.
        Set CPU core to Thread1 : 3. 
        Set Priority set to Thread1 : 98.
Thread2created.
        Set CPU core to Thread2 : 3. 
        Set Priority set to Thread2 : 97.
Thread3created.
        Set CPU core to Thread3 : 3. 
        Set Priority set to Thread3 : 96.
Thread4created.
        Set CPU core to Thread4 : 3. 
        Set Priority set to Thread4 : 95.
Begin Simulation
Simulation end.
                 Thread 1  Thread 2  Thread 3  Thread 4
       Started:       160        80        40        10
      Finished:       160        80        40        10
 Miss Deadline:         0         0         0         0
 
$ ./proj4 5
Used: ./proj4 <use case> 
Use case:
        : 1 - Normal running
        : 2 - Thread 2 overrun
        : 3 - Thread 3 overrun
```

### Design - How were the threads synchronized and dispatched.
- I'm using the mechanism sem_post and sem_wait for waiting and dispatched.

- Begin the thread, we use the sem_wait to wait on a semaphore. When we want to run the thread, from the another thread just using the sem_post to increment the value of a semaphore.


