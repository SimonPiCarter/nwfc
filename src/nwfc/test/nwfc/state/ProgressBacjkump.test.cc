#include <gtest/gtest.h>

#include "variable/domain/BitsetDomain.hh"
#include "state/State.hh"
#include "constraint/bitset/AllDifferentBitset.hh"
#include "constraint/bitset/CompatibilityBitset.hh"
#include "constraint/bitset/CardinalityBitset.hh"
#include "variable/layout/2d/GridLayout.hh"
#include "utils/log/Log.hh"

TEST(ProgressBacjkump, progress) {
	nwfc::State state;
	state.domains.push_back(nwfc::BitsetDomain{{true, true, true}, "0"});
	state.domains.push_back(nwfc::BitsetDomain{{true, true, true}, "1"});
	state.domains.push_back(nwfc::BitsetDomain{{true, true, true}, "2"});
	state.domains.push_back(nwfc::BitsetDomain{{true, true, true}, "3"});
	state.init();
	state.constraints.emplace_back(new nwfc::CompatibilityBitset(0, 0, {{3,0}, {3,1}, {2,0}, {2,1}}));
	state.constraints.emplace_back(new nwfc::CompatibilityBitset(2, 2, {{3,2}}));

	{
		bool backtrack = progress_and_backtrack(state, 0, 0);
		EXPECT_FALSE(backtrack);
	}

	EXPECT_EQ(nwfc::effective_domain(state.domains[0]), std::vector<bool>({true, false, false}));
	EXPECT_EQ(nwfc::effective_domain(state.domains[1]), std::vector<bool>({true, true, true}));
	EXPECT_EQ(nwfc::effective_domain(state.domains[2]), std::vector<bool>({false, false, true}));
	EXPECT_EQ(nwfc::effective_domain(state.domains[3]), std::vector<bool>({false, false, true}));

	progress(state, 1, 1);
	EXPECT_FALSE(backjump(state));

	progress(state, 2, 2);
	for(auto dom : state.domains) {
		display_domain_detailed(INFO_LOG, dom);
	}
	EXPECT_TRUE(backjump(state));
}
