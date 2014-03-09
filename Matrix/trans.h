#import "parse_util.h"
#import "mat4.h"
#import <stdio.h>
#import <stdlib.h>
#import <math.h>

Mat4* ematrix;
Mat4* tmatrix;

int xleft, xright, ybottom, ytop; // of input screen
int cleft, rbottom, cright, rtop; // of output pixel file


void init();
void addline(double,double,double,double,double,double);
Mat4* identity();
void move(double,double,double);
void scale(double, double, double);
void rotate(char, int);
void combine(Mat4*);
void transform();
