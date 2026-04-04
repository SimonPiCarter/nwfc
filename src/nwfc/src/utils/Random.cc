#include "Random.hh"

#ifdef _NOT_GODOT_BUILD_
#include <random>
#else
#include "core/math/random_pcg.h"
#endif

namespace nwfc {

#ifdef _NOT_GODOT_BUILD_

class StdNwfcRandom : public AbstractNwfcRandom {
public:
	StdNwfcRandom() : _gen(42) {}

	std::size_t next_int(std::size_t min, std::size_t max) override {
		std::uniform_int_distribution<std::size_t> dist(min, max);
		return dist(_gen);
	}

	void seed(uint64_t s) override {
		_gen.seed(static_cast<unsigned long>(s));
	}

private:
	std::mt19937 _gen;
};

#else

class PCGNwfcRandom : public AbstractNwfcRandom {
public:
	PCGNwfcRandom() : _rng(42) {}

	std::size_t next_int(std::size_t min, std::size_t max) override {
		return static_cast<std::size_t>(_rng.rand() % (max - min + 1)) + min;
	}

	void seed(uint64_t s) override {
		_rng.seed(s);
	}

private:
	RandomPCG _rng;
};

#endif

NwfcRandom::NwfcRandom()
#ifdef _NOT_GODOT_BUILD_
	: _impl(std::make_shared<StdNwfcRandom>())
#else
	: _impl(std::make_shared<PCGNwfcRandom>())
#endif
{}

std::size_t NwfcRandom::next_int(std::size_t min, std::size_t max) {
	return _impl->next_int(min, max);
}

void NwfcRandom::seed(uint64_t s) {
	_impl->seed(s);
}

} // namespace nwfc
