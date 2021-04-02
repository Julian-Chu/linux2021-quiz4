#include <math.h>
#include <stdio.h>
#include "threadpool.h"


#define PRECISION 100 /* upper bound in BPP sum */
#define NANOSEC 1e9

/* Use Bailey–Borwein–Plouffe formula to approximate PI */
static void *bpp(void *arg)
{
    int k = *(int *) arg;
    double sum = (4.0 / (8 * k + 1)) - (2.0 / (8 * k + 4)) -
                 (1.0 / (8 * k + 5)) - (1.0 / (8 * k + 6));
    double *product = malloc(sizeof(double));
    if (product)
        *product = 1 / pow(16, k) * sum;
    return (void *) product;
}

static double run(tpool_t pool){
    int bpp_args[PRECISION + 1];
    double bpp_sum = 0;
    tpool_future_t futures[PRECISION + 1];

    for (int i = 0; i <= PRECISION; i++) {
        bpp_args[i] = i;
        futures[i] = tpool_apply(pool, bpp, (void *) &bpp_args[i]);
    }

    for (int i = 0; i <= PRECISION; i++) {
        double *result = tpool_future_get(futures[i], 0 /* blocking wait */);
        bpp_sum += *result;
        tpool_future_destroy(futures[i]);
        free(result);
    }
    return bpp_sum;
}


int main()
{
    const int size = 1000;
    const int size_sampling = 100;
    struct timespec t_start, t_end;
    long long elapsed_time[size];
    double bpp[size];
    tpool_t pool = tpool_create(4);

    for(int i = 0; i < size;i++){
        elapsed_time[i] = 0;
        long long elapsed_time_tmp[size_sampling];
        for(int j = 0; j < size_sampling;j++){
            clock_gettime(CLOCK_MONOTONIC, &t_start);
            bpp[i] = run(pool);
            clock_gettime(CLOCK_MONOTONIC, &t_end);
            elapsed_time_tmp[j] = (t_end.tv_sec * NANOSEC + t_end.tv_nsec) -
                              (t_start.tv_sec * NANOSEC + t_start.tv_nsec);
        }
        long long sum = 0;
        for(int k = 0; k < size_sampling;k++){
            sum += elapsed_time_tmp[k];
        }
        long long mean = sum / size_sampling, sd = 0.0;
        for(int k = 0; k < size_sampling;k++){
           sd += (elapsed_time_tmp[k] - mean) * (elapsed_time_tmp[k] - mean);
        }
        sd = sqrt(sd/(size_sampling - 1));
        int count = 0;
        for(int k=0; k < size_sampling; k++){
            if(elapsed_time_tmp[k] < (mean - 2*sd) && elapsed_time_tmp[k] > (mean + 2*sd)){
                continue;
            }
            elapsed_time[i] +=  elapsed_time_tmp[k];
            count++;
        }
        elapsed_time[i] = (elapsed_time[i]/count);
    }
    tpool_join(pool);

    for (int i =0; i < size; i++) {
        printf("%d %lld  %.15f\n", i, elapsed_time[i], bpp[i]);
    }
    return 0;
}
