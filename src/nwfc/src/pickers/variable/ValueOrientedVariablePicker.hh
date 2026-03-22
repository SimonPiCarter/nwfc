#pragma once

#include "VariablePicker.hh"
#include "pickers/value/ValuePicker.hh"

namespace nwfc {
struct ValueOrientedVariablePicker : public VariablePicker {
	ValueOrientedVariablePicker(std::size_t count, std::size_t value) : _count(count), _value(value), value_picker(value) {}
	bool is_done(State const &state) override {
		std::size_t count = 0;
		std::size_t alternatives = 0;
		for (auto &domain : state.domains) {
			if (is_decided(domain) && get_assigned_value(domain) == _value) {
				count++;
			}
			if (!is_decided(domain) && is_value_in_domain(domain, _value)) {
				alternatives++;
			}
		}
		return alternatives == 0 || _count <= count;
	}
	std::size_t pick(State const &state) override {
		std::vector<std::size_t> candidates;
		for (std::size_t idx = 0 ; idx < state.domains.size() ; ++idx) {
			auto &domain = state.domains[idx];
			if (!is_decided(domain) && is_value_in_domain(domain, _value)) {
				candidates.push_back(idx);
			}
		}
		return candidates[rand() % candidates.size()];
	}

	virtual ValuePicker* get_value_picker() override {
		return &value_picker;
	}
private:
	const std::size_t _count;
	const std::size_t _value;
	ConstantValuePicker value_picker;
};
} // namespace nwfc
