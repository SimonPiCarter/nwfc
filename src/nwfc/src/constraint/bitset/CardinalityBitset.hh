#pragma once

#include <algorithm>
#include <vector>
#include <optional>
#include "constraint/Constraint.hh"
#include "utils/log/Log.hh"
#include "state/State.hh"

namespace nwfc {

struct CardinalityBitset : public Constraint {

	std::vector<std::size_t> variables;
	std::size_t value_index;
	std::optional<std::size_t> lower_bound;
	std::optional<std::size_t> upper_bound;

	std::size_t count(State const &state) const {
		std::size_t cnt = 0;
		for (const auto &var : variables) {
			if (is_assigned(state, var) && get_assigned_value(state.domains[var]) == value_index) {
				cnt++;
			}
		}
		return cnt;
	}

	std::size_t count_possible(State const &state) const {
		std::size_t cnt = 0;
		for (const auto &var : variables) {
			if (!is_assigned(state, var) && is_value_in_domain(state.domains[var], value_index)) {
				cnt++;
			}
		}
		return cnt;
	}

	void propagate(State &state, std::size_t variable_assigned) override {
		if (std::find(variables.begin(), variables.end(), variable_assigned) == variables.end()) {
			return;
		}

		auto &memento = state.mementos.back();
		auto cur_count = count(state);
		if (upper_bound && cur_count >= *upper_bound) {
			// Compute explainaion: Last var that was assigned to the value
			std::size_t explaination = 0;
			std::size_t rank = 0;
			for(std::size_t var : variables) {
				if (state.assigned[var] && state.rank[var] > rank &&
					get_assigned_value(state.domains[var]) == value_index) {
					explaination = var;
					rank = state.rank[var];
				}
			}
			for(std::size_t var : variables) {
				if (!is_assigned(state, var)) {
					BitsetDomain &domain = state.domains[var];
					memento.domain_mementos.push_back({var, remove_value(domain, value_index, explaination)});
				}
			}
		}

		std::size_t possible_count = count_possible(state);
		if (lower_bound && possible_count + cur_count <= *lower_bound) {
			// Compute explainaion: Last var that was assigned to a different value with the value in its domain
			std::size_t explaination = 0;
			std::size_t rank = 0;
			for(std::size_t var : variables) {
				if (state.assigned[var] && state.rank[var] > rank &&
					is_value_in_domain(state.domains[var], value_index) &&
					get_assigned_value(state.domains[var]) != value_index) {
					explaination = var;
					rank = state.rank[var];
				}
			}
			for(std::size_t var : variables) {
				BitsetDomain &domain = state.domains[var];
				if (!is_assigned(state, var) && (is_value_in_domain(domain, value_index) || possible_count + cur_count < *lower_bound)) {
					memento.domain_mementos.push_back({var, remove_all_but_value(domain, value_index, explaination)});
				}
			}
		}
	}

	static CardinalityBitset *newLowerCardinality(std::vector<std::size_t> const & v, std::size_t val, std::size_t lb) {
		return new CardinalityBitset(v, val, lb, std::nullopt);
	}
	static CardinalityBitset *newUpperCardinality(std::vector<std::size_t> const & v, std::size_t val, std::size_t ub) {
		return new CardinalityBitset(v, val, std::nullopt, ub);
	}
	static CardinalityBitset *newCardinality(std::vector<std::size_t> const & v, std::size_t val, std::size_t lb, std::size_t ub) {
		return new CardinalityBitset(v, val, lb, ub);
	}
private:
	CardinalityBitset(std::vector<std::size_t> const & v, std::size_t val, std::optional<std::size_t> lb, std::optional<std::size_t> ub)
		: variables(v), value_index(val), lower_bound(lb), upper_bound(ub) {}

};

} // namespace nwfc
