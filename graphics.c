#include <stdio.h>
#include <SDL2/SDL.h>

#include "graphics.h"
#include "universe.h"
#include "glad/glad.h"

struct glwindow*
newglwindow(int width, int height)
{
	struct glwindow* glwindow = malloc(sizeof(struct glwindow));

	if (!glwindow) {
		fprintf(stderr, "error: could not allocate memory\n");
		exit(1);
	}
	glwindow->window = SDL_CreateWindow("LIFE",
	                                   SDL_WINDOWPOS_CENTERED,
	                                   SDL_WINDOWPOS_CENTERED,
	                                      width,
	                                      height,
	                                      SDL_WINDOW_OPENGL);
	if (!glwindow->window) {
		SDL_Log("error: could not create window: %s", SDL_GetError());
		exit(1);
	}
	glwindow->context = SDL_GL_CreateContext(glwindow->window);
	if (!glwindow->context) {
		SDL_Log("error: could not create OpenGL context: %s", SDL_GetError());
		exit(1);
	}
	SDL_GL_SetSwapInterval(1);
	glwindow->width = width;
	glwindow->height = height;
	return glwindow;
}

void
initsdl(void)
{
	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		SDL_Log("error: could not initialize graphics: %s", SDL_GetError());
		SDL_Quit();
		exit(1);
	}
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
}

void
updateglwindow(struct glwindow* glwindow)
{
	SDL_GL_SwapWindow(glwindow->window);
}

void
delglwindow(struct glwindow* glwindow)
{
	SDL_GL_DeleteContext(glwindow->context);
	SDL_DestroyWindow(glwindow->window);
	free(glwindow);
}

void
quitsdl(void)
{
	SDL_Quit();
}

char*
readshader(const char* filename)
{
	FILE* file = fopen(filename, "rb");
	long filesize;
	char* buffer;

	if (!file) {
		fprintf(stderr, "error: could not open %s\n", filename);
		exit(1);
	}

	fseek(file, 0, SEEK_END);
	filesize = ftell(file);
	rewind(file);

	buffer = malloc(filesize + 1);
	if (fread(buffer, 1, filesize, file) != filesize) {
		fprintf(stderr, "error: could not read %s\n", filename);
		exit(1);
	}
	buffer[filesize] = 0;
	return buffer;
}

unsigned
genshaderprog(void)
{
	int success;
	char log[512];
	char* vshadersrc = readshader("vshader.glsl");
	char* fshadersrc = readshader("fshader.glsl");
	unsigned vshader = glCreateShader(GL_VERTEX_SHADER), fshader = glCreateShader(GL_FRAGMENT_SHADER);
	unsigned program = glCreateProgram();

	glShaderSource(vshader, 1, (const char**) &vshadersrc, NULL);
	glCompileShader(vshader);
	glGetShaderiv(vshader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(vshader, 512, NULL, log);
		fprintf(stderr, "error: %s\n", log);
		exit(1);
	}
	glShaderSource(fshader, 1, (const char**) &fshadersrc, NULL);
	glCompileShader(fshader);
	glGetShaderiv(fshader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(fshader, 512, NULL, log);
		fprintf(stderr, "error: %s\n", log);
		exit(1);
	}

	glAttachShader(program, vshader);
	glAttachShader(program, fshader);
	glLinkProgram(program);
	glGetProgramiv(program, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(program, 512, NULL, log);
		fprintf(stderr, "error: %s\n", log);
		exit(1);
	}
	glDeleteShader(vshader);
	glDeleteShader(fshader);
	free(vshadersrc);
	free(fshadersrc);
	return program;
}

void
drawgluniverse(struct glwindow* glwindow, struct gluniverse* gluniverse, struct universe* universe)
{
	int i, x, y;
	int colorlocation;
	float r, g, b;

	glUseProgram(gluniverse->program);
	colorlocation = glGetUniformLocation(gluniverse->program, "color");
	for (i = 0, x = 0, y = 0; i < gluniverse->numvao; ++i) {
		/* If the cell is alive, all the colors rgb will be set to 1
		 * so alive cells are white and dead cells are black. */
		r = universe->space[x][y];
		g = universe->space[x][y];
		b = universe->space[x][y];
		glUniform4f(colorlocation, r, g, b, 0.0f);
		glBindVertexArray(gluniverse->vao[i]);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
		if (x == universe->cols - 1) {
			++y;
			x = 0;
		} else {
			++x;
		}
	}
	updateglwindow(glwindow);
}

void
delgluniverse(struct gluniverse* gluniverse)
{
	size_t i;

	for (i = 0; i < gluniverse->numvao; ++i)
		glDeleteVertexArrays(1, &gluniverse->vao[i]);
	glDeleteProgram(gluniverse->program);
	free(gluniverse->vao);
	free(gluniverse);
}

/*
 * Transforms the window's x coordinates into OpenGL coordinates.
 */
float
normxcoord(struct glwindow* glwindow, float x)
{
	return ((float) -glwindow->width / 2.0f + x) / ((float) glwindow->width / 2);
}

/*
 * Transforms the window's x coordinates into OpenGL coordinates.
 * In my game of life the y coordinate starts counting from top to bottom,
 * so (0,0) is the top left corner, but in OpenGL the top starts at 1 and
 * decreases all the way down to -1.
 */
float
normycoord(struct glwindow* glwindow, float y)
{
	return ((float) glwindow->height / 2.0f - y) / ((float) glwindow->height / 2);
}

struct gluniverse*
newgluniverse(struct glwindow* glwindow, struct universe* universe)
{
	int x, y, squarec;
	unsigned vbo, ebo;
	struct gluniverse* gluniverse = malloc(sizeof(struct gluniverse));
	float sqwidth, sqheight, square[12];
	const unsigned indices[] = {
		0, 2, 1,  /* First triangle */
		1, 3, 2   /* Second triangle */
	};

	if (!gluniverse) {
		fprintf(stderr, "error: could not allocate memory.\n");
		exit(1);
	}
	gluniverse->program = genshaderprog();
	gluniverse->numvao = universe->cols * universe->rows;
	gluniverse->vao = malloc(gluniverse->numvao * sizeof(unsigned));
	if (!gluniverse->vao) {
		fprintf(stderr, "error: could not allocate memory.\n");
		exit(1);
	}
	sqwidth = (float) glwindow->width / (float) universe->cols;
	sqheight = (float) glwindow->height / (float) universe->rows;
	gluniverse->sqwidth = sqwidth;
	gluniverse->sqheight = sqheight;

	glViewport(0, 0, glwindow->width, glwindow->height);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glGenVertexArrays(gluniverse->numvao, &gluniverse->vao[squarec]);
	for (x = 0, y = 0, squarec = 0; squarec < universe->cols * universe->rows; ++squarec) {
		/* Top left */
		square[0] = normxcoord(glwindow, (float) x * sqwidth);
		square[1] = normycoord(glwindow, (float) y * sqheight);
		square[2] = 0.0f;
		/* Bottom left */
		square[3] = normxcoord(glwindow, (float) x * sqwidth);
		square[4] = normycoord(glwindow, (float) y * sqheight + sqheight);
		square[5] = 0.0f;
		/* Top right */
		square[6] = normxcoord(glwindow, (float) x * sqwidth + sqwidth);
		square[7] = normycoord(glwindow, (float) y * sqheight);
		square[8] = 0.0f;
		/* Bottom right */
		square[9] = normxcoord(glwindow, (float) x * sqwidth + sqwidth);
		square[10] = normycoord(glwindow, (float) y * sqheight + sqwidth);
		square[11] = 0.0f;

		glGenBuffers(1, &vbo);
		glGenBuffers(1, &ebo);
		glBindVertexArray(gluniverse->vao[squarec]);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);

		glBufferData(GL_ARRAY_BUFFER, sizeof(square), square, GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_DYNAMIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*) 0);

		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glDeleteBuffers(1, &vbo);
		glDeleteBuffers(1, &ebo);
		if (x == universe->cols - 1) {
			++y;
			x = 0;
		} else {
			++x;
		}
	}
	return gluniverse;
}

void
freegphs(struct graphics* graphics)
{
	delglwindow(graphics->glwindow);
	delgluniverse(graphics->gluniverse);
	free(graphics);
	quitsdl();
}

struct graphics*
initgphs(struct universe* universe)
{
	int width = 400, height = 400;
	struct graphics* graphics = malloc(sizeof(struct graphics));
	struct glwindow* glwindow;
	struct gluniverse* gluniverse;

	if (!graphics) {
		fprintf(stderr, "error: could not allocate memory.\n");
		exit(1);
	}

	initsdl();

	glwindow = newglwindow(width, height);
	gladLoadGLLoader((GLADloadproc) SDL_GL_GetProcAddress);
	gluniverse = newgluniverse(glwindow, universe);

	graphics->glwindow = glwindow;
	graphics->gluniverse = gluniverse;
	return graphics;
}