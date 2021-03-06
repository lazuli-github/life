#include <stdlib.h>

#include "graphics.h"
#include "universe.h"

int
main(void)
{
	SDL_Event event;
	struct graphics* graphics;
	struct universe* universe = genuniverse(40, 40);
	struct universe* nextuniverse;
	int quit = 0, lasttime = 0, currenttime = 0;

	graphics = initgphs(universe);

	universe->space[19][20] = 1;
	universe->space[20][20] = 1;
	universe->space[21][20] = 1;
	lasttime = SDL_GetTicks();
	while (!quit)
	{
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT)
				quit = 1;
		}
		if (SDL_GetTicks() - lasttime >= 150) {
			drawgluniverse(graphics->glwindow, graphics->gluniverse, universe);
			nextuniverse = nextgen(universe);
			deluniverse(universe);
			universe = nextuniverse;
			lasttime = SDL_GetTicks();
		}
	}
	freegphs(graphics);
	deluniverse(universe);

	return EXIT_SUCCESS;
}
