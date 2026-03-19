
add_library(nwfc)

target_sources(nwfc PRIVATE
	src/variable/domain/BitsetDomain.cc
	src/state/State.cc
)

target_include_directories(nwfc PUBLIC src/)

add_subdirectory(test)
