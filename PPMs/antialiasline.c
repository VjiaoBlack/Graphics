#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define CANVAS_SIZE 500

struct pixel_t {
    int r;
    int g; 
    int b;
};
typedef struct pixel_t pixel_t;

char* pixeltostring(pixel_t);

int main(int argc, char* argv[]) {
    // following declaration of atoi needs altering
    int x = 0, y = 0, // standard graphics coordinate system. (origin at top left)
        placeholder, acc, delta, func,
        up, right, r = 255, g = 255, b = 255,
        x1 = 0, y1 = 0, x2 = 0, y2 = 0;
    float antialias;
    char* filename = malloc(sizeof(char) * 256);
    char* line = malloc(sizeof(char) * 1001); //throwaway. /donest work, fml
    
    FILE* input = fopen("input.txt", "r");

    pixel_t pixels[CANVAS_SIZE][CANVAS_SIZE];

    while (y < 500) {
        while (x < 500) {
            pixels[y][x] = (pixel_t) {0,0,0};
            x++;
        }
        y++;
        x = 0;
    }

    while ( (func = fgetc(input)) != 'q') {
        x = y = 0;
        switch (func) {
            case '#':
                fgets(line, 1000, input);
                printf("THrowaway line: %s\n", line);
                goto endofdrawline;
                break;
            case 'c': // should use fgets
                fscanf(input,"\n%d %d %d\n", &r, &g, &b);
                printf("Input colors, %d, %d, %d\n", r, g, b);
                goto endofdrawline;
                break;
            case 'l':
                fscanf(input,"\n%d %d %d %d\n", &x1, &y1, &x2, &y2);
                break;
            case 'g':
                fscanf(input,"\n%s\n", filename);
                printf("Naming file: %s\n", filename);
                goto endofdrawline;
                break;
            default:
                printf("Default, idk\n");
                goto endofdrawline;
                break;
        }
        printf("drawing line %d, %d : %d, %d with %d %d %d\n", x1, y1, x2, y2, r, g, b);


        if (x1 == x2 && y1 == y2) { // start and end point are same 
            pixels[y1][x1] = (pixel_t) {r, g, b};
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
                pixels[y][x] = (pixel_t) {r, g, b};
        
                // the partner of the current pixel is at (x,y), (x,y+1) if DOWN, (x,y-1) if UP. (unless acc = 0)
                int partnerx = x, partnery = y;


                pixel_t partnerpixel = (pixel_t) {pixels[partnery][x].r, pixels[partnery][x].g, pixels[partnery][x].b};
                if (acc) {
                    if (up && y > 0)
                        partnery = y - 1;
                    else if (y < 499)
                        partnery = y + 1;

                    partnerpixel.r = r;
                    partnerpixel.g = g;
                    partnerpixel.b = b;

                    antialias = (float) (delta - acc) / (float) (delta);
                    antialias = pow(antialias, .75);
                    pixels[y][x].r = (int) ( (float) pixels[y][x].r * antialias);
                    pixels[y][x].g = (int) ( (float) pixels[y][x].g * antialias);
                    pixels[y][x].b = (int) ( (float) pixels[y][x].b * antialias);

                    antialias = (float) (acc) / (float) (delta);
                    antialias = pow(antialias, .75);
                    partnerpixel.r = (int) ( (float) partnerpixel.r * antialias);
                    partnerpixel.g = (int) ( (float) partnerpixel.g * antialias);
                    partnerpixel.b = (int) ( (float) partnerpixel.b * antialias);
                }

                pixels[partnery][partnerx] = partnerpixel;

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
                pixels[y][x] = (pixel_t) {r, g, b}; 


                int partnerx = x, partnery = y;

                pixel_t partnerpixel = (pixel_t) {pixels[y][partnerx].r, pixels[y][partnerx].g, pixels[y][partnerx].b};

                if (acc) {
                    if (right && x < 499)
                        partnerx = x + 1;
                    else if (x > 0)
                        partnerx = x - 1;

                    partnerpixel.r = r;
                    partnerpixel.g = g;
                    partnerpixel.b = b;

                    antialias = (float) (delta - acc) / (float) (delta);
                    antialias = pow(antialias, .99);
                    pixels[y][x].r = (int) ( (float) pixels[y][x].r * antialias);
                    pixels[y][x].g = (int) ( (float) pixels[y][x].g * antialias);
                    pixels[y][x].b = (int) ( (float) pixels[y][x].b * antialias);

                    antialias = (float) (acc) / (float) (delta);
                    antialias = pow(antialias, .75);
                    partnerpixel.r = (int) ( (float) partnerpixel.r * antialias);
                    partnerpixel.g = (int) ( (float) partnerpixel.g * antialias);
                    partnerpixel.b = (int) ( (float) partnerpixel.b * antialias);
                }

                pixels[partnery][partnerx] = partnerpixel;

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
        endofdrawline: x = 0;//printf("jumped to end?\n");
    }

    FILE *fp = fopen(filename, "w"); //this needs to be fixed
    fputs("P3\n", fp);
    fputs("500 500\n", fp);
    fputs("255\n", fp);

    x = y = 0;
    while (y < 500) {
        while (x < 500) {
            fputs(pixeltostring(pixels[y][x]), fp);
            x++;
        }
        fputs("\n", fp);
        x = 0;
        y++;
    }

    fputc(' ', fp);
    fputc(EOF, fp);

    fclose(fp);
    return 0;
    
}

char* pixeltostring(pixel_t pixel) {
    char* string = malloc(sizeof(char) * 15);
    sprintf(string, "%d %d %d ", pixel.r, pixel.g, pixel.b);  
    return string;  
}







