#pragma once

#include <cstdint>
#include <cstddef>
#include <memory>

namespace nwfc {

class AbstractNwfcRandom {
public:
	virtual ~AbstractNwfcRandom() = default;
	virtual std::size_t next_int(std::size_t min, std::size_t max) = 0;
	virtual void seed(uint64_t s) = 0;
};

/// Platform-independent random number generator.
/// Godot build uses RandomPCG for cross-platform determinism.
/// Standalone build uses std::mt19937 (for tests only).
class NwfcRandom {
public:
	NwfcRandom();

	/// Returns a random integer in [min, max] (inclusive).
	std::size_t next_int(std::size_t min, std::size_t max);

	void seed(uint64_t s);

private:
	std::shared_ptr<AbstractNwfcRandom> _impl;
};

} // namespace nwfc
