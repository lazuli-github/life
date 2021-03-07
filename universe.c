#include <stdio.h>

#include "universe.h"

static char
getnewcell(struct universe* restrict universe, int x, int y)
{
	int neighbors = 0;

	if (x > 0 && y > 0)
		neighbors += universe->space[x - 1][y - 1];
	if (y > 0)
		neighbors += universe->space[x][y - 1];
	if (x < universe->cols - 2 && y > 0)
		neighbors += universe->space[x + 1][y - 1];
	if (x < universe->cols - 2)
		neighbors += universe->space[x + 1][y];
	if (x < universe->cols - 2 && y < universe->rows - 2)
		neighbors += universe->space[x + 1][y + 1];
	if (y < universe->cols - 2)
		neighbors += universe->space[x][y + 1];
	if (x > 0 && y < universe->rows - 2)
		neighbors += universe->space[x - 1][y + 1];
	if (x > 0)
		neighbors += universe->space[x - 1][y];

	if (universe->space[x][y] && (neighbors == 2 || neighbors == 3))
		return 1;
	if (!universe->space[x][y] && neighbors == 3)
		return 1;
	return 0;
}

void
deluniverse(struct universe* restrict universe)
{
	int i;

	for (i = 0; i < universe->cols; ++i)
		free(universe->space[i]);
	free(universe->space);
	free(universe);
}

struct universe*
genuniverse(int cols, int rows)
{
	struct universe* universe = malloc(sizeof(struct universe));
	int i;

	if (!universe) {
		fprintf(stderr, "error: could not allocate memory\n");
		exit(1);
	}
	universe->space = malloc(cols * sizeof(char*));
	if (!universe->space) {
		fprintf(stderr, "error: could not allocate memory\n");
		exit(1);
	}
	for (i = 0; i < cols; ++i) {
		universe->space[i] = calloc(rows, 1);
		if (!universe->space[i]) {
			fprintf(stderr, "error: could not allocate memory\n");
			return NULL;
		}
	}
	universe->cols = cols;
	universe->rows = rows;

	return universe;
}

struct universe*
nextgen(struct universe* restrict universe)
{
	int x, y;
	struct universe* newuniverse = genuniverse(universe->cols, universe->rows);

	for (x = 0; x < universe->cols; ++x) {
		for (y = 0; y < universe->rows; ++y)
			newuniverse->space[x][y] = getnewcell(universe, x, y);
	}
	return newuniverse;
}

void
printuniverse(struct universe *restrict universe)
{
	long x, y;

	for (x = 0; x < universe->rows; x++) {
		for (y = 0; y < universe->cols; y++) {
			if (universe->space[y][x])
				printf("🟩");
			else
				printf("⬜");
		}
		putchar('\n');
	}
	putchar('\n');
}
