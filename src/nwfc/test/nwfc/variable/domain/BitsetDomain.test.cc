#include <gtest/gtest.h>

#include "variable/domain/BitsetDomain.hh"

TEST(BitsetDomainTest, value_evolution) {
	nwfc::BitsetDomain domain {{true, true, true}};

	EXPECT_EQ(domain.bits, std::vector<bool>({true, true, true}));
	auto memento = nwfc::remove_value(domain, 1, 0);
	EXPECT_EQ(domain.bits, std::vector<bool>({true, false, true}));
	nwfc::restore(domain, memento);
	EXPECT_EQ(domain.bits, std::vector<bool>({true, true, true}));
}

TEST(BitsetDomainTest, is_value_in_domain) {
	nwfc::BitsetDomain domain {{true, false, true}};

	EXPECT_TRUE(nwfc::is_value_in_domain(domain, 0));
	EXPECT_FALSE(nwfc::is_value_in_domain(domain, 1));
	EXPECT_TRUE(nwfc::is_value_in_domain(domain, 2));
}

TEST(BitsetDomainTest, is_decided) {
	nwfc::BitsetDomain domain {{true, true, false}};

	EXPECT_FALSE(nwfc::is_decided(domain));
	nwfc::remove_value(domain, 1, 0);
	EXPECT_FALSE(nwfc::is_decided(domain));
	nwfc::assign_value(domain, 0);
	EXPECT_TRUE(nwfc::is_decided(domain));
}

TEST(BitsetDomainTest, is_empty) {
	nwfc::BitsetDomain domain {{false, false, false}};

	EXPECT_TRUE(nwfc::is_empty(domain));
}
