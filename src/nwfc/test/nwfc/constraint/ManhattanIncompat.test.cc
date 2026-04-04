#include <gtest/gtest.h>

#include "variable/domain/BitsetDomain.hh"
#include "state/State.hh"
#include "constraint/bitset/ManhattanIncompatibilityBitset.hh"

// Helpers to build a state with `n` variables, each having `num_values` possible values.
static nwfc::State make_state(std::size_t n, std::size_t num_values) {
	nwfc::State state;
	for (std::size_t i = 0; i < n; ++i) {
		state.domains.push_back(nwfc::BitsetDomain{std::vector<bool>(num_values, true)});
	}
	state.init();
	state.mementos.push_back(nwfc::StateMemento{});
	return state;
}

static void assign(nwfc::State &state, std::size_t var, std::size_t val) {
	state.mementos.back().domain_mementos.push_back({var, nwfc::assign_value(state.domains[var], val)});
}

// 3x3 grid (width=3, height=3):
//  0 1 2
//  3 4 5
//  6 7 8
// Values: 0=trigger, 1=incompatible, 2=neutral

TEST(ManhattanIncompatibilityBitset, trigger_assigned_removes_incompatible_from_manhattan_neighbors) {
	// Assign center cell (4) trigger_value=0 with distance=1.
	// Manhattan-1 neighbors of (1,1): right=(2,1)=5, left=(0,1)=3, up=(1,0)=1, down=(1,2)=7.
	// Diagonal cells (0,2,6,8) are at distance 2 and must be untouched.
	nwfc::State state = make_state(9, 3);
	nwfc::ManhattanIncompatibilityBitset constraint(3, 3, 1, /*trigger=*/0, {/*incompatible=*/1});

	assign(state, 4, 0);
	constraint.propagate(state, 4);

	// Direct neighbors: value 1 removed
	EXPECT_EQ(nwfc::effective_domain(state.domains[1]), std::vector<bool>({true, false, true}));
	EXPECT_EQ(nwfc::effective_domain(state.domains[3]), std::vector<bool>({true, false, true}));
	EXPECT_EQ(nwfc::effective_domain(state.domains[5]), std::vector<bool>({true, false, true}));
	EXPECT_EQ(nwfc::effective_domain(state.domains[7]), std::vector<bool>({true, false, true}));

	// Diagonal cells at distance 2: unchanged
	EXPECT_EQ(nwfc::effective_domain(state.domains[0]), std::vector<bool>({true, true, true}));
	EXPECT_EQ(nwfc::effective_domain(state.domains[2]), std::vector<bool>({true, true, true}));
	EXPECT_EQ(nwfc::effective_domain(state.domains[6]), std::vector<bool>({true, true, true}));
	EXPECT_EQ(nwfc::effective_domain(state.domains[8]), std::vector<bool>({true, true, true}));
}

TEST(ManhattanIncompatibilityBitset, incompatible_assigned_removes_trigger_from_manhattan_neighbors) {
	// Assign center cell (4) incompatible_value=1 with distance=1.
	// The trigger_value=0 must be removed from all Manhattan-1 neighbors.
	nwfc::State state = make_state(9, 3);
	nwfc::ManhattanIncompatibilityBitset constraint(3, 3, 1, /*trigger=*/0, {/*incompatible=*/1});

	assign(state, 4, 1);
	constraint.propagate(state, 4);

	EXPECT_EQ(nwfc::effective_domain(state.domains[1]), std::vector<bool>({false, true, true}));
	EXPECT_EQ(nwfc::effective_domain(state.domains[3]), std::vector<bool>({false, true, true}));
	EXPECT_EQ(nwfc::effective_domain(state.domains[5]), std::vector<bool>({false, true, true}));
	EXPECT_EQ(nwfc::effective_domain(state.domains[7]), std::vector<bool>({false, true, true}));

	// Diagonal cells: unchanged
	EXPECT_EQ(nwfc::effective_domain(state.domains[0]), std::vector<bool>({true, true, true}));
	EXPECT_EQ(nwfc::effective_domain(state.domains[2]), std::vector<bool>({true, true, true}));
	EXPECT_EQ(nwfc::effective_domain(state.domains[6]), std::vector<bool>({true, true, true}));
	EXPECT_EQ(nwfc::effective_domain(state.domains[8]), std::vector<bool>({true, true, true}));
}

TEST(ManhattanIncompatibilityBitset, neutral_value_does_not_propagate) {
	// Assigning a value that is neither trigger nor incompatible must leave all domains unchanged.
	nwfc::State state = make_state(9, 3);
	nwfc::ManhattanIncompatibilityBitset constraint(3, 3, 1, /*trigger=*/0, {/*incompatible=*/1});

	assign(state, 4, 2);
	constraint.propagate(state, 4);

	for (std::size_t i = 0; i < 9; ++i) {
		if (i == 4) continue;
		EXPECT_EQ(nwfc::effective_domain(state.domains[i]), std::vector<bool>({true, true, true})) << "cell " << i;
	}
}

TEST(ManhattanIncompatibilityBitset, corner_cell_only_affects_valid_in_bounds_neighbors) {
	// Assign top-left corner (cell 0, x=0, y=0) trigger_value=0 with distance=1.
	// Only in-bounds Manhattan-1 neighbors: right=(1,0)=1, down=(0,1)=3.
	// No out-of-bounds access; cells 2,4,5,6,7,8 untouched.
	nwfc::State state = make_state(9, 3);
	nwfc::ManhattanIncompatibilityBitset constraint(3, 3, 1, /*trigger=*/0, {/*incompatible=*/1});

	assign(state, 0, 0);
	constraint.propagate(state, 0);

	EXPECT_EQ(nwfc::effective_domain(state.domains[1]), std::vector<bool>({true, false, true}));
	EXPECT_EQ(nwfc::effective_domain(state.domains[3]), std::vector<bool>({true, false, true}));

	for (std::size_t i : {2u, 4u, 5u, 6u, 7u, 8u}) {
		EXPECT_EQ(nwfc::effective_domain(state.domains[i]), std::vector<bool>({true, true, true})) << "cell " << i;
	}
}

TEST(ManhattanIncompatibilityBitset, distance_2_reaches_further_cells) {
	// 5x1 grid: cells 0 1 2 3 4. Assign cell 2 trigger_value=0 with distance=2.
	// All four other cells are within Manhattan distance 2 along a single row.
	nwfc::State state = make_state(5, 3);
	nwfc::ManhattanIncompatibilityBitset constraint(5, 1, 2, /*trigger=*/0, {/*incompatible=*/1});

	assign(state, 2, 0);
	constraint.propagate(state, 2);

	for (std::size_t i : {0u, 1u, 3u, 4u}) {
		EXPECT_EQ(nwfc::effective_domain(state.domains[i]), std::vector<bool>({true, false, true})) << "cell " << i;
	}
}

TEST(ManhattanIncompatibilityBitset, trigger_value_same_as_incompatible_value) {
	// trigger_value == incompatible_value == 0: "no two cells within distance may share value 0".
	// When cell 4 is assigned 0, value 0 must be removed from all Manhattan-1 neighbours.
	// The is_trigger branch is taken (not is_incompatible-only), so there is no double removal.
	nwfc::State state = make_state(9, 2);
	nwfc::ManhattanIncompatibilityBitset constraint(3, 3, 1, /*trigger=*/0, {/*incompatible=*/0});

	assign(state, 4, 0);
	constraint.propagate(state, 4);

	// Direct neighbours: value 0 removed
	EXPECT_EQ(nwfc::effective_domain(state.domains[1]), std::vector<bool>({false, true}));
	EXPECT_EQ(nwfc::effective_domain(state.domains[3]), std::vector<bool>({false, true}));
	EXPECT_EQ(nwfc::effective_domain(state.domains[5]), std::vector<bool>({false, true}));
	EXPECT_EQ(nwfc::effective_domain(state.domains[7]), std::vector<bool>({false, true}));

	// Diagonal cells at distance 2: unchanged
	EXPECT_EQ(nwfc::effective_domain(state.domains[0]), std::vector<bool>({true, true}));
	EXPECT_EQ(nwfc::effective_domain(state.domains[2]), std::vector<bool>({true, true}));
	EXPECT_EQ(nwfc::effective_domain(state.domains[6]), std::vector<bool>({true, true}));
	EXPECT_EQ(nwfc::effective_domain(state.domains[8]), std::vector<bool>({true, true}));
}

TEST(ManhattanIncompatibilityBitset, already_assigned_neighbor_is_skipped) {
	// Pre-assign cell 1 (neighbor of cell 4) before propagating from cell 4.
	// Propagation must not touch an already-assigned domain.
	nwfc::State state = make_state(9, 3);
	nwfc::ManhattanIncompatibilityBitset constraint(3, 3, 1, /*trigger=*/0, {/*incompatible=*/1});

	assign(state, 1, 1); // pre-assign neighbor; domain is now decided
	assign(state, 4, 0);
	constraint.propagate(state, 4);

	// Cell 1 was assigned value 1; propagation must not alter its decided domain.
	EXPECT_TRUE(nwfc::is_decided(state.domains[1]));
	EXPECT_EQ(nwfc::get_assigned_value(state.domains[1]), 1u);

	// Other unassigned neighbors must still be affected.
	EXPECT_EQ(nwfc::effective_domain(state.domains[3]), std::vector<bool>({true, false, true}));
	EXPECT_EQ(nwfc::effective_domain(state.domains[5]), std::vector<bool>({true, false, true}));
	EXPECT_EQ(nwfc::effective_domain(state.domains[7]), std::vector<bool>({true, false, true}));
}
