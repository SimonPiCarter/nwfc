# nwfc — Copilot Instructions

`nwfc` is a **constraint programming solver** (not a wave function collapse library). It assigns values to variables while satisfying constraints, using backtracking and conflict-directed backjumping.

## Build & Test

Dependencies are managed with **Conan 2**, build files live in `builds/Debug/`.

```bash
# Activate the Python virtual env before any conan command
source ~/venv/bin/activate

# First-time setup (from repo root)
conan install . --build=missing -of builds/Debug -s build_type=Debug -if builds/Debug/

# Configure & build (from builds/Debug)
cmake ../.. -G "Unix Makefiles" -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake -DCMAKE_POLICY_DEFAULT_CMP0091=NEW -DCMAKE_BUILD_TYPE=Debug
cmake --build . --parallel

# Run all tests (from root directory)
./builds/Debug/src/nwfc/test/unit_tests

# Run a single test (GTest filter)
./builds/Debug/src/nwfc/test/unit_tests --gtest_filter=BitsetDomainTest.*

# Run the demo executable (from root directory)
./builds/Debug/src/exe/exenwfc
```

## Architecture

The solver is built around three main concepts:

**`State`** (`src/nwfc/src/state/State.hh`) — central mutable state:
- `domains`: one `BitsetDomain` per variable
- `constraints`: owned `unique_ptr<Constraint>` list, all propagated on each assignment
- `mementos`: a `std::list<StateMemento>` stack enabling undo; each `progress()` call pushes a new memento layer

**`BitsetDomain`** (`src/nwfc/src/variable/domain/BitsetDomain.hh`) — domain of a variable:
- `bits[i]` = true means value `i` is still possible
- `explaination[i]` stores which *variable index* caused value `i` to be removed (used for conflict analysis in backjumping)
- `Domain` in `Domain.hh` is just a `using` alias for `BitsetDomain`

**`Constraint`** (`src/nwfc/src/constraint/Constraint.hh`) — abstract interface:
- Single method: `propagate(State&, size_t variable_assigned)`
- Must write all domain changes as `DomainMementoIdx` entries into `state.mementos.back().domain_mementos` so backtracking can undo them

Available constraints:
| Class | File | Description |
|---|---|---|
| `AllDifferentBitset` | `constraint/bitset/AllDifferentBitset.hh` | All variables in a set must take different values |
| `CompatibilityBitset` | `constraint/bitset/CompatibilityBitset.hh` | Per-(variable, value) pair: when assigned, removes incompatible (variable, value) pairs |
| `CardinalityBitset` | `constraint/bitset/CardinalityBitset.hh` | Lower/upper bound on how many variables may take a given value; use static factories |
| `ManhattanIncompatibilityBitset` | `constraint/bitset/ManhattanIncompatibilityBitset.hh` | Efficient single-object replacement for O(W×H×D²) `CompatibilityBitset` objects on a grid |

**`GridLayout`** (`src/nwfc/src/variable/layout/2d/GridLayout.hh`) — 2D helpers where `index = y * width + x`. Required assumption by `ManhattanIncompatibilityBitset`.

**Solve loop pattern** (see `src/exe/main.cpp`):
```cpp
state.init();
// add constraints to state.constraints with emplace_back(new ...)
size_t var = greedy_pick_variable(state);
while (var < state.domains.size()) {
    size_t val = value_picker.pick(state, var);
    progress(state, var, val);   // assign + propagate
    backjump(state);             // undo if any domain emptied
    var = greedy_pick_variable(state);
}
```

## Key Conventions

- All library code is in the `nwfc` namespace; the `nwfc` static library target is defined in `src/nwfc/CMakeLists.txt`.
- All headers use `#pragma once`.
- Include paths are relative to `src/nwfc/src/` (that directory is the public include root).
- **New constraints** must: inherit `Constraint`, write domain removals via `remove_value()`/`remove_all_but_value()` into `state.mementos.back().domain_mementos`, and be pushed into `state.constraints` with `emplace_back(new ...)`.
- `CardinalityBitset` is constructed only via its static factory methods (`newLowerCardinality`, `newUpperCardinality`, `newCardinality`).
- Logging: `DEBUG_LOG` is a no-op by default; uncomment `#define DEBUG_LEVEL` in `Log.hh` to redirect it to `std::cerr`. `INFO_LOG` always writes to `std::cerr`.
- The word `explaination` (typo for "explanation") is used throughout the codebase — keep this spelling for consistency.
- Test files are named `*.test.cc` and live under `src/nwfc/test/nwfc/`.
