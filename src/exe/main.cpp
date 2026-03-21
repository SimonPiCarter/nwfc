
#include <iostream>

#include "variable/domain/BitsetDomain.hh"
#include "state/State.hh"
#include "constraint/bitset/AllDifferentBitset.hh"
#include "constraint/bitset/CompatibilityBitset.hh"
#include "constraint/bitset/CardinalityBitset.hh"
#include "variable/layout/2d/GridLayout.hh"
#include "utils/log/Log.hh"
#include <cassert>

int main() {
	nwfc::State state;
	nwfc::GridLayout layout(10, 10);
	std::vector<std::size_t> all_vars;
	for (std::size_t i = 0; i < layout.size(); ++i) {
		all_vars.push_back(i);
		state.domains.push_back(nwfc::BitsetDomain{std::vector<bool>(9, true), "var" + std::to_string(i)});
	}
	state.init();

	for (auto constraint : create_man_distance_incompatibilty_constraint(layout, 2, 2, {2})) {
		state.constraints.emplace_back(constraint);
	}
	// for (auto constraint : create_man_distance_incompatibilty_constraint(layout, 4, 1, {1})) {
	// 	state.constraints.emplace_back(constraint);
	// }
	// state.constraints.emplace_back(new nwfc::CompatibilityBitset(0, 0, {{1,0}, {1,1}, {2,0}, {2,1}}));
	// state.constraints.emplace_back(nwfc::CardinalityBitset::newUpperCardinality(all_vars, 7, 2));
	// state.constraints.emplace_back(nwfc::CardinalityBitset::newLowerCardinality(all_vars, 5, 20));
	state.constraints.emplace_back(nwfc::CardinalityBitset::newLowerCardinality(all_vars, 2, 10));

	std::size_t deepness = 0;
	std::size_t var = greedy_pick_variable(state);
	while (var < layout.size()) {
		deepness = std::max(deepness, state.affectation.size());
		std::size_t val = greedy_pick_value(state, var);
		progress(state, var, val);
		nwfc::display_grid(layout, state);
		bool bt = backtrack(state, var, val);
		// for(auto dom : state.domains) {
		// 	display_domain(INFO_LOG, dom);
		// }
		var = greedy_pick_variable(state);
		INFO_LOG<<"deepeness = "<<deepness<<"/"<<state.domains.size()<<std::endl;
	}
	nwfc::display_grid(layout, state);
	return 0;
}
