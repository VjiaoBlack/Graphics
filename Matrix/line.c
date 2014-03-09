#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "mat4.h"
#include "trans.h"
#include "parse_util.h"
#include <string.h>

#define pixels(y,x) pixels[y*dxmax + x]

int main(int argc, char* argv[]) {
    // following declaration of atoi needs altering
    int x = 0, y = 0, // standard graphics coordinate system. (origin at top left)
        placeholder, acc = 0, delta,
        up, right, x1 = 0, y1 = 0, x2 = 0, y2 = 0,
        dxmax, dymax, render, ii = 0, 
        antialias;
    init();
    double sxmin, sxmax, symin, symax;
    char* filename = malloc(sizeof(char) * 256);
    if (argc == 1) {
        printf("./trans <inputfilename>\n");
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

        if (strcmp(inputargv[0],"#") == 0) {
        } else if (strcmp(inputargv[0],"line") == 0) {
            addline(atof(inputargv[1]), atof(inputargv[2]), atof(inputargv[3]), atof(inputargv[4]), atof(inputargv[5]), atof(inputargv[6]));
            printf(": %f, %f, %f : %f, %f, %f", atof(inputargv[1]), atof(inputargv[2]), atof(inputargv[3]), atof(inputargv[4]), atof(inputargv[5]), atof(inputargv[6]));
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
            render = 1;
        } else if (strcmp(inputargv[0],"render-perspective-stereo") == 0) {
            render = 2;
        } else if (strcmp(inputargv[0],"render-parallel") == 0) {
            // should call some render-parallel function? not sure.
            render = 0;
        } else if (strcmp(inputargv[0],"file") == 0) {
            filename = inputargv[1];
        } else if (strcmp(inputargv[0],"end") == 0) {
            goto draw;
        } else {
            printf("Unknown command encountered: |%s|\n", inputargv[0]);
        }

        printf("\n");
    }

    draw: printf("drawing...\n");

    x = 0;
    y = 0;
    int* pixels = malloc(sizeof(int) * dxmax * dymax);
    while (y < dymax) {
        while (x < dxmax) {
            pixels[y*dxmax + x] = 0;
            x++;
        }
        x = 0;
        y++;
    }

    ii = 0;

    while (ii < mat4_columns(ematrix)) {  // WARNING: this is now a broken loop. What you SHOULD do is to make this run through the edgematrix and project and draw all lines.
        // gotta transform the matrix first
        switch (render) {
            case 0:
            case 1:
            case 2:
                x1 = (int) mat4_get(ematrix, 0, ii);
                y1 = (int) mat4_get(ematrix, 1, ii);
                x2 = (int) mat4_get(ematrix, 0, ii+1);
                y2 = (int) mat4_get(ematrix, 1, ii+1);
                break;
        }
        ii += 2;
        
        printf("drawing line %d, %d : %d, %d\n", x1, y1, x2, y2);
        if (x1 == x2 && y1 == y2) { // start and end point are same 
            pixels(y1,x1) = 255;
        } else if (abs(x1 - x2) >= abs(y1 - y2)) { // going in the x dir

            if (x1 > x2) { // swaps points so you're going to the right
                placeholder = x2;
                x2 = x1;
                x1 = placeholder;
                placeholder = y2;
                y2 = y1;
                y1 = placeholder;
            }
            delta = abs(x2 - x1);
            x = x1;
            y = y1;
            up = (y2 < y1);

            /* 
                note that in the bresenham, the acc value holds the amount of accumulation before the pixel switches
                x-cor. In the antialiased version, the acc value should be used (by seeing how close it is to delta)
                to calculate how dark the two pixels in each pixel-pair should be. 
                Again, like the bresenham, this is much more of an approximation than anything else.
                Note that for perfectly diagonal lines, the algorithm will produce about the same result.
            */                

            while (x <= x2) { // This is the drawing function 
                pixels(y,x) = 255;
        
                // the partner of the current pixel is at (x,y), (x,y+1) if DOWN, (x,y-1) if UP. (unless acc = 0)
                int partnerx = x, partnery = y;


                int partnerpixel = pixels(partnery,x);
                if (acc) {
                    if (up && y > 0)
                        partnery = y - 1;
                    else if (y < dymax)
                        partnery = y + 1;

                    partnerpixel = 255;

                    antialias = (float) (delta - acc) / (float) (delta);
                    antialias = pow(antialias, .75);
                    pixels(y,x) = (int) ( (float) pixels(y,x) * antialias);

                    antialias = (float) (acc) / (float) (delta);
                    antialias = pow(antialias, .75);
                    partnerpixel = (int) ( (float) partnerpixel * antialias);
                }

                pixels(partnery,partnerx) = 255; //antialias (disabled)

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
                placeholder = x2;
                x2 = x1;
                x1 = placeholder;
                placeholder = y2;
                y2 = y1;
                y1 = placeholder;
            }
            delta = abs(y2 - y1);
            x = x1;
            y = y1;

            right = (x2 > x1);

            while (y <= y2) { 
                pixels(y,x) = 255; 


                int partnerx = x, partnery = y;

                int partnerpixel = pixels(y,partnerx);

                if (acc) {
                    if (right && x < 499)
                        partnerx = x + 1;
                    else if (x > 0)
                        partnerx = x - 1;

                    partnerpixel = 255;

                    antialias = (float) (delta - acc) / (float) (delta);
                    antialias = pow(antialias, .75);
                    pixels(y,x) = (int) ( (float) pixels(y,x) * antialias);

                    antialias = (float) (acc) / (float) (delta);
                    antialias = pow(antialias, .75);
                    partnerpixel = (int) ( (float) partnerpixel * antialias);
                }

                pixels(partnery,partnerx) = 255; //antialias (disabled)

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

        x = y = 0;
    }

    FILE *fp = fopen(filename, "w"); //this needs to be fixed
    fputs("P3\n", fp);
    char* sizes = calloc(sizeof(char), 32);
    sprintf(sizes, "%d %d\n", dxmax+1, dymax+1);
    fputs(sizes, fp);
    fputs("255\n", fp);
    char* pixelstr = calloc(sizeof(char), 2048);

    x = y = 0;
    while (y < dymax+1) {
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




