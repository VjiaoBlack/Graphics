#import "parse_util.h"
#import "mat4.h"
#import <stdio.h>
#import <stdlib.h>
#import <math.h>
#import <string.h>

Mat4* ematrix; // sets of 3 columns: define trangles to draw.
Mat4* omatrix;
Mat4* tmatrix;

Mat4** tstack;
int stacksize;

// this will be an array that matches index positions of tmatricies to index position of their names.
// this is also an example of data orientated programming
Mat4** namedtdict;
int namedsize;

char** storagenames;

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

int searchtrans(char* name);
void save(Mat4* mat, char* name);
void restore(char* name);
