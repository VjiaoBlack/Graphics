#import "trans.h"

// int main(int argc, char* argv[]) {
//     init();
//     return 1;
// }

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
            mat4_set(new, 1, 2, 0 - s);
            mat4_set(new, 2, 1, s);
            mat4_set(new, 2, 2, c);
            break;
        case 'y':
            mat4_set(new, 0, 0, c);
            mat4_set(new, 2, 0, 0 - s);
            mat4_set(new, 2, 0, s);
            mat4_set(new, 2, 2, c);
            break;
        case 'z':
            mat4_set(new, 0, 0, c);
            mat4_set(new, 0, 1, 0 - s);
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

