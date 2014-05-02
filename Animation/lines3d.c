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

#define MAX(x,y) ( ((x) > (y)) ? (x) : (y) )
#define pixels(y,x) pixels[y*dxmax + x]

typedef struct pixel_t {
    int r;
    int g;
    int b;
} pixel_t;



pixel_t* pixels;
int dxmax;
int dymax;

void drawLine(int, int, int, int, int, int, int);
void swap(int*,int*);

void drawLine(int x1, int y1, int x2, int y2, int ar, int ag, int ab) { // 0 for white, 1 for red, 2 for cyan.
    int acc = 0, delta = 0, x, y, up, right,
        partnerx, partnery;
    float antialias;
    pixel_t partnerpixel;
    int r = ar, g = ag, b = ab;


    if (x1 == x2 && y1 == y2) { // draw pixel if start and end point are same
        pixels(y1,x1) = (pixel_t) {r,g,b};
    } else if (abs(x1 - x2) >= abs(y1 - y2)) { // going in the x dir

        if (abs(x1-x2) == abs(y1-y2)) {
            // printf("color:%d, %d, %d\n", r, g, b);
        }

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
            // printf("testline\n");
            // printf("%d, %d\n", x, y);

            if (acc) { // if there's acc, we need to change partnerp and pixel.
                // the partner of the current pixel is at (x,y), (x,y+1) if DOWN, (x,y-1) if UP. (unless acc = 0)
                if (up && y > 0)
                    partnery = y - 1;
                else if (y < dymax - 1)
                    partnery = y + 1;

                // hold is the max value of color that the new part of the pixel can take (on top of bkgrd color)
                // holdr = abs(r - pixels(y,x).r);
                // holdg = abs(g - pixels(y,x).g);
                // holdb = abs(b - pixels(y,x).b);

                // printf("pre first maxcolor check\n");
                // this following should not be necessary. It's a catch that catches too-high values...
                if (pixels(y,x).r > 255)
                    pixels(y,x).r = 255;
                if (pixels(y,x).g > 255)
                    pixels(y,x).g = 255;
                if (pixels(y,x).b > 255)
                    pixels(y,x).b = 255;
                // printf("first maxcolor check\n");
                antialias = (float) (delta - acc) / (float) (delta); // which pixel is closer linearlly
                antialias = pow(antialias, .70);                     // scales brightness

                // the following sets the pixel's color values to those modified by antialias, and then added back on to their original values.
                pixels(y,x).r = MAX((int) ( antialias * (float) r),pixels(y,x).r);
                pixels(y,x).g = MAX((int) ( antialias * (float) g),pixels(y,x).g);
                pixels(y,x).b = MAX((int) ( antialias * (float) b),pixels(y,x).b);
                if (pixels(y,x).r > 255)
                    pixels(y,x).r = 255;
                if (pixels(y,x).g > 255)
                    pixels(y,x).g = 255;
                if (pixels(y,x).b > 255)
                    pixels(y,x).b = 255;

                // this following should not be necessary, It's a catch that catches too-high values...
                if (pixels(partnery,partnerx).r > 255)
                    pixels(partnery,partnerx).r = 255;
                if (pixels(partnery,partnerx).g > 255)
                    pixels(partnery,partnerx).g = 255;
                if (pixels(partnery,partnerx).b > 255)
                    pixels(partnery,partnerx).b = 255;

                // printf("testline\n");
                // this sets the hold to themax color values the new pixel can have.
                // holdr = abs(r - pixels(partnery, partnerx).r);
                // holdg = abs(g - pixels(partnery, partnerx).g);
                // holdb = abs(b - pixels(partnery, partnerx).b);

                // these sets antialias values.
                antialias = (float) (acc) / (float) (delta);
                antialias = pow(antialias, .70);

                partnerpixel = (pixel_t) {r,g,b}; // makes a pixel_t in partnerpixel

                // the following sets the partnerpxiel to antialiased color, plus the original background color.
                partnerpixel.r = MAX((int) ( antialias * (float) r) , pixels(partnery,partnerx).r);
                partnerpixel.g = MAX((int) ( antialias * (float) g) , pixels(partnery,partnerx).g);
                partnerpixel.b = MAX((int) ( antialias * (float) b) , pixels(partnery,partnerx).b);

                pixels(partnery,partnerx).r = partnerpixel.r;
                pixels(partnery,partnerx).g = partnerpixel.g;
                pixels(partnery,partnerx).b = partnerpixel.b;
                if (pixels(partnery,partnerx).r > 255)
                    pixels(partnery,partnerx).r = 255;
                if (pixels(partnery,partnerx).g > 255)
                    pixels(partnery,partnerx).g = 255;
                if (pixels(partnery,partnerx).b > 255)
                    pixels(partnery,partnerx).b = 255;
            } else {

                if (pixels(y,x).r < r) {
                    pixels(y,x).r = r;
                }
                pixels(y,x).g = (pixels(y,x).g > g) ? pixels(y,x).g : g;
                pixels(y,x).b = (pixels(y,x).b > b) ? pixels(y,x).b : b;
                if (pixels(y,x).r > 255)
                    pixels(y,x).r = 255;
                if (pixels(y,x).g > 255)
                    pixels(y,x).g = 255;
                if (pixels(y,x).b > 255)
                    pixels(y,x).b = 255;
            }
            

            // aliased 
            // pixels(y,x).r = r;
            // pixels(y,x).g = g;
            // pixels(y,x).b = b;
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
            // printf("%d, %d\n", x, y);

            if (acc) {  // sets up correct partner pixel coords
                if (right && x < dxmax - 1)
                    partnerx = x + 1;
                else if (x > 0)
                    partnerx = x - 1;
                if (pixels(y,x).r > 255)
                    pixels(y,x).r = 255;
                if (pixels(y,x).g > 255)
                    pixels(y,x).g = 255;
                if (pixels(y,x).b > 255)
                    pixels(y,x).b = 255;
                // holdr = abs(r - pixels(y,x).r);
                // holdg = abs(g - pixels(y,x).g);
                // holdb = abs(b - pixels(y,x).b);

                antialias = (float) (delta - acc) / (float) (delta);
                antialias = pow(antialias, .70);

                pixels(y,x).r = MAX((int) ( antialias * (float) r) , pixels(y,x).r); // can this possibly give us the wrong value?
                pixels(y,x).g = MAX((int) ( antialias * (float) g) , pixels(y,x).g);
                pixels(y,x).b = MAX((int) ( antialias * (float) b) , pixels(y,x).b);
                if (pixels(y,x).r > 255)
                    pixels(y,x).r = 255;
                if (pixels(y,x).g > 255)
                    pixels(y,x).g = 255;
                if (pixels(y,x).b > 255)
                    pixels(y,x).b = 255;
               
                if (pixels(partnery,partnerx).r > 255)
                    pixels(partnery,partnerx).r = 255;
                if (pixels(partnery,partnerx).g > 255)
                    pixels(partnery,partnerx).g = 255;
                if (pixels(partnery,partnerx).b > 255)
                    pixels(partnery,partnerx).b = 255;

                // holdr = abs(r - pixels(partnery,partnerx).r);
                // holdg = abs(g - pixels(partnery,partnerx).g);
                // holdb = abs(b - pixels(partnery,partnerx).b);

                antialias = (float) (acc) / (float) (delta);
                antialias = pow(antialias, .70);
                partnerpixel = (pixel_t) {r,g,b};
                partnerpixel.r = MAX((int) ( antialias * (float) r) , pixels(partnery, partnerx).r);
                partnerpixel.g = MAX((int) ( antialias * (float) g) , pixels(partnery, partnerx).g);
                partnerpixel.b = MAX((int) ( antialias * (float) b) , pixels(partnery, partnerx).b);

                pixels(partnery,partnerx).r = partnerpixel.r;
                pixels(partnery,partnerx).g = partnerpixel.g;
                pixels(partnery,partnerx).b = partnerpixel.b;
                if (pixels(partnery,partnerx).r > 255)
                    pixels(partnery,partnerx).r = 255;
                if (pixels(partnery,partnerx).g > 255)
                    pixels(partnery,partnerx).g = 255;
                if (pixels(partnery,partnerx).b > 255)
                    pixels(partnery,partnerx).b = 255;
            } else {
                if (pixels(y,x).r < r) {
                    pixels(y,x).r = r;
                }
                pixels(y,x).g = (pixels(y,x).g > g) ? pixels(y,x).g : g;
                pixels(y,x).b = (pixels(y,x).b > b) ? pixels(y,x).b : b;
                if (pixels(y,x).r > 255)
                    pixels(y,x).r = 255;
                if (pixels(y,x).g > 255)
                    pixels(y,x).g = 255;
                if (pixels(y,x).b > 255)
                    pixels(y,x).b = 255;
            }

            // aliased
            // pixels(y,x).r = r;
            // pixels(y,x).g = g;
            // pixels(y,x).b = b;


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

    
