// Name: Aneesha Dasari and Veena Nalluri
// VUnetID: dasaria, nallurv
// Email: aneesha.dasari@vanderbilt.edu, veena.nalluri@vanderbilt.edu
// Class: CS 3281 - Vanderbilt University
// Description: Creates a bank account ledger that uses the two commit protocol for distributed consensus.
// This supports upto 5 instances of this program using sockets for communication between them.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// CHANGE accordingly
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
    printf("-n <num_threads> Set the number of threads used to create image. (default=1)\n");
    printf("-h          Show this help text.\n");
    printf("\nSome examples are:\n");
    printf("mandel -x -0.5 -y -0.5 -s 0.2\n");
    printf("mandel -x -.38 -y -.665 -s .05 -m 100\n");
    printf("mandel -x 0.286932 -y 0.014287 -s .0005 -m 1000\n\n");
}

// Change accordingly
int main(int argc, char* argv[])
{
    char c;

    // These are the default configuration values used
    // if no command line arguments are given.

    const char* outfile = "mandel.bmp";
    double xcenter = 0;
    double ycenter = 0;
    double scale = 4;
    int image_width = 500;
    int image_height = 500;
    int max = 1000;
    int num_threads = 1; //default of one thread

    // For each command line argument given,
    // override the appropriate configuration value.

    while ((c = getopt(argc, argv, "x:y:s:W:H:m:o:n:h")) != -1) {
        switch (c) {
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
    printf("mandel: x=%lf y=%lf scale=%lf max=%d threads=%d outfile=%s\n", xcenter, ycenter, scale, max, num_threads, outfile);

    // Create a bitmap of the appropriate size.
    struct bitmap* bm = bitmap_create(image_width, image_height);

    // Fill it with a dark blue, for debugging
    bitmap_reset(bm, MAKE_RGBA(0, 0, 255, 0));

    int width = bitmap_width(bm);
    int height = bitmap_height(bm);

    // Compute the Mandelbrot image
    struct thread_args arr_structs[num_threads];
    pthread_t arr_threads[num_threads];
    int start = 0;
    double num_per_thread = height/num_threads;

    // Create array of thread_args to feed into threads
    for (int i = 0; i < num_threads; i++)
    {
	if (i == num_threads-1)
        {
	    struct thread_args s1 = {bm, xcenter-scale, xcenter+scale, ycenter-scale, ycenter+scale, start, height, max};
	    arr_structs[i] = s1;
	}
        else
	{
	    struct thread_args s1 = {bm, xcenter-scale, xcenter+scale, ycenter-scale, ycenter+scale, start, start+num_per_thread, max};
	    arr_structs[i] = s1;
	}        
	start = start+num_per_thread;
     }

    // Create threads
    for (int i = 0; i < num_threads; i++)
    {
	pthread_create(&arr_threads[i], NULL, (void *)compute_image, &arr_structs[i]);
    }

    // Join threads
    for (int i = 0; i < num_threads; i++)
    {
	pthread_join(arr_threads[i], NULL);
    }
    

    // Save the image in the stated file.
    if (!bitmap_save(bm, outfile)) {
        fprintf(stderr, "mandel: couldn't write to %s: %s\n", outfile, strerror(errno));
        return 1;
    }

    return 0;
}