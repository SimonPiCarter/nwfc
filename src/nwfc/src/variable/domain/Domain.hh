#pragma once

#include <variable/domain/BitsetDomain.hh>

#include <variant>

namespace nwfc {

using Domain = BitsetDomain;
using DomainMemento = BitsetDomain::Memento;

struct DomainMementoIdx {
	std::size_t index;
	DomainMemento memento;
};

}
