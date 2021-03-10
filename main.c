#include <stdlib.h>
#include <math.h>
#include <errno.h>
#include <limits.h>

#include "graphics.h"
#include "universe.h"

int
main(int argc, char **argv)
{
	char *endptr;
	SDL_Event event;
	struct graphics* graphics;
	int cols = 42, rows = 42;
	long n = 0;
	struct universe* universe;
	struct universe* nextuniverse;
	int quit = 0, lasttime = 0, mousex, mousey, clickedsqx, clickedsqy, paused = 0;
	int fps = 10;

	switch (argc) {
	case 2:
		n = strtol(argv[1], &endptr, 0);
		if (errno == ERANGE || n > INT_MAX) {
			fprintf(stderr, "error: number of columns is too large.\n");
			return EXIT_FAILURE;
		} else if (endptr == argv[1] || n < 1) {
			fprintf(stderr, "error: invalid number of columns.\n");
			return EXIT_FAILURE;
		}
		cols = (int) n;
		rows = cols;
		break;
	case 1:
		cols = 42;
		rows = 42;
		break;
	default:
		fprintf(stderr, "error: invalid arguments.\n");
		return EXIT_FAILURE;
	}

	universe = genuniverse(cols, rows);
	graphics = initgphs(universe);
	lasttime = SDL_GetTicks();
	while (!quit)
	{
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
			case SDL_QUIT:
				quit = 1;
				break;
			case SDL_MOUSEBUTTONDOWN:
				SDL_GetMouseState(&mousex, &mousey);
				/* Here I use double for extra precision in click. */
				clickedsqx = (int) floor((double) mousex / (double) graphics->gluniverse->sqwidth);
				clickedsqy = (int) floor((double) mousey / (double) graphics->gluniverse->sqheight);
				universe->space[clickedsqx][clickedsqy] = (char) !universe->space[clickedsqx][clickedsqy];
				break;
			case SDL_KEYDOWN:
				switch (event.key.keysym.sym) {
				case SDLK_SPACE:
					paused = !paused;
					break;
				case SDLK_UP:
					if (fps < INT_MAX - 1) {
						++fps;
						paused = 0;
					}
					break;
				case SDLK_DOWN:
					if (fps - 1 > 0)
						--fps;
					if (fps - 1 == 0)
						paused = 1;
					break;
				}
			}
		}
		if (SDL_GetTicks() - lasttime >= 1000 / fps) {
			drawgluniverse(graphics->glwindow, graphics->gluniverse, universe);
			if (!paused) {
				nextuniverse = nextgen(universe);
				deluniverse(universe);
				universe = nextuniverse;
			}
			lasttime = SDL_GetTicks();
		}
	}
	freegphs(graphics);
	deluniverse(universe);

	return EXIT_SUCCESS;
}
