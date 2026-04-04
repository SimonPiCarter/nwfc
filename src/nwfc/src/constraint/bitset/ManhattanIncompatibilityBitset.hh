#pragma once

#include <cstdlib>
#include <vector>
#include "constraint/Constraint.hh"
#include "state/State.hh"

namespace nwfc {

/// Single constraint encoding the rule:
/// "If a cell is assigned trigger_value, remove all incompatible_values from every
///  unassigned cell within Manhattan distance. Conversely, if a cell is assigned
///  one of incompatible_values, remove trigger_value from every unassigned cell
///  within Manhattan distance."
///
/// This replaces a vector of O(W×H×D²) CompatibilityBitset objects with a single
/// object whose propagate() cost is O(D²) per assignment.
///
/// Assumes variable index == flat grid index: index = y * width + x.
struct ManhattanIncompatibilityBitset : public Constraint {
	std::size_t width;
	std::size_t height;
	int distance;
	std::size_t trigger_value;
	std::vector<bool> incompatible; ///< incompatible[v] == true iff value v is incompatible with trigger_value

	ManhattanIncompatibilityBitset(std::size_t w, std::size_t h,
			int dist, std::size_t trigger,
			std::vector<std::size_t> const &incompatible_values)
			: width(w), height(h), distance(dist), trigger_value(trigger) {
		for (std::size_t v : incompatible_values) {
			if (v >= incompatible.size()) {
				incompatible.resize(v + 1, false);
			}
			incompatible[v] = true;
		}
	}

	void propagate(State &state, std::size_t variable_assigned) override {
		if (variable_assigned >= width * height) {
			return;
		}

		std::size_t assigned_val = get_assigned_value(state.domains[variable_assigned]);

		bool is_trigger = (assigned_val == trigger_value);
		bool is_incompatible = (assigned_val < incompatible.size() && incompatible[assigned_val]);

		if (!is_trigger && !is_incompatible) {
			return;
		}

		int x = static_cast<int>(variable_assigned % width);
		int y = static_cast<int>(variable_assigned / width);

		auto &memento = state.mementos.back();

		for (int dx = -distance; dx <= distance; ++dx) {
			for (int dy = -distance; dy <= distance; ++dy) {
				if (std::abs(dx) + std::abs(dy) > distance || (dx == 0 && dy == 0)) {
					continue;
				}
				int nx = x + dx;
				int ny = y + dy;
				if (nx < 0 || ny < 0 || nx >= static_cast<int>(width) || ny >= static_cast<int>(height)) {
					continue;
				}
				std::size_t neighbor = static_cast<std::size_t>(ny) * width + static_cast<std::size_t>(nx);
				if (is_assigned(state, neighbor)) {
					continue;
				}
				BitsetDomain &domain = state.domains[neighbor];
				if (is_trigger) {
					for (std::size_t v = 0; v < incompatible.size(); ++v) {
						if (incompatible[v]) {
							memento.domain_mementos.push_back({neighbor, remove_value(domain, v, variable_assigned)});
						}
					}
				} else {
					memento.domain_mementos.push_back({neighbor, remove_value(domain, trigger_value, variable_assigned)});
				}
			}
		}
	}
};

} // namespace nwfc
