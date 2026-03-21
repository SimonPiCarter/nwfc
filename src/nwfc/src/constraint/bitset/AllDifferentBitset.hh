#pragma once

#include <vector>
#include "constraint/Constraint.hh"

namespace nwfc {

struct AllDifferentBitset : public Constraint {
	std::vector<bool> variable_bitset;
	AllDifferentBitset(std::vector<std::size_t> const &variable_indices) {
		std::size_t max_index = 0;
		for (auto index : variable_indices) {
			if (index > max_index) {
				max_index = index;
			}
		}
		variable_bitset.resize(max_index + 1, false);
		for (auto index : variable_indices) {
			variable_bitset[index] = true;
		}
	}

	void propagate(State &state, std::size_t variable_assigned) override {
		if (variable_assigned >= variable_bitset.size() || !variable_bitset[variable_assigned]) {
			return;
		}
		auto &memento = state.mementos.back();
		std::size_t assigned_value = get_assigned_value(state.domains[variable_assigned]);
		for (std::size_t i = 0; i < variable_bitset.size(); ++i) {
			if (variable_bitset[i] && i != variable_assigned) {
				BitsetDomain &domain = state.domains[i];
				memento.domain_mementos.push_back({i, remove_value(domain, assigned_value, variable_assigned)});
			}
		}
	}
};

} // namespace nwfc
