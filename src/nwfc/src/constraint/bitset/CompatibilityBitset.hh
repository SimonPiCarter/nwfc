#pragma once

#include <vector>
#include "constraint/Constraint.hh"
#include "state/State.hh"

namespace nwfc {

struct CompatibilityBitset : public Constraint {

	struct Incompatibility {
		std::size_t variable_index;
		std::size_t incompatible_value;
	};


	std::vector<Incompatibility> incompatibilities;
	std::size_t variable_index;
	std::size_t value_index;

	CompatibilityBitset(std::size_t var, std::size_t val, std::vector<Incompatibility> const &in) : incompatibilities(in), variable_index(var), value_index(val) {}

	void propagate(State &state, std::size_t variable_assigned) override {
		if (variable_assigned != variable_index || get_assigned_value(state.domains[variable_index]) != value_index) {
			return;
		}

		auto &memento = state.mementos.back();
		for (const auto &incompatibility : incompatibilities) {
			if (incompatibility.variable_index >= state.domains.size()) {
				continue;
			}
			if (!is_assigned(state, incompatibility.variable_index)) {
				BitsetDomain &domain = state.domains[incompatibility.variable_index];
				memento.domain_mementos.push_back({incompatibility.variable_index, remove_value(domain, incompatibility.incompatible_value, variable_assigned)});
			}
		}
	}
};

} // namespace nwfc
