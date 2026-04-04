#include "WeightBasedValuePicker.hh"

#include "state/State.hh"
#include "utils/log/Log.hh"

namespace nwfc {

WeightBasedValuePicker::WeightBasedValuePicker(std::size_t num_vars) : weight(num_vars, 1), n(num_vars) {}

void WeightBasedValuePicker::set_weiht(std::vector<std::size_t> const &w) {
	n = 0;
	for(std::size_t i : w) {
		n += i;
	}
	weight = w;
}
void WeightBasedValuePicker::set_weight(std::size_t val, std::size_t w) {
	n += val - weight[val];
	weight[val] = w;
}

std::size_t WeightBasedValuePicker::pick(State const &state, std::size_t var) {
	std::size_t prob_cardinality = 0;
	std::vector<std::size_t> alternatives;
	std::vector<std::size_t> alternatives_weight;
	alternatives.reserve(weight.size());
	alternatives_weight.reserve(weight.size());
	for (size_t i = 0 ; i < weight.size() ; ++ i) {
		if (is_value_in_domain(state.domains[var], i)) {
			prob_cardinality += weight[i];
			alternatives.push_back(i);
			alternatives_weight.push_back(weight[i]);
		}
	}
	DEBUG_LOG<<"prob_cardinality: "<<prob_cardinality<<std::endl;
	DEBUG_LOG<<"alternatives_weight.size(): "<<alternatives_weight.size()<<std::endl;
	auto prob = state.generator.next_int(0, prob_cardinality-1);
	DEBUG_LOG<<"prob: "<<prob<<std::endl;
	std::size_t cur = 0;
	while (cur < alternatives_weight.size() && prob > alternatives_weight[cur]) {
		prob -= alternatives_weight[cur];
		++cur;
	}
	DEBUG_LOG<<"cur: "<<cur<<std::endl;
	return alternatives[cur];
}

} // namespace nwfc
