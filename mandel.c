#include "bitmap.h"

#include <getopt.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>

struct bitmap *bm;
int num_threads;
double xcenter, ycenter, scale;
int max;

int iteration_to_color(int i, int max);
int iterations_at_point(double x, double y, int max);
void *compute_image_wrapper(void *arg);
void compute_image(struct bitmap *bm, double xmin, double xmax, double ymin, double ymax, int max, int num_threads, int thread_id);

void show_help()
{
    printf("Use: mandel [options]\n");
    printf("Where options are:\n");
    printf("-m <max>    The maximum number of iterations per point. (default=1000)\n");
    printf("-x <coord>  X coordinate of image center point. (default=0)\n");
    printf("-y <coord>  Y coordinate of image center point. (default=0)\n");
    printf("-s <scale>  Scale of the image in Mandlebrot coordinates. (default=4)\n");
    printf("-W <pixels> Width of the image in pixels. (default=500)\n");
    printf("-H <pixels> Height of the image in pixels. (default=500)\n");
    printf("-o <file>   Set output file. (default=mandel.bmp)\n");
    printf("-n <threads> The number of threads to use. (default=1)\n");
    printf("-h          Show this help text.\n");
    printf("\nSome examples are:\n");
    printf("mandel -x -0.5 -y -0.5 -s 0.2\n");
    printf("mandel -x -.38 -y -.665 -s .05 -m 100\n");
    printf("mandel -x 0.286932 -y 0.014287 -s .0005 -m 1000\n\n");
}

int main(int argc, char *argv[])
{
    char c;

    // These are the default configuration values used
    // if no command line arguments are given.

    const char *outfile = "mandel.bmp";
    xcenter = 0;
    ycenter = 0;
    scale = 4;
    int image_width = 500;
    int image_height = 500;
    max = 1000;
    num_threads = 1;

    // For each command line argument given,
    // override the appropriate configuration value.

    while ((c = getopt(argc, argv, "x:y:s:W:H:m:o:n:h")) != -1)
    {
        switch (c)
        {
        case 'x':
            xcenter = atof(optarg);
            break;
        case 'y':
            ycenter = atof(optarg);
            break;
        case 's':
            scale = atof(optarg);
            break;
        case 'W':
            image_width = atoi(optarg);
            break;
        case 'H':
            image_height = atoi(optarg);
            break;
        case 'm':
            max = atoi(optarg);
            break;
        case 'o':
            outfile = optarg;
            break;
        case 'n':
            num_threads = atoi(optarg);
            break;
        case 'h':
            show_help();
            exit(1);
            break;
        }
    }

    // Display the configuration of the image.
    printf("mandel: x=%lf y=%lf scale=%lf max=%d outfile=%s num_threads=%d\n", xcenter, ycenter, scale, max, outfile, num_threads);

    // Create a bitmap of the appropriate size.
    bm = bitmap_create(image_width, image_height);

    // Fill it with a dark blue, for debugging
    bitmap_reset(bm, MAKE_RGBA(0, 0, 255, 0));

    // Compute the Mandelbrot image using multiple threads
    pthread_t threads[num_threads];
    for (int i = 0; i < num_threads; i++)
    {
        int *thread_id = malloc(sizeof(int));
        *thread_id = i;
        pthread_create(&threads[i], NULL, compute_image_wrapper, thread_id);
    }

    for (int i = 0; i < num_threads; i++)
    {
        pthread_join(threads[i], NULL);
    }
    // Save the image in the stated file.
    if (!bitmap_save(bm, outfile))
    {
        fprintf(stderr, "mandel: couldn't write to %s: %s\n", outfile, strerror(errno));
        return 1;
    }

    return 0;
}

void *compute_image_wrapper(void *arg)
{
    int *thread_id = (int *)arg;
    compute_image(bm, xcenter - scale, xcenter + scale, ycenter - scale, ycenter + scale, max, num_threads, *thread_id);
    free(thread_id);
    return NULL;
}

/*
Compute an entire Mandelbrot image, writing each point to the given bitmap.
Scale the image to the range (xmin-xmax,ymin-ymax), limiting iterations to "max"
*/

void compute_image(struct bitmap *bm, double xmin, double xmax, double ymin, double ymax, int max, int num_threads, int thread_id)
{
    int i, j;

    int width = bitmap_width(bm);
    int height = bitmap_height(bm);

    int band_height = height / num_threads;
    int start_row = thread_id * band_height;
    int end_row = (thread_id == num_threads - 1) ? height : (thread_id + 1) * band_height;

    // For every pixel in the assigned band...

    for (j = start_row; j < end_row; j++)
    {
        for (i = 0; i < width; i++)
        {
            // Determine the point in x,y space for that pixel.
            double x = xmin + i * (xmax - xmin) / width;
            double y = ymin + j * (ymax - ymin) / height;

            // Compute the iterations at that point.
            int iters = iterations_at_point(x, y, max);

            // Set the pixel in the bitmap.
            bitmap_set(bm, i, j, iters);
        }
    }
}

/*
Return the number of iterations at point x, y
in the Mandelbrot space, up to a maximum of max.
*/

int iterations_at_point(double x, double y, int max)
{
    double x0 = x;
    double y0 = y;

    int iter = 0;

    while ((x * x + y * y <= 4) && iter < max)
    {

        double xt = x * x - y * y + x0;
        double yt = 2 * x * y + y0;

        x = xt;
        y = yt;

        iter++;
    }

    return iteration_to_color(iter, max);
}

/*
Convert a iteration number to an RGBA color.
Here, we just scale to gray with a maximum of imax.
Modify this function to make more interesting colors.
*/

int iteration_to_color(int i, int max)
{
    int gray = 255 * i / max;
    return MAKE_RGBA(gray, gray, gray, 0);
}