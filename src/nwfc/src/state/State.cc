#include "State.hh"

#include <random>
#include <stdexcept>
#include "utils/log/Log.hh"

namespace nwfc {

bool is_assigned(State const &state, std::size_t var) {
	return state.assigned[var];
}

bool progress_and_backtrack(State &state, std::size_t var, std::size_t val) {
	INFO_LOG << "Progressing with variable " << var << " = " << val << std::endl;
	state.init();
	// push memento
	state.mementos.push_back(StateMemento{state.mementos.size(), {}});
	if (!is_value_in_domain(state.domains[var], val)) {
		throw std::runtime_error("Tried to progress with a value not in domain");
	}

	auto &memento = state.mementos.back();
	DEBUG_LOG<<"Memento idx: "<<memento.index<<std::endl;
	memento.domain_mementos.push_back({var, assign_value(state.domains[var], val)});
	// update assigned vector
	state.assigned[var] = true;
	state.affectation.push_back(var);

	// propagate
	for(auto &constraint: state.constraints) {
		constraint->propagate(state, var);
	}

	// check if any domain has been emptied
	bool backtrack = false;
	for (std::size_t i = 0 ; i < state.domains.size() ; ++ i) {
		if (is_empty(state.domains[i])) {
			backtrack = true;
			break;
		}
	}

	std::size_t cur_var = var;
	std::size_t cur_val = val;
	while (backtrack) {
		INFO_LOG << "Backtracking with variable " << cur_var << " = " << cur_val << std::endl;

		auto &last_memento = state.mementos.back();
		DEBUG_LOG<<"Memento idx: "<<last_memento.index<<std::endl;
		state.assigned[cur_var] = false;
		state.affectation.pop_back();
		// revert memento
		for(std::size_t i = last_memento.domain_mementos.size() ; i > 0 ; --i) {
			DomainMementoIdx const &memIdx = last_memento.domain_mementos[i-1];
			restore(state.domains[memIdx.index], memIdx.memento);
		}
		state.mementos.pop_back();

		// remove value from domain in previous memento layer
		state.mementos.back().domain_mementos.push_back({ cur_var, remove_value(state.domains[cur_var], cur_val)});

		if (is_empty(state.domains[cur_var])) {
			if (state.affectation.empty()) {
				// no solution
				return true;
			}
			cur_var = state.affectation.back();
			cur_val = get_assigned_value(state.domains[cur_var]);
		} else {
			break;
		}
	}

	return backtrack;
}

std::size_t greedy_pick_variable(State const &state) {
	std::size_t var = state.domains.size();
	for(std::size_t i = 0 ; i < state.domains.size() ; ++i) {
		bool decided = is_decided(state.domains[i]);
		if (decided && !state.assigned[i]) {
			return i;
		}
		if (!decided && var == state.domains.size()) {
			var = i;
		}
	}
	return var;
}

std::size_t greedy_pick_value(State const &state, std::size_t var) {
	for (std::size_t i = 0 ; i < state.domains[var].bits.size() ; ++ i ) {
		if (state.domains[var].bits[i]) {
			return i;
		}
	}
	throw std::runtime_error("Tried to pick a value from an empty domain!");
}

std::size_t random_pick_value(State const &state, std::size_t var) {
	std::vector<std::size_t> available_values;
	for (std::size_t i = 0 ; i < state.domains[var].bits.size() ; ++ i ) {
		if (state.domains[var].bits[i]) {
			available_values.push_back(i);
		}
	}
	if (available_values.size() == 0) {
		throw std::runtime_error("Tried to pick a value from an empty domain!");
	}
	return available_values[rand() % available_values.size()];
}

} // namespace nwfc
