#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "mat4.h"
#include "trans.h"
#include "parse_util.h"
#include <string.h>

// BUGS:
// sometimes unset (set to 0 as init, but that's it) pixels will suddenly change to big numbers like 32407.
// always the same number.
// they come and go with a different input setup
// i have no idea.
// also, my antialiasing algorithm seems to be slightly off, and can sometimes generate uneveness of lumosity
// for some slanted lines (seems to be those nearest 45 degrees with the most probs, as expected.)

#define pixels(y,x) pixels[y*dxmax + x]
int* pixels;
int dxmax;
int dymax;

void drawLine(int, int, int, int);
void swap(int*,int*);

void swap(int* a, int* b) {
    int placeholder = *a;
    *a = *b;
    *b = placeholder;
}

void drawLine(int x1, int y1, int x2, int y2) {
    int acc = 0, delta = 0, x, y, up, right,
        partnerx, partnery, partnerpixel, hold;
    float antialias;

    if (x1 == x2 && y1 == y2) { // draw pixel if start and end point are same
        pixels(y1,x1) = 255;
    } else if (abs(x1 - x2) >= abs(y1 - y2)) { // going in the x dir

        if (x1 > x2) { // swaps points so you're going to the right
            swap(&x1, &x2);
            swap(&y1, &y2);
        }

        delta = abs(x2 - x1); // initialize delta and pos values
        x = x1;
        y = y1;
        up = (y2 < y1);             

        while (x <= x2) { // This is the drawing function 
            partnerx = x, partnery = y;
            if (acc) { // if there's acc, we need to change partnerp and pixel.
                // the partner of the current pixel is at (x,y), (x,y+1) if DOWN, (x,y-1) if UP. (unless acc = 0)
                if (up && y > 0)
                    partnery = y - 1;
                else if (y < dymax)
                    partnery = y + 1;



                hold = 255 - pixels(y,x);

                if (pixels(y,x) > 255)
                    pixels(y,x) = 255;
                antialias = (float) (delta - acc) / (float) (delta); // which pixel is closer linearlly
                antialias = pow(antialias, .70);                    // scales brightness
                pixels(y,x) = (int) ( antialias * (float) hold) + pixels(y,x);

                if (x == 304 && y == 264) {
                    printf("test: %d at %d, %d\n", pixels(y,x), x, y);
                }

                if (pixels(partnery,partnerx) > 255)
                    pixels(partnery,partnerx) = 255;
                hold = 255 - pixels(partnery, partnerx);
                if (pixels(partnery,partnerx) > 255) {
                    printf("failure at %d, %d with %d\n", partnerx, partnery, pixels(partnery,partnerx));
                }
                antialias = (float) (acc) / (float) (delta);
                antialias = pow(antialias, .70);
                partnerpixel = (int) ( antialias * (float) hold) + pixels(partnery,partnerx);

                if (partnerx == 304 && partnery == 264) {
                    printf("ptest: %d at %d, %d\n", pixels(partnery,partnerx), partnerx, partnery);
                    printf("  data: hold,%d; anti,%f;\n", hold, antialias);
                }
            } else {
                pixels(y,x) = 255;
            }
            pixels(partnery,partnerx) = partnerpixel;

            acc += abs(y2 - y1);
            if (acc >= delta) {
                acc -= delta;
                if (up)
                    y--;
                else 
                    y++;
            } 
            x++;
        }   

    } else if (abs(x1 - x2) < abs(y1 - y2)) { // going in the y dir 

        if (y1 > y2) { // swaps points so that you're going down
            swap(&x1, &x2);
            swap(&y1, &y2);
        }

        delta = abs(y2 - y1); // setup delta, pos, and variables
        x = x1;
        y = y1;
        right = (x2 > x1);

        while (y <= y2) { // drawing function
            partnerx = x, partnery = y;

            if (acc) {  // sets up correct partner pixel coords
                if (right && x < 499)
                    partnerx = x + 1;
                else if (x > 0)
                    partnerx = x - 1;

                if (pixels(y,x) > 255)
                pixels(y,x) = 255;
                hold = 255 - pixels(y,x);

                antialias = (float) (delta - acc) / (float) (delta);
                antialias = pow(antialias, .70);
                pixels(y,x) = (int) ( antialias * (float) hold) + pixels(y,x);

                if (y == 264 && (x == 304))
                    printf("test: %d at %d, %d\n", pixels(y,x), x, y);
               
                if (pixels(partnery,partnerx) > 255)
                pixels(partnery,partnerx) = 255;
                hold = 255 - pixels(partnery,partnerx);
                antialias = (float) (acc) / (float) (delta);
                antialias = pow(antialias, .70);
                partnerpixel = (int) ( antialias * (float) hold) + pixels(partnery, partnerx);
            } else {
                pixels(y,x) = 255; // if the pixel is perfectly aligned with the liene to draw
            }

            pixels(partnery,partnerx) = partnerpixel;

            acc += abs(x2 - x1);
            if (acc >= delta) {
                acc -= delta;
                if (right)
                    x++;
                else 
                    x--;
            } 
            y++;
        }   

    }
}

int main(int argc, char* argv[]) {
    // following declaration of atoi needs altering
    int x = 0, y = 0, // standard graphics coordinate system. (origin at top left)
        x1 = 0, y1 = 0, x2 = 0, y2 = 0,
        render, ii = 0, 
        rx, ry, z1, z2, rz, i;
    init();
    Mat4* render_holder;
    double sxmin, sxmax, symin, symax,
        reyex, reyey, reyez, leyex, leyey, leyez;
    char* filename = malloc(sizeof(char) * 256);
    if (argc == 1) {
        printf("./lines3d <inputfilename>\n");
        return -1;
    }

    char* inputname = argv[1];
    FILE* input = fopen(inputname, "r");

    char commands[128];

    // char** inputargv = calloc(7, sizeof(char*));
    // for (i = 0; i < 7; i++) {
    //     inputargv[i] = calloc(64, sizeof(char));
    // }
    char** inputargv;
    int inputargc;

    while (fgets(commands, 128, input)) {
        inputargv = parse_split(commands);
        inputargc = parse_numwords(inputargv);
        x = y = 0;

        printf("%s", inputargv[0]);

        if (strcmp(inputargv[0],"#") == 0 || *inputargv[0] == '#') {
            printf(" comment:");
            for (i = 1; i < inputargc; i++) {
                printf(" %s", inputargv[i]);
            }
        } else if (strcmp(inputargv[0],"line") == 0) {
            addline(atof(inputargv[1]), atof(inputargv[2]), atof(inputargv[3]), atof(inputargv[4]), atof(inputargv[5]), atof(inputargv[6]));
            printf(": %5.2f, %5.2f, %5.2f : %5.2f, %5.2f, %5.2f", atof(inputargv[1]), atof(inputargv[2]), atof(inputargv[3]), atof(inputargv[4]), atof(inputargv[5]), atof(inputargv[6]));
        } else if (strcmp(inputargv[0],"sphere") == 0) {
            printf(": SPHERE DRAWING NOT IMPLEMENTED");
            // not implemented
        } else if (strcmp(inputargv[0],"identity") == 0) {
            mat4_delete(tmatrix);
            tmatrix = identity();
        } else if (strcmp(inputargv[0],"move") == 0) {
            move(atof(inputargv[1]), atof(inputargv[2]), atof(inputargv[3]));
        } else if (strcmp(inputargv[0],"scale") == 0) {
            scale(atof(inputargv[1]), atof(inputargv[2]), atof(inputargv[3]));
        } else if (strcmp(inputargv[0],"rotate-x") == 0) {
            rotate('x', atof(inputargv[1]));
        } else if (strcmp(inputargv[0],"rotate-y") == 0) {
            rotate('y', atof(inputargv[1]));
        } else if (strcmp(inputargv[0],"rotate-z") == 0) {
            rotate('z', atof(inputargv[1]));
        } else if (strcmp(inputargv[0],"screen") == 0) {
            sxmin = atof(inputargv[1]);
            symin = atof(inputargv[2]);
            sxmax = atof(inputargv[3]);
            symax = atof(inputargv[4]);
        } else if (strcmp(inputargv[0],"pixels") == 0) {
            dxmax = atoi(inputargv[1]);
            dymax = atoi(inputargv[2]);
        } else if (strcmp(inputargv[0],"transform") == 0) {
            transform();
        } else if (strcmp(inputargv[0],"render-perspective-cyclops") == 0) {
            leyex = reyex = atof(inputargv[1]);
            leyey = reyey = atof(inputargv[2]);
            leyez = reyez = atof(inputargv[3]);
            render = 1;
            // stretch
            render_holder = mat4_copy(tmatrix);
            tmatrix = identity();
            move(0 - sxmin, 0 - symax, 0);
            scale(((double) dxmax) / (sxmax - sxmin), ((double) dymax) / (symax - symin), ((double) dxmax) / (sxmax - sxmin));
            transform();
            tmatrix = render_holder;
        } else if (strcmp(inputargv[0],"render-perspective-stereo") == 0) { // note: you need to draw twice for this. Make drawing its own function.
            reyex = atof(inputargv[1]); // NOTE: not yet implemented
            reyey = atof(inputargv[2]);
            reyez = atof(inputargv[3]);
            leyex = atof(inputargv[4]);
            leyey = atof(inputargv[5]);
            leyez = atof(inputargv[6]);
            render = 2;
            // stretch
            render_holder = mat4_copy(tmatrix);
            tmatrix = identity();
            move(0 - sxmin, 0 - symax, 0);
            scale(((double) dxmax) / (sxmax - sxmin), ((double) dymax) / (symax - symin), ((double) dxmax) / (sxmax - sxmin));
            transform();
            tmatrix = render_holder;
        } else if (strcmp(inputargv[0],"render-parallel") == 0) {
            render = 0;
            // stretch 
            render_holder = mat4_copy(tmatrix);
            tmatrix = identity();
            move(0 - sxmin, 0 - symax, 0);
            scale(((double) dxmax) / (sxmax - sxmin), ((double) dymax) / (symax - symin), 0);
            transform();
            tmatrix = render_holder;
        } else if (strcmp(inputargv[0],"file") == 0) {
            filename = inputargv[1];
        } else if (strcmp(inputargv[0],"end") == 0) {
            printf(" read from input...\n");
            goto draw;
        } else {
            printf("Unknown command encountered: |%s|\n", inputargv[0]);
        }

        printf("\n");
    }

    // drawinggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggg

    draw: printf("drawing...\n");

    x = 0;
    y = 0;
    pixels = calloc(dxmax * dymax, sizeof(int));
    while (y < dymax) {
        while (x < dxmax) {
            pixels[y*dxmax + x] = 0;
            x++;
        }
        x = 0;
        y++;
    }

    ii = 0;
    while (ii < mat4_columns(ematrix)) {  
        // gotta transform the matrix first
        switch (render) {
            case 0: // parallel
                x1 = (int) mat4_get(ematrix, 0, ii);
                y1 = 0 - (int) mat4_get(ematrix, 1, ii);
                x2 = (int) mat4_get(ematrix, 0, ii+1);
                y2 = 0 - (int) mat4_get(ematrix, 1, ii+1);
                break;
            case 1: // cyclops
                rx = (int) ((reyex * dxmax) / (sxmax - sxmin)  + dxmax / 2);
                ry = (int) ((reyey * dymax) / (symax - symin)  + dymax / 2);
                rz = (int) ((reyez * dxmax) / (sxmax - sxmin));

                x1 = rx - (int) mat4_get(ematrix, 0, ii);
                y1 = ry + (int) mat4_get(ematrix, 1, ii);
                z1 = mat4_get(ematrix, 2, ii);
                x2 = rx - (int) mat4_get(ematrix, 0, ii+1);
                y2 = ry + (int) mat4_get(ematrix, 1, ii+1);
                z2 = mat4_get(ematrix, 2, ii+1);

                x1 = rx - x1 * rz / (rz - z1);
                y1 = ry - y1 * rz / (rz - z1);
                x2 = rx - x2 * rz / (rz - z2);
                y2 = ry - y2 * rz / (rz - z2);

                break;
            case 2: // stereo
                x1 = (int) ( (mat4_get(ematrix, 0, ii) * reyez) / (mat4_get(ematrix, 2, ii) * reyez) );
                y1 = 0 - (int) ( (mat4_get(ematrix, 1, ii) * reyez) / (mat4_get(ematrix, 2, ii) * reyez) );
                x2 = (int) ( (mat4_get(ematrix, 0, ii+1) * reyez) / (mat4_get(ematrix, 2, ii+1) * reyez) );
                y2 = 0 - (int) ( (mat4_get(ematrix, 1, ii+1) * reyez) / (mat4_get(ematrix, 2, ii+1) * reyez) );
                // following does not yet work
                // x3 = (int) ( (mat4_get(ematrix, 0, ii) * leyez) / (mat4_get(ematrix, 2, ii) * leyez);
                // y3 = (int) ( (mat4_get(ematrix, 1, ii) * leyez) / (mat4_get(ematrix, 2, ii) * leyez);
                // x4 = (int) ( (mat4_get(ematrix, 0, ii+1) * leyez) / (mat4_get(ematrix, 2, ii+1) * leyez);
                // y4 = (int) ( (mat4_get(ematrix, 1, ii+1) * leyez) / (mat4_get(ematrix, 2, ii+1) * leyez);
                break;
        }
        ii += 2;

        x = 0;
        y = 0;
        //if (x1 > dxmax || x2 > dxmax || x2 < 0 || x1 < 0 || y1 > dymax || y2 > dymax || y1 < 0 || y2 < 0) 
            //continue;
        printf("drawing line %d, %d : %d, %d\n", x1, y1, x2, y2);
        drawLine(x1, y1, x2, y2);
        x = y = 0;
    }

    printf("saving to ppm...\n");

    FILE *fp = fopen(filename, "w"); //  initializes file
    fputs("P3\n", fp);
    char* sizes = calloc(sizeof(char), 32);
    sprintf(sizes, "%d %d\n", dxmax+1, dymax+1);
    fputs(sizes, fp);
    fputs("255\n", fp);
    char* pixelstr = calloc(sizeof(char), 2048);

    x = y = 0;

    while (y < dymax+1) { // puts pixel information into file.
        while (x < dxmax+1) {
            sprintf(pixelstr, "%d %d %d ", pixels(y,x), pixels(y,x), pixels(y,x));
            fputs(pixelstr, fp);
            fflush(fp);
            x++;
        }
        fputs("\n", fp);    
        x = 0;
        y++;
    }

    fputc(' ', fp); 
    fclose(fp);
    return 0;
    
}




