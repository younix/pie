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
 * 
 * Working with relative values confused me, so I worked with absolute ones 
 * instead. Generally this should not be a problem, as the only relative values
 * you need are the chart's centre coordinates and its radius, and these are a
 * linear function of the bounding box size or canvas size. See the sample
 * values for how this could work out.
 */

#define _XOPEN_SOURCE 700

#include <stdio.h>
#include <limits.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define DEG2RAD(a) ((M_PI / 180.0) * (a))

void
usage(void)
{
	fprintf(stderr, "pie [OPTIONS] NUM ...\n"
			"\n"
			"OPTIONS:\n"
			"	-t TITLE	Title of the chart.\n"
			"	-d DESCRIPTION	Description of the chart.\n"
			"	-o FILE		Output file for SVG data.\n");
	exit(EXIT_FAILURE);
}

int
main(int argc, char **argv)
{
	/* colors from the Tango Desktop Projekt */
	char *color[] = {"#8ae234", "#ef2929", "#729fcf", "#ad7fa8", "#fcaf3e",
			 "#4e9a06", "#a40000", "#204a87", "#5c3566", "#ce5c00",
			 "#73d216", "#cc0000", "#3465a4", "#75507b", "#f57900"};
	int max_colors = sizeof(color) / sizeof(color[0]);

	/* canvas size */
	int width = 400;
	int height = 400;

	char *title = "Pie chart";
	char *desc = "Picture of a pie chart";

	int ch, fd;
	while ((ch = getopt(argc, argv, "o:t:d:w:h:")) != -1) {
		switch (ch) {
		case 'o':
			if ((fd = open(optarg, O_WRONLY|O_CREAT, S_IRUSR|S_IWUSR
				|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH)) == -1) {
				perror(optarg);
				exit(EXIT_FAILURE);
			}

			dup2(fd, STDOUT_FILENO);
			close(fd);
			break;
		case 't':
			title = strdup(optarg);
			break;
		case 'd':
			desc = strdup(optarg);
			break;
		case 'w':
			width = strtol(optarg, NULL, 10);
			break;
		case 'h':
			height = strtol(optarg, NULL, 10);
			break;
		default:
			usage();
			/* NOTREACHED */
		}
	}
	argc -= optind;
	argv += optind;

	if (argc > max_colors) {
		fprintf(stderr, "Too mutch data. "
			"Maximum of %d numbers are posibile.\n", max_colors);
		exit(EXIT_FAILURE);
	} else if (argc < 1) {
		fprintf(stderr, "no numbers found.\n");
		usage();
	}

	int data[argc];

	/* centre of the pie chart */
	double centerx = width / 2;
	double centery = height / 2;

	int laf = 0;

	/* radius of the pie chart */
	double radius = MIN(centerx, centery) - 10;
	if (radius < 5.0) {
		fprintf(stderr, "Your chart is too small to draw.\n");
		exit(EXIT_FAILURE);
	}

	/* Draw and output the SVG file. */
	printf( "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n"
		"<svg xmlns:svg=\"http://www.w3.org/2000/svg\"\n"
		"    xmlns=\"http://www.w3.org/2000/svg\" version=\"1.0\"\n"
		"    width=\"%d\" height=\"%d\" id=\"svg2\">\n"
		"\t<title>%s</title>\n"
		"\t<desc>%s</desc>\n",
		width, height, title, desc);

	int max = argc;
	int sum = 0;
	for (int i = 0; i < max; i++) {
		data[i] = strtol(argv[i], NULL, 10);
		if (data[i] < 0) {
			fprintf(stderr, "negative input number\n");
			exit(EXIT_FAILURE);
		}
		sum += data[i];
	}
	double deg = sum / 360.0;	/* one degree */
	double jung = sum / 2.0;	/* necessary to test for arc type */

	/* Data for grid, circle, and slices */ 
	double dx = radius;		/* Starting point: */
	double dy = 0.0;		/* first slice starts in the East */
	double oldangle = 0.0;

	/* Loop through the slices */
	for (int i = 0; i < max; i++) {
		double angle = oldangle + data[i] / deg; /* cumulative angle     */
		double x = cos(DEG2RAD(angle)) * radius; /* x of arc's end point */
		double y = sin(DEG2RAD(angle)) * radius; /* y of arc's end point */

		if (data[i] > jung)
			laf = 1;	/* arc spans more than 180 degrees */
		else
			laf = 0;

		double ax  = centerx + x;	/* absolute x  */
		double ay  = centery + y;	/* absolute y  */
		double adx = centerx + dx;	/* absolute dx */
		double ady = centery + dy;	/* absolute dy */

		printf( "\t<path d=\"M%f,%f"	/* move cursor to center */
			" L%f,%f"		/* draw line away from cursor */
			" A%f,%f 0 %d,1 %f,%f"	/* draw arc */
			" z\""			/* z = close path */
			" fill=\"%s\" stroke=\"#FFFFFF\" stroke-width=\"3\""
			" fill-opacity=\"1.0\" stroke-linejoin=\"round\" />\n",
			centerx, centery, adx, ady, radius, radius, laf, ax, ay,
			color[i]);

		/* old end points become new starting point */
		dx = x; dy = y; oldangle = angle;
	}

	printf("</svg>\n");

	return EXIT_SUCCESS;
}
