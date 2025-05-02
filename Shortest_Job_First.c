#include <stdio.h> 
#include <stdlib.h> 
#include <time.h>

// Dinh nghia cau truc PCB cho tien trinh
typedef struct {
    int iPID;           // ID cua tien trinh
    int iArrival, iBurst; // Thoi gian den va thoi gian xu ly
    int iStart, iFinish;  // Thoi gian bat dau va ket thuc
    int iWaiting, iResponse, iTaT; // Thoi gian cho, phan hoi va quay vong
} PCB;

// Khoi tao ngau nhien cac tien trinh
void inputProcess(int n, PCB P[]) {
    srand(time(NULL));
    for(int i = 0; i < n; i++) {
        P[i].iPID = i + 1;              // Gan ID tien trinh
        P[i].iArrival = rand() % 21;    // Thoi gian den ngau nhien (0-20)
        P[i].iBurst = (rand() % 11) + 2; // Thoi gian xu ly ngau nhien (2-12)
        P[i].iStart = 0;                // Khoi tao thoi gian bat dau
        P[i].iFinish = 0;               // Khoi tao thoi gian ket thuc
        P[i].iWaiting = 0;              // Khoi tao thoi gian cho
        P[i].iResponse = 0;             // Khoi tao thoi gian phan hoi
        P[i].iTaT = 0;                  // Khoi tao thoi gian quay vong
    }
}

// In thong tin cac tien trinh
void printProcess(int n, PCB P[]) {
    printf("____________[Process:]____________\n");
    printf("PID\tArrival Time\tBurst Time\n");
    for(int i = 0; i < n; i++) {
        printf("P%d\t    %d\t\t    %d\n", P[i].iPID, P[i].iArrival, P[i].iBurst);
    }
    printf("__________________________________\n");
}

// In bieu do Gantt
void exportGanttChart(int n, PCB P[]) {
    printf("\nGantt Chart:\n");
    printf("0");
    for(int i = 0; i < n; i++) {
        printf("\t%d", P[i].iFinish); // In thoi diem ket thuc
    }
    printf("\n|");
    for(int i = 0; i < n; i++) {
        printf("   P%d\t|", P[i].iPID); // In ten tien trinh
    }
    printf("\n\n");
}

// Them tien trinh vao mang
void pushProcess(int *n, PCB P[], PCB Q) {
    P[*n] = Q; // Them tien trinh Q vao mang
    (*n)++;    // Tang so luong tien trinh
}

// Xoa tien trinh tai vi tri index
void removeProcess(int *n, int index, PCB P[]) {
    if(index < 0 || index >= *n) return; // Kiem tra index hop le
    for(int i = index; i < *n-1; i++) {
        P[i] = P[i+1]; // Dich chuyen cac tien trinh
    }
    (*n)--; // Giam so luong tien trinh
}

// Hoan doi hai tien trinh
int swapProcess(PCB *P, PCB *Q) {
    PCB temp = *P;
    *P = *Q;
    *Q = temp;
    return 1;
}

// Phan vung cho quickSort
int partition(PCB P[], int low, int high, int iCriteria) {
    PCB pivot = P[high]; // Chon pivot la phan tu cuoi
    int i = low - 1;
    for(int j = low; j < high; j++) {
        int compare;
        if(iCriteria == 1) // Sap xep theo thoi gian den
            compare = P[j].iArrival <= pivot.iArrival;
        else // Sap xep theo thoi gian xu ly
            compare = P[j].iBurst <= pivot.iBurst;
        if(compare) {
            i++;
            swapProcess(&P[i], &P[j]); // Hoan doi neu thoa man
        }
    }
    swapProcess(&P[i+1], &P[high]); // Dat pivot vao vi tri dung
    return i+1;
}

// Sap xep quickSort
void quickSort(PCB P[], int low, int high, int iCriteria) {
    if(low < high) {
        int pi = partition(P, low, high, iCriteria); // Phan vung
        quickSort(P, low, pi-1, iCriteria); // Sap xep ben trai
        quickSort(P, pi+1, high, iCriteria); // Sap xep ben phai
    }
}

// Tinh thoi gian cho trung binh
void calculateAWT(int n, PCB P[]) {
    float totalWaiting = 0;
    for(int i = 0; i < n; i++) {
        P[i].iWaiting = P[i].iStart - P[i].iArrival; // Tinh thoi gian cho
        totalWaiting += P[i].iWaiting;
    }
    printf("Average Waiting Time: %.2f\n", totalWaiting/n);
}

// Tinh thoi gian quay vong trung binh
void calculateATaT(int n, PCB P[]) {
    float totalTaT = 0;
    for(int i = 0; i < n; i++) {
        P[i].iTaT = P[i].iFinish - P[i].iArrival; // Tinh thoi gian quay vong
        totalTaT += P[i].iTaT;
    }
    printf("Average Turnaround Time: %.2f\n", totalTaT/n);
}

// Ham chinh
int main() {
    int n;
    printf("Please input number of Process: ");
    scanf("%d", &n); // Nhap so luong tien trinh

    PCB P[n + 2];
    inputProcess(n, P); // Tao ngau nhien cac tien trinh
    printProcess(n, P); // In thong tin tien trinh
    
    quickSort(P, 0, n-1, 1); // Sap xep theo thoi gian den
    
    // Trien khai SJF khong chiem huu
    PCB executed[n]; // Mang luu tien trinh da thuc thi
    int executedCount = 0; // So luong tien trinh da thuc thi
    int currentTime = P[0].iArrival; // Thoi gian bat dau
    int completed = 0; // So luong tien trinh da hoan thanh
    
    while(completed < n) {
        int minBurst = 9999; 
        int minIndex = -1; //
        
        // Tim tien trinh co thoi gian xu ly ngan nhat
        for(int i = 0; i < n; i++) {
            int isExecuted = 0; 
            for(int j = 0; j < executedCount; j++) { 
                if(P[i].iPID == executed[j].iPID) {
                    isExecuted = 1;
                    break;
                }
            }
            if(!isExecuted && P[i].iArrival <= currentTime && P[i].iBurst < minBurst) { // Kiem tra thoi gian den va thoi gian xu ly
                minBurst = P[i].iBurst;
                minIndex = i;
            }
        }
        
        if(minIndex != -1) {
            P[minIndex].iStart = currentTime; // Gan thoi gian bat dau
            P[minIndex].iFinish = P[minIndex].iStart + P[minIndex].iBurst; // Gan thoi gian ket thuc
            P[minIndex].iResponse = P[minIndex].iStart - P[minIndex].iArrival; // Tinh thoi gian phan hoi
            
            currentTime = P[minIndex].iFinish; // Cap nhat thoi gian hien tai
            
            executed[executedCount] = P[minIndex]; // Luu tien trinh da thuc thi
            executedCount++;
            completed++;
        } else {
            currentTime++; // Tang thoi gian neu khong co tien trinh san sang
        }
    }

    for(int i = 0; i < n; i++) {
        P[i] = executed[i]; // Sao chep tien trinh da thuc thi vao mang P
    }
    
    printf("_________[SJF Scheduling]_________\n"); // Sua tieu de cho dung SJF
    exportGanttChart(n, P); // In bieu do Gantt
    calculateAWT(n, P); // Tinh thoi gian cho trung binh
    calculateATaT(n, P); // Tinh thoi gian quay vong trung binh
    
    return 0;
}