
find_package(GTest)

add_executable(unit_tests)

target_sources(unit_tests PRIVATE
	nwfc/variable/domain/BitsetDomain.test.cc
	nwfc/constraint/AllDiff.test.cc
	nwfc/constraint/Compat.test.cc
	nwfc/state/ProgressBacktrack.test.cc
	nwfc/state/ProgressBacjkump.test.cc
)

target_link_libraries(unit_tests PRIVATE nwfc GTest::gtest_main)
