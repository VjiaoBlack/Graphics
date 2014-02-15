#include <stdio.h>
#include <stdlib.h>
#include <math.h>

/*
struct pixel_t {
    int r;
    int g; 
    int b;
};
typedef struct pixel_t pixel_t;
*/

#define CANVAS_SIZE 500
#define PI 3.14159264

int main(int argc, char* argv[]) { // (inner_radius, outer_radius, precision)
    if (argc != 4) {
        printf("./antialiascircle <inner_radius> <outer_radius> <precision>\n");
        printf("Note that all arguments are necessary.\n");
        return 1;
    }
    // init code ##################################################################################
    int //center_x = atoi(argv[1]),   //   pow(radius_inner, 2)
        //center_y = atoi(argv[2]),   // < pow((x - center_x), 2) + pow((y - center_y), 2)
        inner_radius = atoi(argv[1]),     // < pow(radius_outer, 2)
        r, g, b, i = 0, // i is for the pos in samples
        precision = atoi(argv[3]),
        outer_radius = atoi(argv[2]),
        sumr = 0, sumg = 0, sumb = 0;
    double x = 0 - CANVAS_SIZE / 2, y = CANVAS_SIZE / 2, theta;
    int samplesr[precision*precision];
    int samplesg[precision*precision];
    int samplesb[precision*precision];
    char* output_pixel_string = malloc(sizeof(char) * 15);
    FILE *fp = fopen("antialiascircle.ppm", "w");
    fputs("P3\n", fp);
    fputs("500 500\n", fp);
    fputs("255\n", fp);

    while (y > 0 - CANVAS_SIZE / 2) {
        while (x < CANVAS_SIZE / 2) {
            while (i < precision*precision) { // (x + (double)(i%precision)/precision) is the modified variable
                double antix = (x + (double)(i%precision)/precision);
                double antiy = (y + (double)(i/precision)/precision);
                if (antix*antix + antiy*antiy <= inner_radius*inner_radius){
                    samplesr[i] = (abs(inner_radius - antix))% 255;
                    samplesg[i] = (abs(inner_radius - antiy))% 255;
                    samplesb[i] = (abs(inner_radius - (antix + antiy) / 2))% 255;
                } else if (antix*antix + antiy*antiy >= outer_radius*outer_radius){
                    samplesg[i] = (int) pow(antix / 8.0,2) % 130;
                    samplesr[i] = ((int) pow(fabs(antiy),0.95 * log(fabs(antix))))% 255;
                    samplesb[i] = ((int) floor(fabs(5*(antix+antiy))))% 255;
                } else {
                    samplesg[i] = (3 * (int)sqrt(0-(antix*antiy))) % 255;
                    samplesb[i] = (3 * (int)sqrt(antix*antiy)) % 255;
                    samplesr[i] = 100 - (3 * (int)sqrt(antix*antiy) % 255 + 3 * (int)sqrt(0-(antix*antiy)) % 255) % 100;
                }

                if (antix*antix + antiy*antiy <= 400) {
                    samplesg[i] = samplesr[i] = samplesb[i] = 0;
                } 
                sumr += samplesr[i];
                sumg += samplesg[i];
                sumb += samplesb[i];
                i++;
            }
            r = sumr / (precision*precision);
            g = sumg / (precision*precision);
            b = sumb / (precision*precision);
            sprintf(output_pixel_string, "%d %d %d\t", r, g, b);
            fputs(output_pixel_string, fp);
            x++;
            i = 0;
            sumr = sumg = sumb = 0;
        }
        fputc('\n', fp);
        y--;
        x = 0 - CANVAS_SIZE / 2;
    }
    fputc(' ', fp);
    fputc(EOF, fp);

    fclose(fp);
    return 0;
}
