/*
* Copyright (C) Josh Y. - All Rights Reserved
* Unauthorized copying of this file, via any medium is strictly prohibited 
* Proprietary and confidential
* Written by Josh Y. <joyang112@gmail.com>, June 2017
*/

#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>
#include <chrono>
#include <thread>
#include <iomanip>
#include <sched.h>

using namespace std;

#define MAX_COL 10
#define MAX_ROW 10
// 4 thread worker and 1 for scheduler
#define NUM_TREAD 5
#define FRAME_PERIOD 16
#define PERIOD 10
// Use mutex for worker only
#define NUM_MUTEX 4
// A unit shall be any where from 10-100ms (depending on operating system options)
#define UNIT_MS 10
#define CPU_CORE 3

// Semaphore using in the thread
sem_t sem[NUM_TREAD];  // Semaphore for all thread
// Thread using in the thread
pthread_mutex_t mutex[NUM_MUTEX];
// Thread period

// Thread1 has a period of 1 unit
// Thread2 has a period of 2 unit
// Thread3 has a period of 4 unit
// Thread4 has a period of 16 unit
int ThreadTimePerPeriod[] = { 1, 2, 4, 16 };
int TaskStart[] = { 0, 0, 0, 0 };
int count1, count2, count3, count4;

// Thread1 executes DoWork 100 times
// Thread2 executes DoWork 200 times
// Thread3 executes DoWork 400 times
// Thread4 executes DoWork 1600 times
int ThreadTimeRunDoWorkPerPeriod[] = { 100, 200, 400, 1600 };

// Count thread finished time
int ThreadFinishedTimeCnt[] = { 0, 0, 0, 0 };
int SchedulerInvokleThread[] = { 0, 0, 0, 0 };
int TaskMissDeadline[] = { 0, 0, 0, 0 };
int ThreadID[] = { 0, 1, 2, 3 };
// Time to End Scheduler
chrono::system_clock::time_point EndSimulator;
// Using for interval per perior
const chrono::milliseconds PeriorInterValMS { UNIT_MS };

/**
 * DoWork action.
 * @param: times - The number run the job
 */
void DoWork(int times) {
  for (int i = 0; i < times; i++) {
    double board[MAX_ROW][MAX_COL];
    for (int x = 0; x < MAX_ROW; x++) {
      for (int y = 0; y < MAX_COL; y++) {
        board[x][y] = 1.0;
      }
    }
    // Will multiple the content of each cell of a 10x10 matrix starting with
    // column 0 followed by 5, then 1 followed by 6, etc
    for (int x = 0; x < MAX_COL / 2; x++) {
      for (int y = 0; y < MAX_ROW; y++) {
        board[y][x] = board[y][x]
            * board[(y + 1) % MAX_ROW][(x + MAX_ROW / 2) % MAX_COL];
        board[y][(x + MAX_ROW / 2) % MAX_COL] = board[y][(x + MAX_ROW / 2)
            % MAX_COL] * board[(y + 1) % MAX_ROW][x];
      }
    }
  }
}

/**
 * Thread worker.
 *
 * @param: ThreadID: Thread number identify.
 */
void* ThreadWorer(void *threadID) {
  int *id = (int*) threadID;
  int maxTime = FRAME_PERIOD * PERIOD / ThreadTimePerPeriod[*id];
  // Thread1 executes DoWork 100 times
  // Thread2 executes DoWork 200 times
  // Thread3 executes DoWork 400 times
  // Thread4 executes DoWork 1600 times
  for (int x = 0; x < maxTime; x++) {
    sem_wait(&sem[(*id) + 1]);
    TaskStart[*id] += 1;
    // call DoWork
    DoWork(ThreadTimeRunDoWorkPerPeriod[*id]);
    if (chrono::system_clock::now() > EndSimulator) {
      // Exit thread when time for simulator is over
      break;
    }
    ThreadFinishedTimeCnt[*id] += 1;
    pthread_mutex_lock(&mutex[*id]);
    SchedulerInvokleThread[*id] -= 1;
    pthread_mutex_unlock(&mutex[*id]);
  }
  // Exit thread
  pthread_exit(NULL);
}

void* RateMonotonicScheduler(void*) {
  cout << "Simulation start." << endl;
  sem_wait(&sem[0]);
  chrono::system_clock::time_point currentTime = chrono::system_clock::now();
  // Calculate new EndSimualator
  EndSimulator = currentTime + PeriorInterValMS * FRAME_PERIOD * PERIOD;
  int runCnt = FRAME_PERIOD * PERIOD;
  for (int i = 0; i <= runCnt; i++) {
    currentTime += PeriorInterValMS;
    // Thread 1 - has a period of 1 unit
    pthread_mutex_lock(&mutex[0]);
    if (SchedulerInvokleThread[0] > 0) {
      TaskMissDeadline[0] += 1;
    }
    SchedulerInvokleThread[0] += 1;
    pthread_mutex_unlock(&mutex[0]);
    sem_post(&sem[1]);
    // Thread 2 - have a period of 2 units
    if (i % ThreadTimePerPeriod[1] == 0) {
      pthread_mutex_lock(&mutex[1]);
      if (SchedulerInvokleThread[1] > 0)
        TaskMissDeadline[1] += 1;
      SchedulerInvokleThread[1] += 1;
      pthread_mutex_unlock(&mutex[1]);
      sem_post(&sem[2]);
    }
    // Thread 3 - havs period of 4 units
    if (i % ThreadTimePerPeriod[2] == 0) {
      pthread_mutex_lock(&mutex[2]);
      if (SchedulerInvokleThread[2] > 0)
        TaskMissDeadline[2]++;
      SchedulerInvokleThread[2]++;
      pthread_mutex_unlock(&mutex[2]);
      sem_post(&sem[3]);
    }
    // Thread 4 - has a period of 16 units
    if (i % ThreadTimePerPeriod[3] == 0) {
      pthread_mutex_lock(&mutex[3]);
      if (SchedulerInvokleThread[3] > 0)
        TaskMissDeadline[3]++;
      SchedulerInvokleThread[3]++;
      pthread_mutex_unlock(&mutex[3]);
      sem_post(&sem[4]);
    }
    // Sleep until next time
    this_thread::sleep_until(currentTime);
  }

  cout << "Simulation end." << endl;
  cout << setw(15) << setfill(' ') << " ";
  cout << setw(10) << setfill(' ') << "Thread 1";
  cout << setw(10) << setfill(' ') << "Thread 2";
  cout << setw(10) << setfill(' ') << "Thread 3";
  cout << setw(10) << setfill(' ') << "Thread 4" << endl;

  // Info
  cout << setw(15) << setfill(' ') << "Started:";
  cout << setw(10) << setfill(' ') << TaskStart[0];
  cout << setw(10) << setfill(' ') << TaskStart[1];
  cout << setw(10) << setfill(' ') << TaskStart[2];
  cout << setw(10) << setfill(' ') << TaskStart[3] << endl;

  // Finished
  cout << setw(15) << setfill(' ') << "Finished:";
  cout << setw(10) << setfill(' ') << ThreadFinishedTimeCnt[0];
  cout << setw(10) << setfill(' ') << ThreadFinishedTimeCnt[1];
  cout << setw(10) << setfill(' ') << ThreadFinishedTimeCnt[2];
  cout << setw(10) << setfill(' ') << ThreadFinishedTimeCnt[3] << endl;

  // Miss deadline

  cout << setw(15) << setfill(' ') << "Miss Deadline:";
  cout << setw(10) << setfill(' ') << TaskMissDeadline[0];
  cout << setw(10) << setfill(' ') << TaskMissDeadline[1];
  cout << setw(10) << setfill(' ') << TaskMissDeadline[2];
  cout << setw(10) << setfill(' ') << TaskMissDeadline[3] << endl;

  pthread_exit(NULL);
}

int main(int argc, char **argv) {
  if (argc != 2) {
    cout << "Used: " << argv[0] << " <use case> " << endl;
    cout << "Use case:" << endl;
    cout << "\t: 1 - Normal running" << endl;
    cout << "\t: 2 - Thread 2 overrun" << endl;
    cout << "\t: 3 - Thread 3 overrun" << endl;
    return 1;
  }
  int mode = atoi(argv[1]);
  switch (mode) {
    case 1:
      cout << "Running mode Normal" << endl;
      break;
    case 2:
      cout << "Running mode Thread 2 overrun" << endl;
      ThreadTimeRunDoWorkPerPeriod[1] = 50000;
      break;
    case 3:
      cout << "Running mode Thread 3 overrun" << endl;
      ThreadTimeRunDoWorkPerPeriod[2] = 50000;
      break;
    default:
      cout << "Used: " << argv[0] << " <use case> " << endl;
      cout << "Use case:" << endl;
      cout << "\t: 1 - Normal running" << endl;
      cout << "\t: 2 - Thread 2 overrun" << endl;
      cout << "\t: 3 - Thread 3 overrun" << endl;
      return 1;
  }
  // Set-up CPU set
  cpu_set_t cpuScheduler;
  CPU_ZERO(&cpuScheduler);
  CPU_SET(CPU_CORE, &cpuScheduler);

  // Define thread parameter using for priority
  struct sched_param schedParameter;
  pthread_t *threadList = new pthread_t[NUM_TREAD];
  // Init mutex and semaphore
  for (int i = 0; i < NUM_TREAD; ++i) {
    sem_init(&sem[i], 0, 0);
    if (i < NUM_MUTEX)
      pthread_mutex_init(&mutex[i], NULL);
  }
  cout << "Interval periods " << UNIT_MS << " milliseconds." << endl;
  cout << "The thread information: " << endl;
  for (int i = 0; i < NUM_MUTEX; i++) {
    cout << "\tThread " << (i + 1) << " has a period of "
         << ThreadTimePerPeriod[i] << " unit and run DoWork() "
         << ThreadTimeRunDoWorkPerPeriod[i] << " times." << endl;
  }
  // Set argum,ent for scheduler
  schedParameter.sched_priority = sched_get_priority_max(SCHED_FIFO);
  cout << "Scheduler create." << endl;
  pthread_create(&threadList[0], NULL, RateMonotonicScheduler, NULL);
  cout << "Set CPU core to Scheduler: " << CPU_CORE << ". ";
  pthread_setaffinity_np(threadList[0], sizeof(cpu_set_t), &cpuScheduler);
  cout << "Scheduler Priority " << schedParameter.sched_priority << "." << endl;
  pthread_setschedparam(threadList[0], SCHED_FIFO, &schedParameter);
  // Create the workder
  for (int i = 1; i < NUM_TREAD; i++) {
    cout << "Thread" << i << "created." << endl;
    pthread_create(&threadList[i], NULL, ThreadWorer, (void*) &ThreadID[i - 1]);
    cout << "\tSet CPU core to Thread" << i << " : " << CPU_CORE << ". "
         << endl;
    pthread_setaffinity_np(threadList[i], sizeof(cpu_set_t), &cpuScheduler);
    schedParameter.sched_priority--;
    cout << "\tSet Priority set to Thread" << i << " : "
         << schedParameter.sched_priority << "." << endl;
    pthread_setschedparam(threadList[i], SCHED_FIFO, &schedParameter);
  }
  cout << "Begin Simulation" << endl;
  // Start the scheduler
  sem_post(&sem[0]);
  // Join all thread
  for (int i = NUM_TREAD; i > 0; i--) {
    pthread_join(threadList[i - 1], NULL);
  }
  // Go there mean program end. Need to release all mutex and semaphore
  for (int i = 0; i < NUM_TREAD; ++i) {
    sem_destroy(&sem[i]);
    if (i < NUM_MUTEX)
      pthread_mutex_destroy(&mutex[i]);
  }
  // deallocated thread
  delete threadList;
  return 0;
}
