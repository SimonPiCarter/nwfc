#pragma once

#include <stdexcept>
#include "state/State.hh"

namespace nwfc {

struct ValuePicker {
	virtual std::size_t pick(State const &state, std::size_t var) = 0;
};

struct ConstantValuePicker : public ValuePicker {
	ConstantValuePicker(std::size_t val) : value(val) {}

	const std::size_t value = 0;

	std::size_t pick(State const &state, std::size_t var) override {
		if (is_value_in_domain(state.domains[var], value)) {
			return value;
		}
		//throw std::logic_error("Called pick constant value on a variable with incompatible domain");
		return greedy_pick_value(state, var);
	}
};

}
