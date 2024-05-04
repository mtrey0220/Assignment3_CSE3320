# Assignment 3 - Threads

This was one of my Favorite Assignments from Spring 2024 that helped me understand Threading and Parallel Programming in Operating Systems.

Description:

In order to study parallelism, we must have a problem that will take a significant amount
of computation. We will generate images in the Mandelbrot set a well known fractal
structure. The set is interesting both mathematically and aesthetically because it has an
infinitely recursive structure. You can zoom into any part and find swirls, spirals,
snowflakes, and other fun structures, as long as you are willing to do enough computation.

./mandel -x -0.5 -y 0 -s 2 -o mandel1.bmp
./mandel -x -0.5 -y -0.5 -s 1 -o mandel2.bmp
./mandel -x -0.5 -y -0.5 -s 0.005 -o mandel3.bmp
./mandel -x -0.5 -y -0.5 -s 0.05 -o mandel3.bmp
./mandel -x -0.5 -y -0.5 -s 0.05 -o mandel4.bmp

You will be provided a program that generates images of the Mandelbrot set and saves
them as BMP files. Run make to build the code. If you run the program with no
arguments, it generates a default image and writes it to mandel.bmp. You can see all of
the command line options with mandel -h, and use them to override the defaults. This
program uses the escape time algorithm. For each pixel in the image, it starts
with the x and y position, and then computes a recurrence relation until it exceeds a fixed
value or runs for max iterations.

Then, the pixel is assigned a color according to the number of iterations completed. An
easy color scheme is to assign a gray value proportional to the number of iterations.
The max value controls the amount of work done by the algorithm. If we increase max,
then we can see much more detail in the set, but it may take much longer to compute.
Generally speaking, you need to turn the max value higher as you zoom in. For example,
here is the same area in the set computed with four different values of max:
./mandel -x 0.286932 -y 0.014287 -s .0005 -m 50 -o mandel1.bmp
./mandel -x 0.286932 -y 0.014287 -s .0005 -m 100 -o mandel2.bmp
./mandel -x 0.286932 -y 0.014287 -s .0005 -m 500 -o mandel3.bmp
./mandel -x 0.286932 -y 0.014287 -s .0005 -m 1000 -o mandel4.bmp
./mandel -x 0.286932 -y 0.014287 -s .0005 -m 2000 -o mandel5.bmp

Parallel Programming
What does this all have to do with operating systems? It can take a long time to
compute a Mandelbrot image. The larger the image, the closer it is to the origin, and
the higher the max value, the longer it will take. Suppose that you want to create a
movie of high resolution Mandelbrot images, and it is going to take a long time. Your
job is to speed up the process by using multiple CPUs. You will do this in two different
ways: using multiple processes and using multiple threads.

Find an image
Explore the Mandelbrot space a little bit, and find an interesting area. The more you
zoom in, the more interesting it gets, so try to get -s down to 0.0001 or smaller. Play
around with -m to get the right amount of detail. Find a configuration that takes about
5 seconds to generate on omega. If you find an image that you like, but it only takes a
second or two to create, then increase the size of the image using -W and -H, which will
definitely make it run longer.

Multiple Threads
Instead of running multiple programs at once, we can take a different approach of
making each individual process faster by using multiple threads.
Modify mandel.c to use an arbitrary number of threads to compute the image. Each
thread should compute a completely separate band of the image. For example, if you
specify three threads and the image is 500 pixels high, then thread 0 should work on
lines 0-165, thread 1 should work on lines 166-331, and thread 2 should work on lines
332-499. Add a new command line argument -n to allow the user to specify the number
of threads. If -n is not given, assume a default of one thread. Your modified version of
mandel should work correctly for any arbitrary number of threads and image
configuration. Verify that your modified mandelseries produces the same output as
the original.
