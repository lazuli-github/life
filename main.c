#include <stdlib.h>
#include <time.h>

#include "graphics.h"
#include "universe.h"

int
main(void)
{
	SDL_Event event;
	struct graphics* graphics;
	const int cols = 50, rows = 49;
	struct universe* universe = genuniverse(cols, rows);
	struct universe* nextuniverse;
	int quit = 0, lasttime = 0, currenttime = 0;

	graphics = initgphs(universe);

	srand(time(NULL));
	for (int i = 0; i < cols; ++i) {
		for (int j = 0; j < rows; j++)
			universe->space[i][j] = rand() & 0xff % 2;
	}
	lasttime = SDL_GetTicks();
	while (!quit)
	{
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT)
				quit = 1;
		}
		if (SDL_GetTicks() - lasttime >= 100) {
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
