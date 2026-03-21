#include "GridLayout.hh"

#include "utils/log/Log.hh"
#include <iostream>

namespace nwfc {

std::vector<CompatibilityBitset *> create_man_distance_incompatibilty_constraint(GridLayout const &layout, int distance, std::size_t value_index, std::vector<std::size_t> val_in) {
    std::vector<CompatibilityBitset *> constraints;
    for(std::size_t x = 0; x < layout.width ; ++ x) {
        for(std::size_t y = 0; y < layout.height ; ++ y) {
            std::vector<CompatibilityBitset::Incompatibility> incompatibilities;
            std::size_t index = layout.get_index(x, y);

            for(int dx = -distance; dx <= distance; ++ dx) {
                for(int dy = -distance; dy <= distance; ++ dy) {
                    if(abs(dx) + abs(dy) > distance ||
                      (dx == 0 && dy == 0) ||
                      static_cast<int>(x) + dx < 0 || static_cast<int>(y) + dy < 0 ||
                      static_cast<int>(x) + dx > layout.width || static_cast<int>(y) + dy > layout.height) {
                        continue;
                    }
                    std::size_t nx = static_cast<int>(x) + dx;
                    std::size_t ny = static_cast<int>(y) + dy;
                    for (std::size_t incompatible_value : val_in) {
                        incompatibilities.push_back({static_cast<std::size_t>(layout.get_index(nx, ny)), incompatible_value});
                    }
                }
            }
            constraints.push_back(new CompatibilityBitset(index, value_index, incompatibilities));
        }
    }
    return constraints;
}

void display_grid(GridLayout const &layout, State const &state) {
    for(std::size_t y = 0; y < layout.height; ++y) {
		for(std::size_t x = 0; x < layout.width; ++x) {
			std::size_t index = layout.get_index(x, y);
			if(state.assigned[index]) {
				INFO_LOG << get_assigned_value(state.domains[index]) << " ";
			} else {
				INFO_LOG << "? ";
			}
		}
		INFO_LOG << "\n";
	}
}

}
