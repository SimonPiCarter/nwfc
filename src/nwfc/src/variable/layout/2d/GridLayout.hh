#pragma once

#include <vector>
#include "constraint/bitset/CompatibilityBitset.hh"

namespace nwfc {

struct GridLayout {
    std::size_t width;
	std::size_t height;

	GridLayout(std::size_t w, std::size_t h) : width(w), height(h) {}

	std::size_t get_index(std::size_t x, std::size_t y) const {
		return y * width + x;
	}

	std::size_t get_x(std::size_t index) const {
		return index % width;
	}

	std::size_t get_y(std::size_t index) const {
		return index / width;
	}

    std::size_t size() const {
        return width * height;
    }
};

std::vector<CompatibilityBitset *> create_man_distance_incompatibilty_constraint(GridLayout const &layout, int distance, std::size_t value_index, std::vector<std::size_t> incompatibilities);

void display_grid(GridLayout const &layout, State const &state);

}
