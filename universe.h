#ifndef LIFE_UNIVERSE_H
#define LIFE_UNIVERSE_H

#include <stdlib.h>

struct universe {
	char** space;
	int cols;
	int rows;
};

struct universe* genuniverse(int cols, int rows);
void deluniverse(struct universe* restrict universe);
struct universe* nextgen(struct universe* restrict universe);
void printuniverse(struct universe *restrict universe);

#endif