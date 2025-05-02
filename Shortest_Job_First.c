#include <stdio.h> 
#include <stdlib.h> 
#include <time.h>

typedef struct {
    int iPID;
    int iArrival, iBurst;
    int iStart, iFinish, iWaiting, iResponse, iTaT;
} PCB;

void inputProcess(int n, PCB P[]) {
    srand(time(NULL));
    for(int i = 0; i < n; i++) {
        P[i].iPID = i + 1;
        P[i].iArrival = rand() % 21;      
        P[i].iBurst = (rand() % 11) + 2;
        P[i].iStart = 0;
        P[i].iFinish = 0;
        P[i].iWaiting = 0;
        P[i].iResponse = 0;
        P[i].iTaT = 0;
    }
}

void printProcess(int n, PCB P[]) {
    printf("=============\n");
    for(int i = 0; i < n; i++) {
        printf("P%d (%d, %d)\n", P[i].iPID, P[i].iArrival, P[i].iBurst);
    }
    printf("=============\n");
}

void exportGanttChart(int n, PCB P[]) {
    printf("\nGantt Chart:\n");
    printf("(0)");
    for(int i = 0; i < n; i++) {
        printf(" -- P%d -> (%d)", P[i].iPID, P[i].iFinish);
    }
    printf("\n");
}

void pushProcess(int *n, PCB P[], PCB Q) {
    P[*n] = Q;
    (*n)++;
}

void removeProcess(int *n, int index, PCB P[]) {
    if(index < 0 || index >= *n) return;
    for(int i = index; i < *n-1; i++) {
        P[i] = P[i+1];
    }
    (*n)--;
}

int swapProcess(PCB *P, PCB *Q) {
    PCB temp = *P;
    *P = *Q;
    *Q = temp;
    return 1;
}

int partition(PCB P[], int low, int high, int iCriteria) {
    PCB pivot = P[high];
    int i = low - 1;
    for(int j = low; j < high; j++) {
        int compare;
        if(iCriteria == 1) // Sort by Arrival time
            compare = P[j].iArrival <= pivot.iArrival;
        else // Sort by Burst time
            compare = P[j].iBurst <= pivot.iBurst;
        if(compare) {
            i++;
            swapProcess(&P[i], &P[j]);
        }
    }
    swapProcess(&P[i+1], &P[high]);
    return i+1;
}

void quickSort(PCB P[], int low, int high, int iCriteria) {
    if(low < high) {
        int pi = partition(P, low, high, iCriteria);
        quickSort(P, low, pi-1, iCriteria);
        quickSort(P, pi+1, high, iCriteria);
    }
}

void calculateAWT(int n, PCB P[]) {
    float totalWaiting = 0;
    for(int i = 0; i < n; i++) {
        P[i].iWaiting = P[i].iStart - P[i].iArrival;
        totalWaiting += P[i].iWaiting;
    }
    printf("Average Waiting Time: %.2f\n", totalWaiting/n);
}

void calculateATaT(int n, PCB P[]) {
    float totalTaT = 0;
    for(int i = 0; i < n; i++) {
        P[i].iTaT = P[i].iFinish - P[i].iArrival;
        totalTaT += P[i].iTaT;
    }
    printf("Average Turnaround Time: %.2f\n", totalTaT/n);
}

int main() {
    int n;
    printf("Nhap n: ");
    scanf("%d", &n);

    PCB P[n + 2];
    inputProcess(n, P);
    printProcess(n, P);
    
    quickSort(P, 0, n-1, 1);
    
    //SJF non-preemptive
    PCB executed[n];
    int executedCount = 0;
    int currentTime = P[0].iArrival;
    int completed = 0;
    
    while(completed < n) {
        int minBurst = 9999;
        int minIndex = -1;
        
        for(int i = 0; i < n; i++) {
            int isExecuted = 0;
            for(int j = 0; j < executedCount; j++) {
                if(P[i].iPID == executed[j].iPID) {
                    isExecuted = 1;
                    break;
                }
            }
            if(!isExecuted && P[i].iArrival <= currentTime && P[i].iBurst < minBurst) {
                minBurst = P[i].iBurst;
                minIndex = i;
            }
        }
        
        if(minIndex != -1) {
            P[minIndex].iStart = currentTime;
            P[minIndex].iFinish = P[minIndex].iStart + P[minIndex].iBurst;
            P[minIndex].iResponse = P[minIndex].iStart - P[minIndex].iArrival;
            
            currentTime = P[minIndex].iFinish;
            
            executed[executedCount] = P[minIndex];
            executedCount++;
            completed++;
        } else {
            currentTime++;
        }
    }

    for(int i = 0; i < n; i++) {
        P[i] = executed[i];
    }
    
    exportGanttChart(n, P);
    calculateAWT(n, P);
    calculateATaT(n, P);
    
    return 0;
}