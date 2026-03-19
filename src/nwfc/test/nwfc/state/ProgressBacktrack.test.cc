#include <gtest/gtest.h>

#include "variable/domain/BitsetDomain.hh"
#include "state/State.hh"
#include "constraint/bitset/AllDifferentBitset.hh"
#include "constraint/bitset/CompatibilityBitset.hh"

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

	EXPECT_EQ(state.domains[0].bits, std::vector<bool>({true, false, false}));
	EXPECT_EQ(state.domains[1].bits, std::vector<bool>({false, true, true}));
	EXPECT_EQ(state.domains[2].bits, std::vector<bool>({false, true, true}));
	EXPECT_EQ(state.domains[3].bits, std::vector<bool>({true, true, true}));

	{
		std::size_t var = greedy_pick_variable(state);
		std::size_t val = greedy_pick_value(state, var);
		EXPECT_EQ(1U, var);
		EXPECT_EQ(1U, val);
		bool backtrack = progress_and_backtrack(state, var, val);
		EXPECT_FALSE(backtrack);
	}

	EXPECT_EQ(state.domains[0].bits, std::vector<bool>({true, false, false}));
	EXPECT_EQ(state.domains[1].bits, std::vector<bool>({false, true, false}));
	EXPECT_EQ(state.domains[2].bits, std::vector<bool>({false, false, true}));
	EXPECT_EQ(state.domains[3].bits, std::vector<bool>({true, true, true}));
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

	EXPECT_EQ(state.domains[0].bits, std::vector<bool>({false, true, true}));
	EXPECT_EQ(state.domains[1].bits, std::vector<bool>({true, true, true}));
	EXPECT_EQ(state.domains[2].bits, std::vector<bool>({true, true, true}));
	EXPECT_EQ(state.domains[3].bits, std::vector<bool>({true, true, true}));

	{
		std::size_t var = greedy_pick_variable(state);
		std::size_t val = greedy_pick_value(state, var);
		EXPECT_EQ(0U, var);
		EXPECT_EQ(1U, val);
		bool backtrack = progress_and_backtrack(state, var, val);
		EXPECT_FALSE(backtrack);
	}

	EXPECT_EQ(state.domains[0].bits, std::vector<bool>({false, true, false}));
	EXPECT_EQ(state.domains[1].bits, std::vector<bool>({true, false, true}));
	EXPECT_EQ(state.domains[2].bits, std::vector<bool>({true, false, true}));
	EXPECT_EQ(state.domains[3].bits, std::vector<bool>({true, true, true}));
}

TEST(ProgressBacktrack, progress_loop) {
	nwfc::State state;
	state.domains.push_back(nwfc::BitsetDomain{{true, true, true}});
	state.domains.push_back(nwfc::BitsetDomain{{true, true, true}});
	state.domains.push_back(nwfc::BitsetDomain{{true, true, true}});
	state.domains.push_back(nwfc::BitsetDomain{{true, true, true}});
	state.init();
	state.constraints.emplace_back(new nwfc::AllDifferentBitset({0, 1, 2}));
	state.constraints.emplace_back(new nwfc::CompatibilityBitset(0, 0, {{1,0}, {1,1}, {2,0}, {2,1}}));

	{
		std::size_t var = greedy_pick_variable(state);
		std::size_t val = greedy_pick_value(state, var);
		EXPECT_EQ(0U, var);
		EXPECT_EQ(0U, val);
		bool backtrack = progress_and_backtrack(state, var, val);
		EXPECT_TRUE(backtrack);
	}

	EXPECT_EQ(state.domains[0].bits, std::vector<bool>({false, true, true}));
	EXPECT_EQ(state.domains[1].bits, std::vector<bool>({true, true, true}));
	EXPECT_EQ(state.domains[2].bits, std::vector<bool>({true, true, true}));
	EXPECT_EQ(state.domains[3].bits, std::vector<bool>({true, true, true}));

	{
		std::size_t var = greedy_pick_variable(state);
		std::size_t val = greedy_pick_value(state, var);
		EXPECT_EQ(0U, var);
		EXPECT_EQ(1U, val);
		bool backtrack = progress_and_backtrack(state, var, val);
		EXPECT_FALSE(backtrack);
	}

	EXPECT_EQ(state.domains[0].bits, std::vector<bool>({false, true, false}));
	EXPECT_EQ(state.domains[1].bits, std::vector<bool>({true, false, true}));
	EXPECT_EQ(state.domains[2].bits, std::vector<bool>({true, false, true}));
	EXPECT_EQ(state.domains[3].bits, std::vector<bool>({true, true, true}));
}

