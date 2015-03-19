/*
 * Sample solution to 3305b assignment #3
 * Takes in the name of a PPM image file on the command-line and
 * prints out the (x1, y1) - (x2, y2) coordinates of the smallest
 * bounding box which does not crop any of the foreground. Pixel
 * (0, 0) is taken to be the background colour.
 *
 * Mike Burrell
 *
 */

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>

static int width, height;

/*
 * bg is the background colour. pic is the pointer to the first
 * pixel in the pixel array
 *
 */
static struct pixel {
	unsigned char r, g, b;
} bg, *pic;

static
struct pixel
pixel_at(int x, int y)
{
	return pic[y * width + x];
}

/*
 * finds x1 (the leftmost boundary). we start with x = width and
 * refine that through each iteration. we make sure to only
 * iterate between the topmost and bottommost boundaries, so
 * those need to be known already.
 */
static
int
find_x1(int y1, int y2)
{
	int w = width;
	int y;
	for (y = y1; y <= y2; y++) {
		int x;
		for (x = 0; x < w; x++) {
			struct pixel p = pixel_at(x, y);
			if (p.r != bg.r || p.g != bg.g || p.b != bg.b) {
				w = x;
				break;
			}
		}
	}
	return w;
}

/*
 * works just like find_x1, but finds the rightmost boundary instead of
 * the leftmost boundary. this should probably be merged with find_x1.
 *
 */
static
int
find_x2(int y1, int y2)
{
	int w = -1;
	int y;
	for (y = y1; y <= y2; y++) {
		int x;
		for (x = width - 1; x > w; x--) {
			struct pixel p = pixel_at(x, y);
			if (p.r != bg.r || p.g != bg.g || p.b != bg.b) {
				w = x;
				break;
			}
		}
	}
	return w;
}

/*
 * finds either the topmost or bottommost boundary. y is the starting
 * y coordinate (y = 0 for the topmost boundary, y = height - 1 for the
 * bottommost). d is the direction to look in (y = 1 means scan
 * downwards; y = -1 means scan upwards)
 *
 */
static
int
find_y(int y, int d)
{
	while (1) {
		int x;
		for (x = 0; x < width; x++) {
			struct pixel p = pixel_at(x, y);
			if (p.r != bg.r || p.g != bg.g || p.b != bg.b)
				return y;
		}
		y += d;
	}
}

/*
 * returns the file size of the file attached to an open file descriptor.
 * I should probably check for errors here
 *
 */
static
unsigned long
fsize(int fd)
{
	struct stat s;
	fstat(fd, &s);
	return s.st_size;
}

int
main(int argc, char **argv)
{
	char *whole_file;
	if (argc != 2) {
		fprintf(stderr, "usage: %s image.pnm\n", argv[0]);
		return 1;
	}
	int fd = open(argv[1], O_RDONLY);
	if (fd < 0) {
		fprintf(stderr, "cannot open '%s': %s\n", argv[1],
			strerror(errno));
		return 1;
	}
	whole_file = mmap(0, fsize(fd), PROT_READ, MAP_PRIVATE, fd, 0);
	char *str = whole_file;
	int filetype, maxval;
	sscanf(str, "P%d", &filetype);
	str = strchr(str, '\n') + 1;
	sscanf(str, "%d %d", &width, &height);
	str = strchr(str, '\n') + 1;
	sscanf(str, "%d", &maxval);
	pic = (struct pixel *)(strchr(str, '\n') + 1);
	bg = pixel_at(0, 0);
	int y1 = find_y(0, 1);
	int y2 = find_y(height - 1, -1);
	printf("(%d, %d) - (%d, %d)\n", find_x1(y1, y2), y1,
		find_x2(y1, y2), y2);
	return 0;
}
