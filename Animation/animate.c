#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "mat4.h"
#include "trans.h"
#include "parse_util.h"
#include <string.h>
#include "line.h"
#include <ctype.h>

// BUGS:
// sometimes unset (set to 0 as init, but that's it) pixels will suddenly change to big numbers like 32407.
// always the same number.
// they come and go with a different input setup
// i have no idea.
// also, my antialiasing algorithm seems to be slightly off, and can sometimes generate uneveness of lumosity
// for some slanted lines (seems to be those nearest 45 degrees with the most probs, as expected.)

typedef struct variable { // object-orientated style of programming
    char* name;
    double init_value;
    double end_value;
    int init_frame;
    int end_frame;
} variable_t;


int isvisible(double[3], double[3], double[3], double, double, double, int);

int isvarinit;
variable_t** variables;
int numvariables;

int current_frame;
int num_frames;

void swap(int* a, int* b) {
    int placeholder = *a;
    *a = *b;
    *b = placeholder;
}

double getvalue(variable_t* v) {

    if (v == NULL) {
        // printf("NULL\n");
        return 0.0;
    }

    if (current_frame < v->init_frame) 
        return v->init_value;
    if (current_frame > v->end_frame) 
        return v->end_value;

    double range = v->end_value - v->init_value;
    int frames = v->end_frame - v->init_frame; // (the +1 left out for speed purposes)
    // printf("%5.2f : %5.2f |||||\n", v->init_value, v->end_value);
    return v->init_value + (double) (range / (double) frames) * (double) (current_frame - v->init_frame);

}

variable_t* find(char* name) {
    int i;
    printf("init find\n");
    for (i = 0; i < numvariables; i++) {
        printf("currently at %dth place in variables array (out of %d)\n", i, numvariables);
        if (strlen(variables[i]->name) > 0) {
            printf("test2: current: |%s|, finding: |%s|\n", variables[i]->name, name);
            if (strcmp(variables[i]->name,name)==0) {
                printf("abaca ---\n");

                if (variables[i]->init_frame -1 <= current_frame && variables[i]->end_frame-1 >= current_frame){
                    return variables[i];
                    printf("found\n");

                }
            }
            printf("adsf\n");

        }
        printf("?\n");
    }
    printf("notfound\n");
    return NULL;
}

int main(int argc, char* argv[]) {
    isvarinit = 0;
    variables = (variable_t**) malloc(sizeof(variable_t*) * 1);
    variable_t test = (variable_t) {NULL,0.0,0.0,0,0};
    variables[0] = &test;
    numvariables = 0;
    int animated = 0; // default set to not animated
    double delay = 0.0; // delay between frames of animated gif
    int numframes = 0;

    int currentframe = 0;

    // initializes array of storage names.
    storagenames = malloc(sizeof(char*) * 1);
    namedsize = 0;
    storagenames[0] = malloc(sizeof(char) * 1);




    start:; // start label
    tmatrix = identity();
    // printf("%d\n", current_frame);
    // following declaration of atoi needs altering
    int x = 0, y = 0, // standard graphics coordinate system. (origin at top left)
        x1 = 0, y1 = 0, x2 = 0, y2 = 0,
        x3 = 0, y3 = 0, z3 = 0,
        x6 = 0, y6 = 0, z6 = 0,
        render, ii = 0, 
        rx1, ry1, z1, z2, rz1, i, rx2, ry2, rz2, rx, ry, rz, 
        x4 = 0, y4 = 0, z4 = 0, x5 = 0, y5 = 0, z5 = 0;
    int argcounter;
    float sx, sy, sz, rox, roy, roz, mx, my, mz;
    init();
    Mat4* render_holder;
    double sxmin, sxmax, symin, symax,
        reyex, reyey, reyez, leyex, leyey, leyez;
    double p1[3], p2[3], p3[3];
    char* filename = malloc(sizeof(char) * 256);



    if (argc == 1) {
        printf("./objects3d <inputfilename>\n");
        return -1;
    }



    char* inputname = argv[1];
    FILE* input = fopen(inputname, "r");

    char commands[128];



    char** inputargv;
    int inputargc;
    mat4_delete(tmatrix);


    tmatrix = identity();

    while (fgets(commands, 128, input)) {
        inputargv = parse_split(commands);
        inputargc = parse_numwords(inputargv);
        printf("a\n");
        // you need to convert variable names to variable numbers in inputargv.
        for (argcounter = 1; argcounter < inputargc; argcounter++) {
            printf("replacing\n");
            printf("%s\n",(inputargv[0]));
            if (isalpha((char)inputargv[argcounter][0])) { // then it's either a variable name or a transformation name.
                if (strcmp(inputargv[0], "vary") == 0 || 
                    strcmp(inputargv[0], "save") == 0 || 
                    strcmp(inputargv[0], "restore") == 0 || 
                    strcmp(inputargv[0], "files") == 0 || 
                    strcmp(inputargv[0], "file") == 0 || 
                    strcmp(inputargv[0], "#") == 0) {
                    // if it was any of those, do nothing.
                    printf("useles\n"); 
                } else {
                    // replace variable here
                    // printf("can replace variable!~\n");
                    char* newvar = malloc(sizeof(char) * 64);
                    printf("test |%d|\n", argcounter);
                    // printf("test, finding: %s\n",inputargv[argcounter]);
                    variable_t* ab = find(inputargv[argcounter]);
                    printf("test7\n");
                    double a = getvalue(ab);
                    printf("testa\n");
                    sprintf(newvar, "%5.2f", a);
                    printf("test4\n"); 
                    // printf("|%s:%5.2f|=====\n",inputargv[argcounter],getvalue(find(inputargv[argcounter])));
                    inputargv[argcounter] = newvar;
                    printf("test5\n");  
                }

            }
            printf("not alph\n");
        }
        printf("b\n");
        // printf("\n");

        x = y = 0;
        // printf("%s\n", inputargv[0]);

        if (0) {
            // does nothing at all
        } else if (strcmp(inputargv[0],"screen") == 0) {
            sxmin = atof(inputargv[1]);
            symin = atof(inputargv[2]); 
            sxmax = atof(inputargv[3]);
            symax = atof(inputargv[4]);
        } else if (strcmp(inputargv[0],"pixels") == 0) {
            dxmax = atoi(inputargv[1]);
            dymax = atoi(inputargv[2]);
        } else if (strcmp(inputargv[0],"box-t") == 0) {
            box();
            push(tmatrix);
            tmatrix = identity();
            sx = atof(inputargv[1]);
            sy = atof(inputargv[2]);
            sz = atof(inputargv[3]);
            rox = atof(inputargv[4]);
            roy = atof(inputargv[5]);
            roz = atof(inputargv[6]);
            mx = atof(inputargv[7]);
            my = atof(inputargv[8]);
            mz = atof(inputargv[9]);
            scale(sx,sy,sz);
            rotate('x', rox);
            rotate('y', roy);
            rotate('z', roz);
            move(mx,my,mz);
            transform();
            tmatrix = pop();
            transform();
            addcurrentobject();
        } else if (strcmp(inputargv[0],"sphere-t") == 0) { 
            sphere(); 
            push(tmatrix);
            tmatrix = identity();
            sx = atof(inputargv[1]);
            sy = atof(inputargv[2]);
            sz = atof(inputargv[3]);
            rox = atof(inputargv[4]);
            roy = atof(inputargv[5]);
            roz = atof(inputargv[6]);
            mx = atof(inputargv[7]);
            my = atof(inputargv[8]);
            mz = atof(inputargv[9]);
            scale(sx,sy,sz);
            rotate('x', rox);
            rotate('y', roy);
            rotate('z', roz);
            move(mx,my,mz);
            transform();
            tmatrix = pop();
            transform();
            addcurrentobject();
        } else if (strcmp(inputargv[0],"identity") == 0) {
            mat4_delete(tmatrix);
            tmatrix = identity();
        } else if (strcmp(inputargv[0],"move") == 0) {
            printf("asdf\n");
            push(tmatrix);
            tmatrix = identity();
            move(atof(inputargv[1]), atof(inputargv[2]), atof(inputargv[3]));
            tmatrix = mat4_mult(pop(),tmatrix);
            printf("asdf\n");
        } else if (strcmp(inputargv[0],"scale") == 0) {
            push(tmatrix);
            tmatrix = identity();
            scale(atof(inputargv[1]), atof(inputargv[2]), atof(inputargv[3]));
            tmatrix = mat4_mult(pop(),tmatrix);
        } else if (strcmp(inputargv[0],"rotate-x") == 0) {
            push(tmatrix);
            tmatrix = identity();
            rotate('x', atof(inputargv[1]));
            tmatrix = mat4_mult(pop(),tmatrix);
        } else if (strcmp(inputargv[0],"rotate-y") == 0) {
            push(tmatrix);
            tmatrix = identity();
            rotate('y', atof(inputargv[1]));
            tmatrix = mat4_mult(pop(),tmatrix);
        } else if (strcmp(inputargv[0],"rotate-z") == 0) {
            push(tmatrix);
            tmatrix = identity();
            rotate('z', atof(inputargv[1]));
            tmatrix =  mat4_mult(pop(),tmatrix);
        } else if (strcmp(inputargv[0],"push") == 0) {
            push(tmatrix);
        } else if (strcmp(inputargv[0],"pop") == 0) {
            tmatrix = pop();
        } else if (strcmp(inputargv[0],"save") == 0) {
            save(tmatrix, inputargv[1]);
        } else if (strcmp(inputargv[0],"restore") == 0) {
            restore(inputargv[1]);
            print(tmatrix);
        } else if (strcmp(inputargv[0],"render-parallel") == 0) {
            render = 0;
            // stretch 
            render_holder = mat4_copy(tmatrix);
            tmatrix = identity();
            move(0 - sxmin, 0 - symax, 0);
            scale(((double) dxmax) / (sxmax - sxmin), ((double) dymax) / (symax - symin), ((double) dxmax) / (sxmax - sxmin) );
            rendertransform();
            tmatrix = render_holder;
        } else if (strcmp(inputargv[0],"render-perspective-cyclops") == 0) {
            // printf("test\n");

            leyex = reyex = atof(inputargv[1]);
            leyey = reyey = atof(inputargv[2]);
            leyez = reyez = atof(inputargv[3]);
            render = 1;
            // stretch
            render_holder = mat4_copy(tmatrix);
            tmatrix = identity();
            move(0 - sxmin, 0 - symax, 0);
            scale(((double) dxmax) / (sxmax - sxmin), ((double) dymax) / (symax - symin), ((double) dxmax) / (sxmax - sxmin));
            rendertransform();
            tmatrix = render_holder;
        } else if (strcmp(inputargv[0],"render-perspective-stereo") == 0) { 
            reyex = atof(inputargv[1]); 
            reyey = atof(inputargv[2]);
            reyez = atof(inputargv[3]);
            leyex = atof(inputargv[4]);
            leyey = atof(inputargv[5]);
            leyez = atof(inputargv[6]);
            render = 2;
            render_holder = mat4_copy(tmatrix);
            tmatrix = identity();
            move(0 - sxmin, 0 - symax, 0);
            scale(((double) dxmax) / (sxmax - sxmin), ((double) dymax) / (symax - symin), ((double) dxmax) / (sxmax - sxmin));
            rendertransform();
            tmatrix = render_holder;
        } else if (strcmp(inputargv[0],"file") == 0) {
            filename = inputargv[1];
            animated = 0;
        } else if (strcmp(inputargv[0],"files") == 0) {
            animated = 1;
            filename = inputargv[1];
        } else if (strcmp(inputargv[0],"display") == 0) {
            delay = atof(inputargv[1]);
        } else if (strcmp(inputargv[0],"frames") == 0) {
            num_frames = atoi(inputargv[2]);
            // printf("|%s|\n",inputargv[2]);
            // printf("|%d|\n",atoi(inputargv[2]));
            printf("%d curframe | %d maxframe\n", current_frame, num_frames - 1);
        } else if (strcmp(inputargv[0],"vary") == 0) {
            if (isvarinit == 0) { // just checks if vars have already been defined

                // makes variable array longer
                variables = realloc(variables, (++numvariables + 1)* sizeof(variable_t*));


                // creates a bunch of values to put in variable
                char* var_name = malloc(strlen(inputargv[1]) + 1);
                strcpy(var_name,inputargv[1]);
                var_name[strlen(inputargv[1])] = '\0';
                double var_init_value = atof(inputargv[2]);

                double var_end_value = atof(inputargv[3]);
                int var_init_frame = atoi(inputargv[4]);
                int var_end_frame = atoi(inputargv[5]);

                // puts variable into variable array at the end.
                int asdf;


                variables[numvariables - 1] = malloc(sizeof(variable_t) * 1);
                variables[numvariables - 1]->name = malloc(sizeof(char) * strlen(var_name));
                strcpy(variables[numvariables - 1]->name,var_name);
                
                variables[numvariables - 1]->init_value = var_init_value;
                variables[numvariables - 1]->init_frame = var_init_frame;
                variables[numvariables - 1]->end_value = var_end_value;
                variables[numvariables - 1]->end_frame = var_end_frame;

                printf("put |%s| into variables at %d\n", variables[numvariables - 1]->name, numvariables - 1);
                
                // for (asdf = 0; asdf < numvariables; asdf++) 
                //     printf("  this is var at %d: |%s|\n", asdf, variables[asdf]->name);
                // printf("test\n");
            }
        } else if (strcmp(inputargv[0],"#") == 0 || *inputargv[0] == '#') {
            // literally does nothing. It's a comment.
        } else if (strcmp(inputargv[0],"end") == 0) {
            goto draw;
        } 

        /*
        else if (strcmp(inputargv[0],"clear-triangles") == 0) {
            mat4_delete(ematrix);
            ematrix = mat4_create(0);
        } else if (strcmp(inputargv[0],"clear-pixels") == 0) {
            while (y < dymax) {
                while (x < dxmax) {
                    pixels[y*dxmax + x].r = 0;
                    pixels[y*dxmax + x].g = 0;
                    pixels[y*dxmax + x].b = 0;
                    x++;
                }
                x = 0;
                y++;
            }
        } else if (strcmp(inputargv[0],"transform") == 0) {
            transform();
        } 
        */

        else {
            printf("Unknown command encountered: |%s|\n", inputargv[0]);
        }
    }

    printf("\n");

    // from here on, the picture(s) are actually generated and drawn onto the pixel grid.
    draw:;
    isvarinit = 1;
    printf("drawing\n");

    x = 0;
    y = 0;
    pixels = calloc(dxmax * dymax, sizeof(pixel_t));
    while (y < dymax) {
        while (x < dxmax) {
            pixels[y*dxmax + x] = (pixel_t) {0,0,0};
            x++;
        }
        x = 0;
        y++;
    }

    printf("initialized pixel matrix\n");

    ii = 0;
    while (ii < mat4_columns(ematrix)) { 
        // gotta transform the matrix first

        switch (render) {
            case 0: // parallel
                x1 =  (int) mat4_get(ematrix, 0, ii);
                y1 =  0 - (int) mat4_get(ematrix, 1, ii);
                z1 =  (int) mat4_get(ematrix, 2, ii);
                x2 =  (int) mat4_get(ematrix, 0, ii+1);
                y2 =  0 - (int) mat4_get(ematrix, 1, ii+1);
                z2 =  (int) mat4_get(ematrix, 2, ii+1);
                x3 =  (int) mat4_get(ematrix, 0, ii+2);
                y3 =  0 - (int) mat4_get(ematrix, 1, ii+2);
                z3 =  (int) mat4_get(ematrix, 2, ii+2);
                //printf("drawing line %d, %d : %d, %d\n", x1, y1, x2, y2);
                p1[0] = mat4_get(ematrix, 0, ii);
                p1[1] = mat4_get(ematrix, 1, ii);
                p1[2] = mat4_get(ematrix, 2, ii);
                p2[0] = mat4_get(ematrix, 0, ii+1);
                p2[1] = mat4_get(ematrix, 1, ii+1);
                p2[2] = mat4_get(ematrix, 2, ii+1);
                p3[0] = mat4_get(ematrix, 0, ii+2);
                p3[1] = mat4_get(ematrix, 1, ii+2);
                p3[2] = mat4_get(ematrix, 2, ii+2);

                if(isvisible(p1,p2,p3,1,2,3,1)){
                    drawLine(x1, y1, x2, y2,255,255,255);
                    drawLine(x3, y3, x2, y2,255,255,255);
                    drawLine(x3, y3, x1, y1,255,255,255);
                }
                break;
            case 1: // cyclops
                // printf("a cyclops!----------------------------\n");
                rx = (int) ((reyex * dxmax) / (sxmax - sxmin)  + dxmax / 2);
                ry = (int) ((reyey * dymax) / (symax - symin)  + dymax / 2);
                rz = (int) ((reyez * dxmax) / (sxmax - sxmin));

                x1 = rx - (int) mat4_get(ematrix, 0, ii);
                y1 = ry + (int) mat4_get(ematrix, 1, ii);
                z1 =            mat4_get(ematrix, 2, ii);
                x2 = rx - (int) mat4_get(ematrix, 0, ii+1);
                y2 = ry + (int) mat4_get(ematrix, 1, ii+1);
                z2 =            mat4_get(ematrix, 2, ii+1);
                x3 = rx - (int) mat4_get(ematrix, 0, ii+2);
                y3 = ry + (int) mat4_get(ematrix, 1, ii+2);
                z3 =            mat4_get(ematrix, 2, ii+2);

                // these are the points in 3d space
                p1[0] = mat4_get(ematrix, 0, ii);
                p1[1] = mat4_get(ematrix, 1, ii);
                p1[2] = mat4_get(ematrix, 2, ii);
                p2[0] = mat4_get(ematrix, 0, ii+1);
                p2[1] = mat4_get(ematrix, 1, ii+1);
                p2[2] = mat4_get(ematrix, 2, ii+1);
                p3[0] = mat4_get(ematrix, 0, ii+2);
                p3[1] = mat4_get(ematrix, 1, ii+2);
                p3[2] = mat4_get(ematrix, 2, ii+2); 

                x1 = rx - x1 * rz / (rz - z1);
                y1 = ry - y1 * rz / (rz - z1);
                x2 = rx - x2 * rz / (rz - z2);
                y2 = ry - y2 * rz / (rz - z2);
                x3 = rx - x3 * rz / (rz - z3);
                y3 = ry - y3 * rz / (rz - z3);
                //printf("drawing line %d, %d : %d, %d\n", x1, y1, x2, y2);

                // printf("testforvisible\n");
                if (isvisible(p1,p2,p3,rx,0-ry,rz,0)){
                    // printf("result: visible\n");
                    drawLine(x1, y1, x2, y2,255,255,255);
                    drawLine(x3, y3, x2, y2,255,255,255);
                    drawLine(x3, y3, x1, y1,255,255,255);
                }

                // printf("end cyclops!////////////////////\n");
                break;
            case 2: // stereo
                rx1 = (int) ((reyex * dxmax) / (sxmax - sxmin)  + dxmax / 2);
                ry1 = (int) ((reyey * dymax) / (symax - symin)  + dymax / 2);
                rz1 = (int) ((reyez * dxmax) / (sxmax - sxmin));

                x1 = rx1 - (int) mat4_get(ematrix, 0, ii);
                y1 = ry1 + (int) mat4_get(ematrix, 1, ii);
                z1 =             mat4_get(ematrix, 2, ii);
                x2 = rx1 - (int) mat4_get(ematrix, 0, ii+1);
                y2 = ry1 + (int) mat4_get(ematrix, 1, ii+1);
                z2 =             mat4_get(ematrix, 2, ii+1);
                x3 = rx1 - (int) mat4_get(ematrix, 0, ii+2);
                y3 = ry1 + (int) mat4_get(ematrix, 1, ii+2);
                z3 =             mat4_get(ematrix, 2, ii+2);
                p1[0] = mat4_get(ematrix, 0, ii);
                p1[1] = mat4_get(ematrix, 1, ii);
                p1[2] = mat4_get(ematrix, 2, ii);
                p2[0] = mat4_get(ematrix, 0, ii+1);
                p2[1] = mat4_get(ematrix, 1, ii+1);
                p2[2] = mat4_get(ematrix, 2, ii+1);
                p3[0] = mat4_get(ematrix, 0, ii+2);
                p3[1] = mat4_get(ematrix, 1, ii+2);
                p3[2] = mat4_get(ematrix, 2, ii+2);

                x1 = rx1 - x1 * rz1 / (rz1 - z1);
                y1 = ry1 - y1 * rz1 / (rz1 - z1);
                x2 = rx1 - x2 * rz1 / (rz1 - z2);
                y2 = ry1 - y2 * rz1 / (rz1 - z2);
                x3 = rx1 - x2 * rz1 / (rz1 - z3);
                y3 = ry1 - y2 * rz1 / (rz1 - z3);
                //printf("drawing line %d, %d : %d, %d\n", x1, y1, x2, y2);
                if (isvisible(p1,p2,p3,rx1,0-ry1,rz1,0)){
                    drawLine(x1, y1, x2, y2, 255,0,0);
                    drawLine(x3, y3, x2, y2, 255,0,0);
                    drawLine(x3, y3, x1, y1, 255,0,0);
                }
                printf("drawed a triangl\n");



                rx2 = (int) ((leyex * dxmax) / (sxmax - sxmin)  + dxmax / 2);
                ry2 = (int) ((leyey * dymax) / (symax - symin)  + dymax / 2);
                rz2 = (int) ((leyez * dxmax) / (sxmax - sxmin));

                x4 = rx2 - (int) mat4_get(ematrix, 0, ii);
                y4 = ry2 + (int) mat4_get(ematrix, 1, ii);
                z4 =             mat4_get(ematrix, 2, ii);
                x5 = rx2 - (int) mat4_get(ematrix, 0, ii+1);
                y5 = ry2 + (int) mat4_get(ematrix, 1, ii+1);
                z5 =             mat4_get(ematrix, 2, ii+1);
                x6 = rx2 - (int) mat4_get(ematrix, 0, ii+2);
                y6 = ry2 + (int) mat4_get(ematrix, 1, ii+2);
                z6 =             mat4_get(ematrix, 2, ii+2);
                p1[0] = mat4_get(ematrix, 0, ii);
                p1[1] = mat4_get(ematrix, 1, ii);
                p1[2] = mat4_get(ematrix, 2, ii);
                p2[0] = mat4_get(ematrix, 0, ii+1);
                p2[1] = mat4_get(ematrix, 1, ii+1);
                p2[2] = mat4_get(ematrix, 2, ii+1);
                p3[0] = mat4_get(ematrix, 0, ii+2);
                p3[1] = mat4_get(ematrix, 1, ii+2);
                p3[2] = mat4_get(ematrix, 2, ii+2);

                x4 = rx2 - x4 * rz2 / (rz2 - z4);
                y4 = ry2 - y4 * rz2 / (rz2 - z4);
                x5 = rx2 - x5 * rz2 / (rz2 - z5);
                y5 = ry2 - y5 * rz2 / (rz2 - z5);
                x6 = rx2 - x6 * rz2 / (rz2 - z6);
                y6 = ry2 - y6 * rz2 / (rz2 - z6);
                //printf("drawing line %d, %d : %d, %d\n", x3, y3, x4, y4);
                if (isvisible(p1,p2,p3,rx2,0-rz2,ry2,0)){
                    drawLine(x4, y4, x5, y5, 0,180,180);
                    drawLine(x4, y4, x6, y6, 0,180,180);
                    drawLine(x5, y5, x6, y6, 0,180,180);
                }
                printf("drawed a triangl\n");

                break;
        }
        ii += 3;

        x = 0;
        y = 0;
        //if (x1 > dxmax || x2 > dxmax || x2 < 0 || x1 < 0 || y1 > dymax || y2 > dymax || y1 < 0 || y2 < 0) 
            //continue;
        x = y = 0;
    }
    printf("drawed!\n");
    FILE *fp;

    if (!animated) {
        printf("saving to ppm: '%s'\n", filename);
        fp = fopen(filename, "w"); //  initializes file

    }
    else {

        printf("saving to ppms: '%s' from %d to %d", filename, 0, num_frames - 1);
        char* currentfilename = (char*) malloc(sizeof(char) * (strlen(filename) + 10));
        strcpy(currentfilename,filename);
        currentfilename[strlen(filename)]='_';
        currentfilename[strlen(filename)+1]=(char)( ((int)'0')+(current_frame/100) );
        currentfilename[strlen(filename)+2]=(char)( ((int)'0')+((current_frame/10)%10) );
        currentfilename[strlen(filename)+3]=(char)( ((int)'0')+(current_frame%10) );
        currentfilename[strlen(filename)+4]='.';
        currentfilename[strlen(filename)+5]='p';
        currentfilename[strlen(filename)+6]='p';
        currentfilename[strlen(filename)+7]='m';
        currentfilename[strlen(filename)+8]='\0';
        printf("--- %s\n", currentfilename);

        fp = fopen(currentfilename, "w"); //  initializes file

    }

    fputs("P3\n", fp);
    char* sizes = calloc(sizeof(char), 32);
    sprintf(sizes, "%d %d\n", dxmax+1, dymax+1);
    fputs(sizes, fp);
    fputs("255\n", fp);
    char* pixelstr = calloc(sizeof(char), 2048);

    x = y = 0;

    while (y < dymax+1) { // puts pixel information into file.
        while (x < dxmax+1) {
            if (x == dxmax || y == dymax) // for some reason, this works. I have to analyze the pixels storage inisilaliztin.
                sprintf(pixelstr,"0 0 0 ");
            else
                sprintf(pixelstr, "%d %d %d ", pixels(y,x).r, pixels(y,x).g, pixels(y,x).b);
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
    if (++current_frame >= num_frames || numframes == 1)
        return 0;
    else
        goto start;
    
}

int isvisible(double p1[3], double p2[3], double p3[3], double eyex, double eyey, double eyez, int parallel){
    int ans = 0;
    double cx, cy, cz, ex, ey, ez; // vectors and cross product (area vector)
    double a, b, c, d, e, f;
    a = p2[0] - p1[0];
    b = p2[1] - p1[1];
    c = p2[2] - p1[2];
    d = p2[0] - p3[0];
    e = p2[1] - p3[1]; 
    f = p2[2] - p3[2];
    cx = b*f - c*e;
    cy = c*d - a*f;
    cz = a*e - b*d;
    ex = p1[0] - eyex;
    ey = p1[1] - eyey;
    ez = p1[2] - eyez;
    if (!parallel)
        ans = ((p1[0]-eyex) * cx + (p1[1]-eyey) * cy + (p1[2]-eyez) * cz) < 0; // dot prod
    else 
        ans = cz < 0; // TODO: Algorithm somehow takes eyes as ABOVE instead of IN FRONT OF

    // switch (ans) {
    //     case 0:
    //         printf("is not visible\n");
    //         break;

    //     case 1:
    //         printf("is visible\n");
    //         break;

    //     default:
    //         break;
    // }

    return ans;
}

