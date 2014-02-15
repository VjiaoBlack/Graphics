#include <stdio.h>

int main(int argc, char* argv[]) {
    char stuff[44];
    int hold[21];
    int i, j;

    for (i = 0; i < 21; i++){
        switch(hold[i] % 3) {
            case 0:
                hold[i] = (0 + i/3);
                break;
            case 1: 
                hold[i] = (1 + i/3);
                break;
            case 2: 
                hold[i] = (2 + i/3);
                break;
        }
    }

    for (i = 0; i < 42; i++) {
        if (i % 2 == 0) {
            stuff[i] = '0' + hold[i/2];
        } else {
            stuff[i] = ' ';
        }
    }

    stuff[42] = '\n';
    stuff[43] = '\0';

    FILE *fp = fopen("test1.ppm", "w");

    fputs("P3\n", fp);
    fputs("7 7\n", fp);
    fputs("9\n", fp);

    for (j = 0; j < 7; j++) {
        fputs(stuff, fp);
    }
    
    fclose(fp);
    return 0;
}
