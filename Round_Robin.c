#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX_SIZE 100

typedef struct {
    int iPID;
    int iArrival, iBurst;
    int iStart, iFinish;
    int iWaiting, iResponse, iTaT;
} PCB;

// Cấu trúc hàng đợi vòng
typedef struct {
    PCB items[MAX_SIZE];
    int front;
    int rear;
    int count;
} Queue;

void initQueue(Queue *q) {
    q->front = 0;
    q->rear = -1;
    q->count = 0;
}

int isEmpty(Queue *q) {
    return q->count == 0;
}

int isFull(Queue *q) {
    return q->count == MAX_SIZE;
}

void enqueue(Queue *q, PCB p) {
    if (isFull(q)) {
        printf("Queue is full!\n");
        return;
    }
    q->rear = (q->rear + 1) % MAX_SIZE;
    q->items[q->rear] = p;
    q->count++;
}

PCB dequeue(Queue *q) {
    if (isEmpty(q)) {
        printf("Queue is empty!\n");
        PCB empty = {0};
        return empty;
    }
    PCB p = q->items[q->front];
    q->front = (q->front + 1) % MAX_SIZE;
    q->count--;
    return p;
}

// Khởi tạo ngẫu nhiên các tiến trình
void inputProcess(int n, PCB P[]) {
    srand(time(NULL));
    for (int i = 0; i < n; i++) {
        P[i].iPID = i + 1;
        P[i].iArrival = rand() % 21;    // Thời gian đến ngẫu nhiên (0-20)
        P[i].iBurst = (rand() % 11) + 2; // Thời gian xử lý ngẫu nhiên (2-12)
        P[i].iStart = -1;
        P[i].iFinish = 0;
        P[i].iWaiting = 0;
        P[i].iResponse = -1;
        P[i].iTaT = 0;
    }
}

// In thông tin các tiến trình
void printProcess(int n, PCB P[]) {
    printf("____________[Process:]____________\n");
    printf("PID\tArrival Time\tBurst Time\n");
    for (int i = 0; i < n; i++) {
        printf("P%d\t    %d\t\t    %d\n", P[i].iPID, P[i].iArrival, P[i].iBurst);
    }
    printf("__________________________________\n");
}

// In biểu đồ Gantt
void exportGanttChart(int executedCount, PCB executed[]) {
    printf("\nGantt Chart:\n");
    int currentTime = 0;
    printf("%d", currentTime);
    for (int i = 0; i < executedCount; i++) {
        if (executed[i].iFinish > currentTime) {
            printf("\t%d", executed[i].iFinish);
            currentTime = executed[i].iFinish;
        }
    }
    printf("\n|");
    for (int i = 0; i < executedCount; i++) {
        printf("   P%d\t|", executed[i].iPID);
    }
    printf("\n\n");
}

// Hoán đổi hai tiến trình
int swapProcess(PCB *P, PCB *Q) {
    PCB temp = *P;
    *P = *Q;
    *Q = temp;
    return 1;
}

// Phân vùng cho quickSort
int partition(PCB P[], int low, int high, int iCriteria) {
    PCB pivot = P[high];
    int i = low - 1;
    for (int j = low; j < high; j++) {
        int compare;
        if (iCriteria == 1) // Sắp xếp theo thời gian đến
            compare = P[j].iArrival <= pivot.iArrival;
        else // Sắp xếp theo thời gian xử lý
            compare = P[j].iBurst <= pivot.iBurst;
        if (compare) {
            i++;
            swapProcess(&P[i], &P[j]);
        }
    }
    swapProcess(&P[i + 1], &P[high]);
    return i + 1;
}

// Sắp xếp quickSort
void quickSort(PCB P[], int low, int high, int iCriteria) {
    if (low < high) {
        int pi = partition(P, low, high, iCriteria);
        quickSort(P, low, pi - 1, iCriteria);
        quickSort(P, pi + 1, high, iCriteria);
    }
}

// Tính thời gian chờ trung bình
void calculateAWT(int n, PCB P[], int currentTime) {
    float totalWaiting = 0;
    for (int i = 0; i < n; i++) {
        P[i].iWaiting = P[i].iFinish - P[i].iBurst - P[i].iArrival;
        if (P[i].iWaiting < 0) P[i].iWaiting = 0;
        totalWaiting += P[i].iWaiting;
    }
    printf("Average Waiting Time: %.2f\n", totalWaiting / n);
}

// Tính thời gian quay vòng trung bình
void calculateATaT(int n, PCB P[]) {
    float totalTaT = 0;
    for (int i = 0; i < n; i++) {
        P[i].iTaT = P[i].iFinish - P[i].iArrival;
        totalTaT += P[i].iTaT;
    }
    printf("Average Turnaround Time: %.2f\n", totalTaT / n);
}

// Hàm chính
int main() {
    int n, quantum;
    printf("Please input number of Process: ");
    scanf("%d", &n);
    printf("Please input time quantum: ");
    scanf("%d", &quantum);

    PCB P[n];
    inputProcess(n, P);
    printProcess(n, P);

    quickSort(P, 0, n - 1, 1); // Sắp xếp theo thời gian đến

    // Triển khai Round Robin
    int remainingBurst[n];
    for (int i = 0; i < n; i++) {
        remainingBurst[i] = P[i].iBurst;
    }

    Queue q;
    initQueue(&q);
    PCB executed[n * 10];
    int executedCount = 0;
    int currentTime = 0;
    int completed = 0;
    int i = 0;
    PCB current;
    int index = -1;

    while (completed < n) {
        // Thêm tất cả tiến trình đã đến vào hàng đợi
        while (i < n && P[i].iArrival <= currentTime) {
            enqueue(&q, P[i]);
            i++;
        }
        if(index != -1 && remainingBurst[index] > 0) {
            enqueue(&q, current); // Đưa lại vào hàng đợi nếu chưa hoàn thành
        }
        
        // Xử lý hàng đợi
        if (!isEmpty(&q)) {
            current = dequeue(&q);
            index = -1;
            for (int j = 0; j < n; j++) {
                if (P[j].iPID == current.iPID) {
                    index = j;
                    break;
                }
            }

            // Gán thời gian bắt đầu và phản hồi lần đầu
            if (P[index].iStart == -1) {
                P[index].iStart = currentTime;
                P[index].iResponse = currentTime - P[index].iArrival;
            }

            // Thực thi trong quantum hoặc đến khi hoàn thành
            int execTime = (remainingBurst[index] <= quantum) ? remainingBurst[index] : quantum;
            remainingBurst[index] -= execTime;
            currentTime += execTime;

            // Cập nhật tiến trình đã thực thi
            current.iFinish = currentTime;
            executed[executedCount] = current;
            executedCount++;

            // Nếu chưa hoàn thành, thêm lại vào cuối hàng đợi
            if (remainingBurst[index] == 0) {
                // Nếu hoàn thành
                P[index].iFinish = currentTime;
                completed++;
            }
        } else {
            currentTime++; // Tăng thời gian nếu không có tiến trình sẵn sàng
        }
    }

    printf("________[Round Robin Scheduling:]________\n");
    exportGanttChart(executedCount, executed);
    calculateAWT(n, P, currentTime);
    calculateATaT(n, P);

    return 0;
}