#ifndef SMART_ENV_SLIDING_WINDOW
#define SMART_ENV_SLIDING_WINDOW

#include <stdio.h>
#include <stdlib.h>

#define DEFAULT_WINDOW_SIZE (10)

void destroyWindow(void);
void setWindowSize(int size);
void addValue(int value);
int getSum(void);
int getCount(void);
float getAverage(void);

#endif