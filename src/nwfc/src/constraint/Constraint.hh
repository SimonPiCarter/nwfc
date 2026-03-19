#pragma once

#include "variable/domain/Domain.hh"

namespace nwfc {
	struct State;

	struct Constraint {
		virtual ~Constraint() = default;

		virtual void propagate(State &state, std::size_t variable_assigned) = 0;
	};

} // namespace nwfc
