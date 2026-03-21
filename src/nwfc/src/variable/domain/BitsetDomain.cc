#include "BitsetDomain.hh"

#include <algorithm>
#include <stdexcept>

#include "utils/log/Log.hh"

namespace nwfc {

static void display_bitset(std::ostream &os, std::vector<bool> const &bitset) {
	os << "[";
	for (std::size_t i = 0; i < bitset.size(); ++i) {
		if (bitset[i]) os << i << " ";
	}
	os << "]";
}

void display_domain(std::ostream &os, BitsetDomain const &domain) {
	os << domain.id << ": ";
	display_bitset(os, domain.bits);
	os << std::endl;
}

BitsetDomain::Memento remove_value(BitsetDomain& domain, std::size_t index) {
	DEBUG_LOG << "Removing value " << index << " from domain " << domain.id << std::endl;
	BitsetDomain::Memento memento;
	memento.bits = domain.bits;
	DEBUG_LOG << "  before: ";
	display_bitset(DEBUG_LOG, domain.bits);
	if (index < domain.bits.size()) {
		domain.bits[index] = false;
	}
	DEBUG_LOG << "\n  after: ";
	display_bitset(DEBUG_LOG, domain.bits);
	DEBUG_LOG << std::endl;
	return memento;
}

BitsetDomain::Memento remove_all_but_value(BitsetDomain& domain, std::size_t index) {
	DEBUG_LOG << "Removing all but value " << index << " from domain " << domain.id << std::endl;
	BitsetDomain::Memento memento;
	memento.bits = domain.bits;
	DEBUG_LOG << "  before: ";
	display_bitset(DEBUG_LOG, domain.bits);
	for (std::size_t i = 1; i <= index; ++i) {
		domain.bits[index-i] = false;
	}
	for (std::size_t i = index+1; i < domain.bits.size(); ++i) {
		domain.bits[i] = false;
	}
	DEBUG_LOG << "\n  after: ";
	display_bitset(DEBUG_LOG, domain.bits);
	DEBUG_LOG << std::endl;
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
	DEBUG_LOG << "Restoring domain "<< domain.id << std::endl;

	DEBUG_LOG << "  before: ";
	display_bitset(DEBUG_LOG, domain.bits);
	domain.bits = memento.bits;
	DEBUG_LOG << "\n  after: ";
	display_bitset(DEBUG_LOG, domain.bits);
	DEBUG_LOG << std::endl;
}

BitsetDomain::Memento assign_value(BitsetDomain& domain, std::size_t index) {
	DEBUG_LOG << "Assigning value " << index << " to domain " << domain.id << std::endl;
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
	DEBUG_LOG << "get_assigned_value for domain "<< domain.id << std::endl;
	display_bitset(DEBUG_LOG, domain.bits);
	DEBUG_LOG<<std::endl;
	throw std::runtime_error("Tried to get assigned value from empty domain");
}

} // namespace nwfc
