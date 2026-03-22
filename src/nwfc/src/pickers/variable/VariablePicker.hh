#pragma once

namespace nwfc {

struct ValuePicker;

struct VariablePicker {
	virtual bool is_done(State const &state) = 0;
	virtual std::size_t pick(State const &state) = 0;
	virtual ValuePicker* get_value_picker() = 0;
};

} // namespace nwfc