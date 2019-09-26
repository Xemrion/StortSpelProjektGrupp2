#pragma once

#include "defs.hpp"
#include "Map.hpp"

// -1 seed => random seed
[[deprecated]] // TODO: remove?
void generate_random_segments(Map* map, U16 num_roads, I32 seed) {
	RD        rd;
	RNG       rng           (rd());
	U16_Dist  gen_x         (0, map->width / 2);
	U16_Dist  gen_y         (0, map->height / 2);
	F32_Dist  gen_selection (0, 100);

	if (seed != -1)
		rng.seed(seed); // set seed if not -1

	while (--num_roads) {
		if (gen_selection(rng) < 50) { // odds: 50-50 vertical/horizontal
			size_t x = gen_x(rng) * 2,
				y_start = gen_y(rng) * 2,
				y_end = gen_y(rng) * 2;
			for (size_t y = y_start; y < y_end; ++y)
				map->data[map->index(x, y)] = Tile::road0;
		}
		else {
			size_t y = gen_y(rng) * 2,
				x_start = gen_x(rng) * 2,
				x_end = gen_x(rng) * 2;
			for (size_t x = x_start; x < x_end; ++x)
				map->data[map->index(x, y)] = Tile::road0;
		}
	}
}
