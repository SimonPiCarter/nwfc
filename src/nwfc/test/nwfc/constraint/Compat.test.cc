#include <gtest/gtest.h>

#include "variable/domain/BitsetDomain.hh"
#include "state/State.hh"
#include "constraint/bitset/CompatibilityBitset.hh"

TEST(CompatibilityBitset, propagate) {
	nwfc::State state;
	state.domains.push_back(nwfc::BitsetDomain{{true, true, true}});
	state.domains.push_back(nwfc::BitsetDomain{{true, true, true}});
	state.domains.push_back(nwfc::BitsetDomain{{true, true, true}});
	state.domains.push_back(nwfc::BitsetDomain{{true, true, true}});
	state.init();

	nwfc::CompatibilityBitset constraint(0, 1, {{1,0}, {1,1}, {2,2}});

	state.mementos.push_back(nwfc::StateMemento{});
	state.mementos.back().domain_mementos.push_back({0, nwfc::assign_value(state.domains[0], 1)});
	constraint.propagate(state, 0);

	EXPECT_EQ(nwfc::effective_domain(state.domains[0]), std::vector<bool>({false, true, false}));
	EXPECT_EQ(nwfc::effective_domain(state.domains[1]), std::vector<bool>({false, false, true}));
	EXPECT_EQ(nwfc::effective_domain(state.domains[2]), std::vector<bool>({true, true, false}));
	EXPECT_EQ(nwfc::effective_domain(state.domains[3]), std::vector<bool>({true, true, true}));
}

