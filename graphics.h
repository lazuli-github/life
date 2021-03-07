#ifndef LIFE_GRAPHICS_INCLUDE_GUARD
#define LIFE_GRAPHICS_INCLUDE_GUARD

#include "SDL2/SDL.h"
#include "universe.h"

struct glwindow {
	SDL_Window *window;
	SDL_GLContext context;
	int width;
	int height;
};

struct gluniverse {
	unsigned program;
	unsigned* vao;
	size_t numvao;
};

struct graphics {
	struct glwindow* glwindow;
	struct gluniverse* gluniverse;
};

struct graphics* initgphs(struct universe* universe);
struct glwindow* newglwindow(int width, int height);
struct gluniverse* newgluniverse(struct glwindow* glwindow, struct universe* universe);
void drawgluniverse(struct glwindow* glwindow, struct gluniverse* gluniverse, struct universe* universe);
void freegphs(struct graphics* graphics);

#endif /* LIFE_GRAPHICS_INCLUDE_GUARD */
