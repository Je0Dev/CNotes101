#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "queue_sim.h"

// --- Simulation Parameters ---
#define SIMULATION_TIME 480 // 480 minutes (8 hours)
#define NUM_TELLERS 3
#define AVG_SERVICE_TIME 5  // Avg 5 minutes
#define CUSTOMER_PROB 3     // 1 in 3 chance of new customer each min
// -----------------------------

int main() {
    srand(time(NULL));
    
    Queue *bankQueue = createQueue();
    int tellers[NUM_TELLERS] = {0}; // 0 = free, >0 = time left
    
    int total_wait_time = 0;
    int total_customers = 0;

    printf("--- Bank Queue Simulation (Tellers: %d) ---\n", NUM_TELLERS);

    // Main simulation loop (time ticks 0 to 479)
    for (int current_time = 0; current_time < SIMULATION_TIME; current_time++) {
        
        // 1. New customer arrival?
        if ((rand() % CUSTOMER_PROB) == 0) {
            Customer newCustomer;
            newCustomer.arrival_time = current_time;
            // Service time: 1 to (2*AVG - 1)
            newCustomer.service_time = (rand() % (AVG_SERVICE_TIME * 2 - 1)) + 1;
            enqueue(bankQueue, newCustomer);
            total_customers++;
            printf("[Min %d] New customer arrived (service: %d min). Queue: %d\n",
                   current_time, newCustomer.service_time, bankQueue->size);
        }

        // 2. Assign free tellers
        for (int i = 0; i < NUM_TELLERS; i++) {
            if (tellers[i] == 0 && !isQueueEmpty(bankQueue)) {
                Customer nextCustomer = dequeue(bankQueue);
                tellers[i] = nextCustomer.service_time;
                
                int wait_time = current_time - nextCustomer.arrival_time;
                total_wait_time += wait_time;
                
                printf("[Min %d] Teller %d serving customer (wait: %d min, service: %d min). Queue: %d\n",
                       current_time, i+1, wait_time, nextCustomer.service_time, bankQueue->size);
            }
        }
        
        // 3. Decrement teller service time
        for (int i = 0; i < NUM_TELLERS; i++) {
            if (tellers[i] > 0) {
                tellers[i]--;
            }
        }
    }

    printf("\n--- Simulation Complete ---\n");
    printf("Total customers served: %d\n", total_customers - bankQueue->size);
    printf("Total customers left in queue: %d\n", bankQueue->size);
    printf("Total wait time: %d minutes\n", total_wait_time);
    
    double avg_wait = (double)total_wait_time / (total_customers - bankQueue->size);
    printf("Average wait time: %.2f minutes\n", avg_wait);
    
    freeQueue(bankQueue);
    return 0;
}
