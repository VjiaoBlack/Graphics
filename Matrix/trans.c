#import "trans.h"

void combine(Mat4* new) {
    tmatrix = mat4_mult(new, tmatrix);
}

void init() { // initializes tmatrix to default value and ematrix.
    tmatrix = identity();

    ematrix = mat4_create(0);
    return;
}

void addline(double x1, double y1, double z1, double x2, double y2, double z2) {
    double* p1 = malloc(sizeof(double) * 4);
    double* p2 = malloc(sizeof(double) * 4);

    p1[0] = x1;
    p1[1] = y1;
    p1[2] = z1;
    p1[3] = 1;
    p2[0] = x2;
    p2[1] = y2;
    p2[2] = z2;
    p2[3] = 1;

    mat4_add_column(ematrix, p1);
    mat4_add_column(ematrix, p2);

    return;
}

Mat4* identity() {
    Mat4* matrix= mat4_create(4);
    mat4_set(matrix, 0, 0, 1);
    mat4_set(matrix, 1, 1, 1);
    mat4_set(matrix, 2, 2, 1);
    mat4_set(matrix, 3, 3, 1);
    return matrix;
}

void move(double x, double y, double z) {
    Mat4* new = identity();

    mat4_set(new, 0, 3, x);
    mat4_set(new, 1, 3, y);
    mat4_set(new, 2, 3, z);

    combine(new);
    return;
}

void scale(double x, double y, double z) {
    Mat4* new = identity();

    mat4_set(new, 0, 0, x);
    mat4_set(new, 1, 1, y);
    mat4_set(new, 2, 2, z);

    combine(new);

    return;
}

void rotate(char axis, int degrees) {
    double radians = degrees * M_PI / 180;
    double c = cos(radians);
    double s = sin(radians);
    Mat4* new = identity();

    switch(axis) {
        case 'x':
            mat4_set(new, 1, 1, c);
            mat4_set(new, 1, 2, 0.0 - s);
            mat4_set(new, 2, 1, s);
            mat4_set(new, 2, 2, c);
            break;
        case 'y':
            mat4_set(new, 0, 0, c);
            mat4_set(new, 2, 0, 0.0 - s);
            mat4_set(new, 0, 2, s);
            mat4_set(new, 2, 2, c);
            break;
        case 'z':
            mat4_set(new, 0, 0, c);
            mat4_set(new, 0, 1, 0.0 - s);
            mat4_set(new, 1, 0, s);
            mat4_set(new, 1, 1, c);
            break;
    }

    combine(new);
    return;
}

void transform() {
    ematrix = mat4_mult(tmatrix, ematrix);
    return;
}

void sphere(double r, double cx, double cy, double cz) {
    int theta; // angle from x axis
    int phi; // angle from z axis
    double x1, y1, z1, x2, y2, z2;
    double t, p;


    for (theta = 0; theta < 180; theta += 15) { // vertical circle (longitude) rotates around z axis.
        for (phi = 0; phi <= 360; phi += 15) { // the point goes from top of the cricle, around, back to the top.
            // set p 2 to current
            t = deg_to_rad(theta);
            p = deg_to_rad(phi);
            x2 = r * sin(p) * cos(t) + cx;
            y2 = r * sin(p) * sin(t) + cy;
            z2 = r * cos(p) + cz;

            // if not beginning, put points in ematrix
            if (phi != 0) {
                addline(x1, y1, z1, x2, y2, z2);
                printf("%f  %f  %f: %f   %f   %f   \n",x1,y1,z1,x2,y2,z2);
            }

            // set p 1
            x1 = r * sin(p) * cos(t) + cx;
            y1 = r * sin(p) * sin(t) + cy;
            z1 = r * cos(p) + cz;

            
        }
    }
    
    for (phi = 0; phi < 180; phi += 15) { // draw sphere horizontal slice
        for (theta = 0; theta <= 360; theta += 15) { // the point goes from top of the cricle, around, back to the top.
            // set p 2 to current
            t = deg_to_rad(theta);
            p = deg_to_rad(phi);
            x2 = r * sin(p) * cos(t) + cx;
            y2 = r * sin(p) * sin(t) + cy;
            z2 = r * cos(p) + cz;

            // if both, put points in ematrix
            if (theta != 0) {
                addline(x1, y1, z1, x2, y2, z2);
            }

            // set p 1
            x1 = r * sin(p) * cos(t) + cx;
            y1 = r * sin(p) * sin(t) + cy;
            z1 = r * cos(p) + cz;

            
        }
    }
}

double deg_to_rad(int deg) {
    return (double) ((deg * M_PI) / 180);
}

