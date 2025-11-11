#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

// structure to store customer data
typedef struct customer {
    int arrival_time;
    struct customer *next;
} customer;

// add a new customer to the queue
void enqueue(customer **front, customer **rear, int time) {
    customer *new_c = malloc(sizeof(customer));
    new_c->arrival_time = time;
    new_c->next = NULL;

    if (*rear == NULL) {
        *front = *rear = new_c;
    } else {
        (*rear)->next = new_c;
        *rear = new_c;
    }
}

// remove a customer from the queue
customer* dequeue(customer **front, customer **rear) {
    if (*front == NULL) return NULL;
    customer *temp = *front;
    *front = (*front)->next;
    if (*front == NULL) *rear = NULL;
    return temp;
}

// generate random number of arrivals using poisson distribution
int poisson(double lambda) {
    double l = exp(-lambda);
    double p = 1.0;
    int k = 0;
    do {
        k++;
        p *= (double)rand() / RAND_MAX;
    } while (p > l);
    return k - 1;
}

// calculate mean
double mean(int arr[], int n) {
    double sum = 0;
    for (int i = 0; i < n; i++)
        sum += arr[i];
    return sum / n;
}

// calculate median
double median(int arr[], int n) {
    // simple bubble sort
    for (int i = 0; i < n - 1; i++)
        for (int j = 0; j < n - i - 1; j++)
            if (arr[j] > arr[j + 1]) {
                int temp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = temp;
            }

    if (n % 2 == 1)
        return arr[n / 2];
    else
        return (arr[n / 2 - 1] + arr[n / 2]) / 2.0;
}

// calculate mode
int mode(int arr[], int n) {
    int maxCount = 0, modeVal = arr[0];
    for (int i = 0; i < n; i++) {
        int count = 1;
        for (int j = i + 1; j < n; j++)
            if (arr[j] == arr[i]) count++;
        if (count > maxCount) {
            maxCount = count;
            modeVal = arr[i];
        }
    }
    return modeVal;
}

// calculate standard deviation
double stddev(int arr[], int n) {
    double m = mean(arr, n), sum = 0;
    for (int i = 0; i < n; i++)
        sum += (arr[i] - m) * (arr[i] - m);
    return sqrt(sum / n);
}

int main() {
    srand(time(NULL));

    double lambda;
    printf("enter average arrival rate per minute (λ): ");
    scanf("%lf", &lambda);

    customer *front = NULL, *rear = NULL;
    int teller_busy = 0;
    int service_left = 0;
    int total = 0;
    int *wait = NULL;

    for (int minute = 0; minute < 480; minute++) {
        int arrivals = poisson(lambda);
        for (int i = 0; i < arrivals; i++)
            enqueue(&front, &rear, minute);

        if (!teller_busy && front != NULL) {
            customer *c = dequeue(&front, &rear);
            int w = minute - c->arrival_time;
            wait = realloc(wait, (total + 1) * sizeof(int));
            wait[total++] = w;
            free(c);
            service_left = 2 + rand() % 2;
            teller_busy = 1;
        }

        if (teller_busy) {
            service_left--;
            if (service_left == 0)
                teller_busy = 0;
        }
    }

    if (total == 0) {
        printf("no customers served.\n");
        return 0;
    }

    int max = wait[0];
    for (int i = 1; i < total; i++)
        if (wait[i] > max)
            max = wait[i];

    double avg = mean(wait, total);
    double med = median(wait, total);
    int mod = mode(wait, total);
    double sd = stddev(wait, total);

    printf("\n--- bank report ---\n");
    printf("total customers served: %d\n", total);
    printf("mean wait time: %.2f minutes\n", avg);
    printf("median wait time: %.2f minutes\n", med);
    printf("mode wait time: %d minutes\n", mod);
    printf("standard deviation of wait times: %.2f minutes\n", sd);
    printf("longest wait time: %d minutes\n", max);

    if (avg > 10)
        printf("manager advice: hire more tellers.\n");
    else
        printf("manager advice: teller count is enough.\n");

    free(wait);
    return 0;
}