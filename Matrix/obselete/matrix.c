#include <stdio.h>
#include <stdlib.h>

// Affine Transformations

int* multiply(int*, int*, int);
int dotproduct(int*, int*, int);
void print(int*, int);



int main(int argc, char* argv[]) {
    int cols = 3;

    int* tmatrix = malloc(sizeof(int*) * 9);
    tmatrix[0] = 1;
    tmatrix[1] = 2;
    tmatrix[2] = 3;
    tmatrix[3] = 4;
    tmatrix[4] = 5;
    tmatrix[5] = 6;
    tmatrix[6] = 7;
    tmatrix[7] = 8;
    tmatrix[8] = 9;

    int* input = malloc(sizeof(int*) * 9);
    input[0] = 2;
    input[1] = 3;
    input[2] = 1;
    input[3] = 4;
    input[4] = 1;
    input[5] = 5;
    input[6] = 6;
    input[7] = 2;
    input[8] = 7;

    int* ans = multiply(tmatrix, input, cols);

    print(ans, cols);

    return 1;
}

int* multiply(int* tmatrix, int* input, int cols) {
    int r = 0, c = 0; // note: we will go through the ans matrix by columns.
    int* trow = malloc(sizeof(int) * 3);
    int* icol = malloc(sizeof(int) * 3);

    int* ans = malloc(sizeof(int) * 3 * cols);

    while (c < cols) {
        while (r < 3) {
            trow[0] = tmatrix[r*3 + 0];
            trow[1] = tmatrix[r*3 + 1];
            trow[2] = tmatrix[r*3 + 2];

            icol[0] = input[0*cols + c];
            icol[1] = input[1*cols + c];
            icol[2] = input[2*cols + c];

            ans[r*cols + c] = dotproduct(trow, icol, 3);
            r++;
        }
        r = 0;
        c++;
    }
    return ans;
}

int dotproduct(int* u, int* v, int len) {
    int ans = 0, i = 0;
    while (i < len) {
        ans += u[i] * v[i];
        i++;
    }
    return ans;
}

void print(int* matrix, int cols) {
    // going through the matrix by rows
    int c = 0, r = 0;
    while (r < 3) {
        while (c < cols) {
            printf("%d ", matrix[r*cols + c]);
            c++;
        }
        r++;
        c = 0;
        printf("\n");
    }
    return;
}
