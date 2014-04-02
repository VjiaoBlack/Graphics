#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#define pixels(y,x) pixels[y*dxmax + x]

typedef struct pixel_t {
    int r;
    int g;
    int b;
} pixel_t;



pixel_t* pixels;
int dxmax;
int dymax;

void drawLine(int, int, int, int, int, int, int);
void swap(int*,int*);
