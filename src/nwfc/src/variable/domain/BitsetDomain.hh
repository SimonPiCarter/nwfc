#pragma once

#include <iostream>
#include <vector>
#include <string>

namespace nwfc {

struct BitsetDomainMemento {
	std::vector<bool> bits;
};

struct BitsetDomain {
	BitsetDomain(std::vector<bool> b, std::string const &i="") : bits(b), explaination(bits.size(), 0), id(i), value(bits.size()) {}

	std::vector<bool> bits;
	std::vector<std::size_t> explaination;
	std::string id;
	std::size_t value;

	typedef BitsetDomainMemento Memento;
};

BitsetDomain::Memento remove_value(BitsetDomain& domain, std::size_t index, std::size_t explaination);
BitsetDomain::Memento remove_all_but_value(BitsetDomain& domain, std::size_t index, std::size_t explaination);
bool is_value_in_domain(const BitsetDomain& domain, std::size_t index);
bool is_decided(const BitsetDomain& domain);
bool is_empty(const BitsetDomain& domain);
void restore(BitsetDomain& domain, const BitsetDomain::Memento& memento);
BitsetDomain::Memento assign_value(BitsetDomain& domain, std::size_t index);
void unnassign_value(BitsetDomain& domain);

std::size_t get_assigned_value(const BitsetDomain& domain);

void display_domain(std::ostream &os, BitsetDomain const &domain);
void display_domain_detailed(std::ostream &os, BitsetDomain const &domain);

std::vector<bool> effective_domain(BitsetDomain const &domain);

}
