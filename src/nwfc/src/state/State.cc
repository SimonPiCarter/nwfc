#include "State.hh"

#include <stdexcept>
#include <set>
#include "utils/log/Log.hh"

namespace nwfc {

bool is_assigned(State const &state, std::size_t var) {
	// if (state.assigned[var] != is_decided(state.domains[var])) {
		// throw std::runtime_error("Assigned status is inconcistent for var "+std::to_string(var));
		// DEBUG_LOG<<("Assigned status is inconcistent for var "+std::to_string(var))<<std::endl;
	// }
	return state.assigned[var];
}

void progress(State &state, std::size_t var, std::size_t val) {
	DEBUG_LOG << "Progressing with variable " << var << " = " << val << std::endl;
	state.init();
	// push memento
	state.mementos.push_back(StateMemento{state.mementos.size(), {}});
	if (!is_value_in_domain(state.domains[var], val)) {
		// throw std::runtime_error("Tried to progress with a value not in domain");
		// DEBUG_LOG<<("Tried to progress with a value not in domain")<<std::endl;
	}

	auto &memento = state.mementos.back();
	// DEBUG_LOG<<"Memento idx: "<<memento.index<<std::endl;
	assign_value(state.domains[var], val);
	// update assigned vector
	state.assigned[var] = true;
	state.rank[var] = state.affectation.size();
	state.affectation.push_back(var);

	// propagate
	for(auto &constraint: state.constraints) {
		constraint->propagate(state, var);
	}
}


std::set<std::size_t> explainations(State const &state) {
	std::set<std::size_t> set;
	for (std::size_t i = 0 ; i < state.domains.size() ; ++ i) {
		if (is_empty(state.domains[i])) {
			set.insert(state.domains[i].explaination.begin(), state.domains[i].explaination.end());
		}
	}
	return set;
}

std::size_t get_jump(State const &state) {
	std::set<std::size_t> set = explainations(state);
	if (set.size() == 0) {
		return state.affectation.size()-1;
	}
	return *(set.rbegin());
}

std::size_t get_conflict(State const &state) {
	std::set<std::size_t> set = explainations(state);
	if (set.size() <= 1) {
		return state.affectation.size()-1;
	}
	return *(++set.rbegin());
}

void atomic_batcktrack(State &state) {
	std::size_t cur_var = state.affectation.back();
	std::size_t cur_val = get_assigned_value(state.domains[cur_var]);
	INFO_LOG << "Backtracking with variable " << cur_var << " = " << cur_val << std::endl;

	auto &last_memento = state.mementos.back();
	DEBUG_LOG<<"Memento idx: "<<last_memento.index<<std::endl;
	unnassign_value(state.domains[cur_var]);
	state.assigned[cur_var] = false;
	state.rank[cur_var] = 0;
	state.affectation.pop_back();
	// revert memento
	for(std::size_t i = last_memento.domain_mementos.size() ; i > 0 ; --i) {
		DomainMementoIdx const &memIdx = last_memento.domain_mementos[i-1];
		restore(state.domains[memIdx.index], memIdx.memento);
	}
	state.mementos.pop_back();
}

bool backtrack(State &state) {
	// check if any domain has been emptied
	bool backtrack = false;
	for (std::size_t i = 0 ; i < state.domains.size() ; ++ i) {
		if (is_empty(state.domains[i])) {
			backtrack = true;
			break;
		}
	}

	while (backtrack) {
		std::size_t cur_var = state.affectation.back();
		std::size_t cur_val = get_assigned_value(state.domains[cur_var]);
		std::size_t explaination = get_conflict(state);
		atomic_batcktrack(state);

		// remove value from domain in previous memento layer
		state.mementos.back().domain_mementos.push_back({ cur_var, remove_value(state.domains[cur_var], cur_val, explaination)});

		if (is_empty(state.domains[cur_var])) {
			if (state.affectation.empty()) {
				// no solution
				return true;
			}
		} else {
			break;
		}
	}

	return backtrack;
}

bool backjump(State &state) {
	// check if any domain has been emptied
	bool has_backtrack = false;
	for (std::size_t i = 0 ; i < state.domains.size() ; ++ i) {
		if (is_empty(state.domains[i])) {
			has_backtrack = true;
			break;
		}
	}

	std::size_t jump = state.affectation.size();
	if (has_backtrack) {
		jump = get_jump(state);
	}
	DEBUG_LOG<<"jump: "<<jump<<std::endl;

	while (state.affectation.size() > jump) {
		std::size_t cur_var = state.affectation.back();
		std::size_t cur_val = get_assigned_value(state.domains[cur_var]);
		std::size_t explaination = get_conflict(state);
		DEBUG_LOG<<"explaination: "<<explaination<<std::endl;
		atomic_batcktrack(state);

		// remove value from domain in previous memento layer
		state.mementos.back().domain_mementos.push_back({ cur_var, remove_value(state.domains[cur_var], cur_val, explaination)});

		if (is_empty(state.domains[cur_var]) && state.affectation.empty()) {
			// no solution
			return true;
		}
	}

	if(has_backtrack) {
		backtrack(state);
	}
	return has_backtrack;
}

bool progress_and_backtrack(State &state, std::size_t var, std::size_t val) {
	progress(state, var, val);
	return backtrack(state);
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
	// throw std::runtime_error("Tried to pick a value from an empty domain!");
	DEBUG_LOG<<("Tried to pick a value from an empty domain!")<<std::endl;
	return 0;
}

std::size_t random_pick_value(State const &state, std::size_t var) {
	std::vector<std::size_t> available_values;
	for (std::size_t i = 0 ; i < state.domains[var].bits.size() ; ++ i ) {
		if (state.domains[var].bits[i]) {
			available_values.push_back(i);
		}
	}
	if (available_values.size() == 0) {
		// throw std::runtime_error("Tried to pick a value from an empty domain!");
		DEBUG_LOG<<("Tried to pick a value from an empty domain!")<<std::endl;
		return state.domains.size();
	}
	return available_values[state.generator.next_int(0, available_values.size()-1)];
}

} // namespace nwfc
