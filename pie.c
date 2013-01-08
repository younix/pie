/*
 * Adopted from a PHP script written by Branko Collin in 2008.
 * Copyright (c) 2013 Jan Klemkow <j.klemkow@wemelug.de>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

/*
 * Branko Collin:
 * Working with relative values confused me, so I worked with absolute ones 
 * instead. Generally this should not be a problem, as the only relative values
 * you need are the chart's centre coordinates and its radius, and these are a
 * linear function of the bounding box size or canvas size. See the sample
 * values for how this could work out.
 */

#include <stdio.h>
#include <limits.h>
#include <stdlib.h>
#include <math.h>

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define DEG2RAD(a) ((M_PI / 180.0) * (a))

int
main(int argc, char **argv)
{
	int data[argc - 1];

	/* canvas size */
	int width = 400;
	int height = 400;

	/* centre of the pie chart */
	double centerx = width / 2;
	double centery = height / 2;

	double cx = 200.0;
	double cy = 200.0;

	int laf = 0;

	/* radius of the pie chart */
	double radius = MIN(centerx, centery) - 10;
	if (radius < 5.0) {
		fprintf(stderr, "Your chart is too small to draw.\n");
		exit(EXIT_FAILURE);
	}

	/* Draw and output the SVG file. */
	char *title = "Pie chart";
	char *desc = "Picture of a pie chart";

	printf( "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n"
		"<svg xmlns:svg=\"http://www.w3.org/2000/svg\"\n"
		"	xmlns=\"http://www.w3.org/2000/svg\" version=\"1.0\"\n"
		"	width=\"%d\" height=\"%d\" id=\"svg2\">\n"
		"\n"
		"<title>%s</title>\n"
		"<desc>%s</desc>\n",
		width, height, title, desc);

//	char colours = "#4e9a06","#a40000","#204a87","#5c3566","#ce5c00";
//	char colours = "#73d216","#cc0000","#3465a4","#75507b","#f57900";
	char *colors[] = {"#8ae234", "#ef2929", "#729fcf", "#ad7fa8", "#fcaf3e"};

	int max = argc - 1;
	int sum = 0;
	for (int i = 1; i < argc; i++) {
		data[i - 1] = strtol(argv[i], NULL, 10);
		sum += data[i - 1];
	}
	double deg = sum / 360.0;	/* one degree */
	double jung = sum / 2.0;	/* necessary to test for arc type */

	/* Data for grid, circle, and slices */ 
	double dx = radius;	/* Starting point: */
	double dy = 0.0;	/* first slice starts in the East */
	double oldangle = 0.0;

	/* Loop through the slices */
	for (int i = 0; i < max; i++) {
		double angle = oldangle + data[i] / deg; /* cumulative angle     */
		double x = cos(DEG2RAD(angle)) * radius; /* x of arc's end point */
		double y = sin(DEG2RAD(angle)) * radius; /* y of arc's end point */

		char *color = colors[i];

		if (data[i] > jung)
			laf = 1;	/* arc spans more than 180 degrees */
		else
			laf = 0;

		double ax = cx + x;	/* absolute x  */
		double ay = cy + y;	/* absolute y  */
		double adx = cx + dx;	/* absolute dx */
		double ady = cy + dy;	/* absolute dy */

		printf( "<path d=\"M%f,%f"	/* move cursor to center */
			" L%f,%f"		/* draw line away from cursor */
			" A%f,%f 0 %d,1 %f,%f"	/* draw arc */
			" z\""			/* z = close path */
			" fill=\"%s\" stroke=\"#FFFFFF\" stroke-width=\"3\""
			" fill-opacity=\"1.0\" stroke-linejoin=\"round\" />\n",
			cx, cy, adx, ady, radius, radius, laf, ax, ay, color);
		dx = x; // old end points become new starting point
		dy = y; // id.
		oldangle = angle;
	}

	printf("\n</svg>\n");

	return EXIT_SUCCESS;
}
