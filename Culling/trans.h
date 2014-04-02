#import "parse_util.h"
#import "mat4.h"
#import <stdio.h>
#import <stdlib.h>
#import <math.h>

Mat4* ematrix; // sets of 3 columns: define trangles to draw.
Mat4* omatrix;
Mat4* tmatrix;

Mat4** tstack;
int stacksize;

int xleft, xright, ybottom, ytop; // of input screen
int cleft, rbottom, cright, rtop; // of output pixel file


void init();
// void addline(double,double,double,double,double,double);
void addtriangle(double,double,double,double,double,double,double,double,double);
void addcurrentobject();
Mat4* identity();
void move(double,double,double);
void scale(double, double, double);
void rotate(char, int);
void combine(Mat4*);
void transform();
void rendertransform();
void sphere();
void box();
double deg_to_rad(int);

void push(Mat4*);
Mat4* pop();
