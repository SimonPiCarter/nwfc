#pragma once

#include <vector>

namespace nwfc {
struct State;

struct WeightBasedValuePicker {
	public:
	WeightBasedValuePicker(std::size_t num_vars);

	std::size_t pick(State const &state, std::size_t var);

	void set_weiht(std::vector<std::size_t> const &w);
	void set_weight(std::size_t val, std::size_t w);
private:
	std::vector<std::size_t> weight;
	std::size_t n;
};

} // namespace nwfc