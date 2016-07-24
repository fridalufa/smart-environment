#include "slidingwindow.h"

static int initialized = 0;

int WINDOW_SIZE = DEFAULT_WINDOW_SIZE;

int* window = NULL;
int* new_window = NULL;
int record_count = 0;
int current_index = 0;


void initWindow(int size)
{
    if (initialized == 0) {
        WINDOW_SIZE = size;
        window = (int*) malloc(WINDOW_SIZE * sizeof(int));
        if (window != NULL) {
            initialized = 1;
        } else {
            puts("Could not allocate memory for sliding window!\n");
            exit(1);
        }
    } else {
        setWindowSize(size);
    }
}

void destroyWindow(void)
{
    free(window);
}

void setWindowSize(int size)
{
    puts("Resetting window ...\n");
    WINDOW_SIZE = size;

    new_window = (int*) realloc(window, WINDOW_SIZE * sizeof(int));

    if (new_window != NULL) {
        window = new_window;
    } else {
        free(window);
        puts("Could not reallocate memory for sliding window!\n");
        exit(1);
    }
    record_count = 0;
    current_index = 0;

    printf("WINDOWSIZE set to %d\n", size);
}

void addValue(int value)
{
    if (initialized == 0) {
        initWindow(DEFAULT_WINDOW_SIZE);
    }
    if (record_count < WINDOW_SIZE) {
        record_count++;
    }
    if (current_index == WINDOW_SIZE) {
        current_index = 0;
    }

    window[current_index++] = value;
}

int getSum(void)
{
    if (initialized == 0) {
        return 0;
    }
    int i;
    int sum = 0;
    for (i = 0; i < record_count; i++) {
        sum += window[i];
    }

    return sum;
}

int getCount(void)
{
    return record_count;
}

float getAverage(void)
{
    if (record_count > 0) {
        return ((float)getSum()) / getCount();
    } else {
        free(window);
        puts("Division by zero. Add values to window before calling this function!\n");
        exit(1);
    }
}