#include "BitsetDomain.hh"

#include <algorithm>
#include <stdexcept>

namespace nwfc {

BitsetDomain::Memento remove_value(BitsetDomain& domain, std::size_t index) {
	BitsetDomain::Memento memento;
	memento.bits = domain.bits;
	if (index < domain.bits.size()) {
		domain.bits[index] = false;
	}
	return memento;
}

bool is_value_in_domain(const BitsetDomain& domain, std::size_t index) {
	if (index < domain.bits.size()) {
		return domain.bits[index];
	}
	return false;
}

bool is_decided(const BitsetDomain& domain) {
	return std::count(domain.bits.begin(), domain.bits.end(), true) == 1;
}

bool is_empty(const BitsetDomain& domain) {
	return std::count(domain.bits.begin(), domain.bits.end(), true) == 0;
}

void restore(BitsetDomain& domain, const BitsetDomain::Memento& memento) {
	domain.bits = memento.bits;
}

BitsetDomain::Memento assign_value(BitsetDomain& domain, std::size_t index) {
	BitsetDomain::Memento memento;
	memento.bits = domain.bits;
	for (std::size_t i = 0; i < domain.bits.size(); ++i) {
		domain.bits[i] = (i == index);
	}
	return memento;
}

std::size_t get_assigned_value(const BitsetDomain& domain) {
	for (std::size_t i = 0; i < domain.bits.size(); ++i) {
		if (domain.bits[i]) {
			return i;
		}
	}
	throw std::runtime_error("Domain is not decided");
}

} // namespace nwfc
