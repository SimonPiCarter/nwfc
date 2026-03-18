#pragma once

#include <BitsetDomain.hh>

#include <variant>

namespace nwfc {

using Domain = std::variant<BitsetDomain>;
using DomainMemento = std::variant<BitsetDomain::Memento>;

DomainMemento remove_value(Domain& domain, size_t index);
bool is_value_in_domain(const Domain& domain, size_t index);
bool is_decided(const Domain& domain);
bool is_empty(const Domain& domain);
void restore(Domain& domain, const DomainMemento& memento);

}
