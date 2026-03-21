#pragma once

#include "variable/domain/Domain.hh"
#include "constraint/Constraint.hh"

#include <list>
#include <memory>

namespace nwfc {

struct StateMemento {
	std::size_t index;
	std::vector<DomainMementoIdx> domain_mementos;
};

struct State {
	void init() {
		if (assigned.size() < domains.size()) {
			assigned.resize(domains.size(), false);
			mementos.resize(1, {0, {}});
		}
	}
	std::vector<Domain> domains;
	std::vector<bool> assigned;
	std::vector<std::size_t> affectation;
	std::vector<std::unique_ptr<Constraint>> constraints;

	// mementos
	std::list<StateMemento> mementos;
};

bool is_assigned(State const &state, std::size_t var);
bool progress_and_backtrack(State &state, std::size_t var, std::size_t val);

// greedy
std::size_t greedy_pick_variable(State const &state);
std::size_t greedy_pick_value(State const &state, std::size_t var);
std::size_t random_pick_value(State const &state, std::size_t var);

}
