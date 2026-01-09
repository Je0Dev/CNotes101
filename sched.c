#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define N 5

// Δομή για τη διαχείριση των διεργασιών
typedef struct {
    int id;
    int arrival_time;
    int burst_time;
    int priority;
    int remaining_time;
    int completion_time;
    int turnaround_time;
    int waiting_time;
} Process;

// FCFS (First-Come, First-Served)
void fcfs(Process processes[], int n) {
    int current_time = 0;
    
    for (int i = 0; i < n; i++) {
        if (current_time < processes[i].arrival_time) {
            current_time = processes[i].arrival_time;
        }
        
        processes[i].completion_time = current_time + processes[i].burst_time;
        current_time = processes[i].completion_time;
        
        processes[i].turnaround_time = processes[i].completion_time - processes[i].arrival_time;
        processes[i].waiting_time = processes[i].turnaround_time - processes[i].burst_time;
    }
}

// SJF (Shortest Job First) - Non-preemptive
void sjf(Process processes[], int n) {
    Process sorted[n];
    for (int i = 0; i < n; i++) {
        sorted[i] = processes[i];
    }
    
    // Ταξινόμηση με βάση το χρόνο εκτέλεσης
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            if (sorted[j].burst_time > sorted[j+1].burst_time) {
                Process temp = sorted[j];
                sorted[j] = sorted[j+1];
                sorted[j+1] = temp;
            }
        }
    }
    
    int current_time = 0;
    for (int i = 0; i < n; i++) {
        if (current_time < sorted[i].arrival_time) {
            current_time = sorted[i].arrival_time;
        }
        
        sorted[i].completion_time = current_time + sorted[i].burst_time;
        current_time = sorted[i].completion_time;
        
        sorted[i].turnaround_time = sorted[i].completion_time - sorted[i].arrival_time;
        sorted[i].waiting_time = sorted[i].turnaround_time - sorted[i].burst_time;
        
        // Ενημέρωση του αρχικού πίνακα
        for (int j = 0; j < n; j++) {
            if (processes[j].id == sorted[i].id) {
                processes[j] = sorted[i];
                break;
            }
        }
    }
}

// Priority Scheduling (Non-preemptive)
void priority_scheduling(Process processes[], int n) {
    Process sorted[n];
    for (int i = 0; i < n; i++) {
        sorted[i] = processes[i];
    }
    
    // Ταξινόμηση με βάση την προτεραιότητα (υψηλότερος αριθμός = υψηλότερη προτεραιότητα)
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            if (sorted[j].priority < sorted[j+1].priority) {
                Process temp = sorted[j];
                sorted[j] = sorted[j+1];
                sorted[j+1] = temp;
            }
        }
    }
    
    int current_time = 0;
    for (int i = 0; i < n; i++) {
        if (current_time < sorted[i].arrival_time) {
            current_time = sorted[i].arrival_time;
        }
        
        sorted[i].completion_time = current_time + sorted[i].burst_time;
        current_time = sorted[i].completion_time;
        
        sorted[i].turnaround_time = sorted[i].completion_time - sorted[i].arrival_time;
        sorted[i].waiting_time = sorted[i].turnaround_time - sorted[i].burst_time;
        
        // Ενημέρωση του αρχικού πίνακα
        for (int j = 0; j < n; j++) {
            if (processes[j].id == sorted[i].id) {
                processes[j] = sorted[i];
                break;
            }
        }
    }
}

// Round Robin (RR)
void round_robin(Process processes[], int n, int quantum) {
    int current_time = 0;
    int completed = 0;
    bool processed[n];
    for (int i = 0; i < n; i++) {
        processed[i] = false;
        processes[i].remaining_time = processes[i].burst_time;
    }
    
    while (completed < n) {
        bool all_done = true;
        
        for (int i = 0; i < n; i++) {
            if (!processed[i] && processes[i].arrival_time <= current_time && processes[i].remaining_time > 0) {
                all_done = false;
                
                if (processes[i].remaining_time > quantum) {
                    current_time += quantum;
                    processes[i].remaining_time -= quantum;
                } else {
                    current_time += processes[i].remaining_time;
                    processes[i].remaining_time = 0;
                    processes[i].completion_time = current_time;
                    processes[i].turnaround_time = processes[i].completion_time - processes[i].arrival_time;
                    processes[i].waiting_time = processes[i].turnaround_time - processes[i].burst_time;
                    processed[i] = true;
                    completed++;
                }
            }
        }
        
        if (all_done) {
            current_time++;
        }
    }
}

// Priority with Round Robin
void priority_round_robin(Process processes[], int n, int quantum) {
    int current_time = 0;
    int completed = 0;
    bool processed[n];
    
    for (int i = 0; i < n; i++) {
        processed[i] = false;
        processes[i].remaining_time = processes[i].burst_time;
    }
    
    while (completed < n) {
        int highest_priority = -1;
        int selected_process = -1;
        
        // Βρίσκουμε τη διεργασία με την υψηλότερη προτεραιότητα που είναι έτοιμη
        for (int i = 0; i < n; i++) {
            if (!processed[i] && processes[i].arrival_time <= current_time && processes[i].remaining_time > 0) {
                if (processes[i].priority > highest_priority) {
                    highest_priority = processes[i].priority;
                    selected_process = i;
                }
            }
        }
        
        if (selected_process == -1) {
            current_time++;
            continue;
        }
        
        if (processes[selected_process].remaining_time > quantum) {
            current_time += quantum;
            processes[selected_process].remaining_time -= quantum;
        } else {
            current_time += processes[selected_process].remaining_time;
            processes[selected_process].remaining_time = 0;
            processes[selected_process].completion_time = current_time;
            processes[selected_process].turnaround_time = processes[selected_process].completion_time - processes[selected_process].arrival_time;
            processes[selected_process].waiting_time = processes[selected_process].turnaround_time - processes[selected_process].burst_time;
            processed[selected_process] = true;
            completed++;
        }
    }
}

// Εκτύπωση αποτελεσμάτων
void print_results(Process processes[], int n, char* algorithm_name) {
    printf("\n%s Scheduling:\n", algorithm_name);
    printf("Process\tArrival\tBurst\tPriority\tCompletion\tTurnaround\tWaiting\n");
    printf("---------------------------------------------------------------\n");
    
    float avg_turnaround = 0, avg_waiting = 0;
    
    for (int i = 0; i < n; i++) {
        printf("P%d\t%d\t%d\t%d\t\t%d\t\t%d\t\t%d\n", 
               processes[i].id, processes[i].arrival_time, processes[i].burst_time,
               processes[i].priority, processes[i].completion_time, 
               processes[i].turnaround_time, processes[i].waiting_time);
        
        avg_turnaround += processes[i].turnaround_time;
        avg_waiting += processes[i].waiting_time;
    }
    
    printf("\nAverage Turnaround Time: %.2f\n", avg_turnaround / n);
    printf("Average Waiting Time: %.2f\n", avg_waiting / n);
}

int main() {
    int n = N;
    
    // Παράδειγμα διεργασιών
    Process processes_fcfs[N] = {
        {1, 0, 10, 3, 0, 0, 0, 0},
        {2, 1, 1, 1, 0, 0, 0, 0},
        {3, 2, 2, 4, 0, 0, 0, 0},
        {4, 3, 1, 5, 0, 0, 0, 0},
        {5, 4, 5, 2, 0, 0, 0, 0}
    };
    
    // Αντίγραφα για τους υπόλοιπους αλγορίθμους
    Process processes_sjf[N], processes_priority[N], processes_rr[N], processes_prr[N];
    for (int i = 0; i < n; i++) {
        processes_sjf[i] = processes_fcfs[i];
        processes_priority[i] = processes_fcfs[i];
        processes_rr[i] = processes_fcfs[i];
        processes_prr[i] = processes_fcfs[i];
    }
    
    // FCFS
    fcfs(processes_fcfs, n);
    print_results(processes_fcfs, n, "FCFS");
    
    // SJF
    sjf(processes_sjf, n);
    print_results(processes_sjf, n, "SJF");
    
    // Priority Scheduling
    priority_scheduling(processes_priority, n);
    print_results(processes_priority, n, "Priority");
    
    // Round Robin
    round_robin(processes_rr, n, 2); // quantum = 2
    print_results(processes_rr, n, "Round Robin");
    
    // Priority with Round Robin
    priority_round_robin(processes_prr, n, 2); // quantum = 2
    print_results(processes_prr, n, "Priority with RR");
    
    return 0;
}