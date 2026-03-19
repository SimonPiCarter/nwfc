#pragma once

#include <vector>

namespace nwfc {

struct BitsetDomainMemento {
	std::vector<bool> bits;
};

struct BitsetDomain {
	std::vector<bool> bits;

	typedef BitsetDomainMemento Memento;
};

BitsetDomain::Memento remove_value(BitsetDomain& domain, std::size_t index);
bool is_value_in_domain(const BitsetDomain& domain, std::size_t index);
bool is_decided(const BitsetDomain& domain);
bool is_empty(const BitsetDomain& domain);
void restore(BitsetDomain& domain, const BitsetDomain::Memento& memento);
BitsetDomain::Memento assign_value(BitsetDomain& domain, std::size_t index);

std::size_t get_assigned_value(const BitsetDomain& domain);

}
