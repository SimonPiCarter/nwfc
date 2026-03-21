#include <gtest/gtest.h>

#include "variable/domain/BitsetDomain.hh"
#include "state/State.hh"
#include "constraint/bitset/AllDifferentBitset.hh"
#include "constraint/bitset/CompatibilityBitset.hh"
#include "constraint/bitset/CardinalityBitset.hh"
#include "variable/layout/2d/GridLayout.hh"

TEST(ProgressBacktrack, progress) {
	nwfc::State state;
	state.domains.push_back(nwfc::BitsetDomain{{true, true, true}});
	state.domains.push_back(nwfc::BitsetDomain{{true, true, true}});
	state.domains.push_back(nwfc::BitsetDomain{{true, true, true}});
	state.domains.push_back(nwfc::BitsetDomain{{true, true, true}});
	state.init();
	state.constraints.emplace_back(new nwfc::AllDifferentBitset({0, 1, 2}));

	{
		std::size_t var = greedy_pick_variable(state);
		std::size_t val = greedy_pick_value(state, var);
		EXPECT_EQ(0U, var);
		EXPECT_EQ(0U, val);
		bool backtrack = progress_and_backtrack(state, var, val);
		EXPECT_FALSE(backtrack);
	}

	EXPECT_EQ(nwfc::effective_domain(state.domains[0]), std::vector<bool>({true, false, false}));
	EXPECT_EQ(nwfc::effective_domain(state.domains[1]), std::vector<bool>({false, true, true}));
	EXPECT_EQ(nwfc::effective_domain(state.domains[2]), std::vector<bool>({false, true, true}));
	EXPECT_EQ(nwfc::effective_domain(state.domains[3]), std::vector<bool>({true, true, true}));

	{
		std::size_t var = greedy_pick_variable(state);
		std::size_t val = greedy_pick_value(state, var);
		EXPECT_EQ(1U, var);
		EXPECT_EQ(1U, val);
		bool backtrack = progress_and_backtrack(state, var, val);
		EXPECT_FALSE(backtrack);
	}

	EXPECT_EQ(nwfc::effective_domain(state.domains[0]), std::vector<bool>({true, false, false}));
	EXPECT_EQ(nwfc::effective_domain(state.domains[1]), std::vector<bool>({false, true, false}));
	EXPECT_EQ(nwfc::effective_domain(state.domains[2]), std::vector<bool>({false, false, true}));
	EXPECT_EQ(nwfc::effective_domain(state.domains[3]), std::vector<bool>({true, true, true}));
}

TEST(ProgressBacktrack, progress_and_backtrack) {
	nwfc::State state;
	state.domains.push_back(nwfc::BitsetDomain{{true, true, true}});
	state.domains.push_back(nwfc::BitsetDomain{{true, true, true}});
	state.domains.push_back(nwfc::BitsetDomain{{true, true, true}});
	state.domains.push_back(nwfc::BitsetDomain{{true, true, true}});
	state.init();
	state.constraints.emplace_back(new nwfc::AllDifferentBitset({0, 1, 2}));
	state.constraints.emplace_back(new nwfc::CompatibilityBitset(0, 0, {{1,0}, {1,1}, {1,2}}));

	{
		std::size_t var = greedy_pick_variable(state);
		std::size_t val = greedy_pick_value(state, var);
		EXPECT_EQ(0U, var);
		EXPECT_EQ(0U, val);
		bool backtrack = progress_and_backtrack(state, var, val);
		EXPECT_TRUE(backtrack);
	}

	EXPECT_EQ(nwfc::effective_domain(state.domains[0]), std::vector<bool>({false, true, true}));
	EXPECT_EQ(nwfc::effective_domain(state.domains[1]), std::vector<bool>({true, true, true}));
	EXPECT_EQ(nwfc::effective_domain(state.domains[2]), std::vector<bool>({true, true, true}));
	EXPECT_EQ(nwfc::effective_domain(state.domains[3]), std::vector<bool>({true, true, true}));

	{
		std::size_t var = greedy_pick_variable(state);
		std::size_t val = greedy_pick_value(state, var);
		EXPECT_EQ(0U, var);
		EXPECT_EQ(1U, val);
		bool backtrack = progress_and_backtrack(state, var, val);
		EXPECT_FALSE(backtrack);
	}

	EXPECT_EQ(nwfc::effective_domain(state.domains[0]), std::vector<bool>({false, true, false}));
	EXPECT_EQ(nwfc::effective_domain(state.domains[1]), std::vector<bool>({true, false, true}));
	EXPECT_EQ(nwfc::effective_domain(state.domains[2]), std::vector<bool>({true, false, true}));
	EXPECT_EQ(nwfc::effective_domain(state.domains[3]), std::vector<bool>({true, true, true}));
}

TEST(ProgressBacktrack, progress_loop) {
	nwfc::State state;
	state.domains.push_back(nwfc::BitsetDomain{{true, true, true}, "var1"});
	state.domains.push_back(nwfc::BitsetDomain{{true, true, true}, "var2"});
	state.domains.push_back(nwfc::BitsetDomain{{true, true, true}, "var3"});
	state.domains.push_back(nwfc::BitsetDomain{{true, true, true}, "var4"});
	state.init();
	state.constraints.emplace_back(new nwfc::AllDifferentBitset({0, 1, 2}));
	state.constraints.emplace_back(new nwfc::CompatibilityBitset(0, 0, {{1,0}, {1,1}, {2,0}, {2,1}}));

	{
		std::size_t var = greedy_pick_variable(state);
		std::size_t val = greedy_pick_value(state, var);
		EXPECT_TRUE(0U == var);
		EXPECT_TRUE(0U == val);
		bool backtrack = progress_and_backtrack(state, var, val);
		EXPECT_TRUE(!backtrack);
	}

	EXPECT_TRUE(nwfc::effective_domain(state.domains[0]) == std::vector<bool>({true, false, false}));
	EXPECT_TRUE(nwfc::effective_domain(state.domains[1]) == std::vector<bool>({false, false, true}));
	EXPECT_TRUE(nwfc::effective_domain(state.domains[2]) == std::vector<bool>({false, false, true}));
	EXPECT_TRUE(nwfc::effective_domain(state.domains[3]) == std::vector<bool>({true, true, true}));

	{
		std::size_t var = greedy_pick_variable(state);
		std::size_t val = greedy_pick_value(state, var);
		EXPECT_TRUE(1U == var);
		EXPECT_TRUE(2U == val);
		bool backtrack = progress_and_backtrack(state, var, val);
		EXPECT_TRUE(backtrack);
	}

	EXPECT_TRUE(nwfc::effective_domain(state.domains[0]) == std::vector<bool>({false, true, true}));
	EXPECT_TRUE(nwfc::effective_domain(state.domains[1]) == std::vector<bool>({true, true, true}));
	EXPECT_TRUE(nwfc::effective_domain(state.domains[2]) == std::vector<bool>({true, true, true}));
	EXPECT_TRUE(nwfc::effective_domain(state.domains[3]) == std::vector<bool>({true, true, true}));


	std::size_t var = greedy_pick_variable(state);
	while (var < 4) {
		std::size_t val = greedy_pick_value(state, var);
		bool backtrack = progress_and_backtrack(state, var, val);
		var = greedy_pick_variable(state);
	}

	EXPECT_TRUE(nwfc::effective_domain(state.domains[0]) == std::vector<bool>({false, true, false}));
	EXPECT_TRUE(nwfc::effective_domain(state.domains[1]) == std::vector<bool>({true, false, false}));
	EXPECT_TRUE(nwfc::effective_domain(state.domains[2]) == std::vector<bool>({false, false, true}));
	EXPECT_TRUE(nwfc::effective_domain(state.domains[3]) == std::vector<bool>({true, false, false}));

}


TEST(ProgressBacktrack, progress_loop_2) {
	nwfc::State state;
	nwfc::GridLayout layout(10, 10);
	for (std::size_t i = 0; i < layout.size(); ++i) {
		state.domains.push_back(nwfc::BitsetDomain{std::vector<bool>(9, true), "var" + std::to_string(i)});
	}

	state.init();

	for (auto constraint : create_man_distance_incompatibilty_constraint(layout, 4, 0, {0/*,1,2,3,4,5,6,7*/})) {
		state.constraints.emplace_back(constraint);
	}
	for (auto constraint : create_man_distance_incompatibilty_constraint(layout, 4, 1, {1})) {
		state.constraints.emplace_back(constraint);
	}
	state.constraints.emplace_back(new nwfc::CompatibilityBitset(0, 0, {{1,0}, {1,1}, {2,0}, {2,1}}));

	std::size_t var = greedy_pick_variable(state);
	while (var < layout.size()) {
		std::size_t val = random_pick_value(state, var);
		bool backtrack = progress_and_backtrack(state, var, val);
		var = greedy_pick_variable(state);
	}
	// nwfc::display_grid(layout, state);
}
