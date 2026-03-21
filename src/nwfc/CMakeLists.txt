
add_library(nwfc)

target_sources(nwfc PRIVATE
	src/utils/log/Log.cc
	src/pickers/value/WeightBasedValuePicker.cc
	src/variable/domain/BitsetDomain.cc
	src/variable/layout/2d/GridLayout.cc
	src/state/State.cc
)

target_include_directories(nwfc PUBLIC src/)

add_subdirectory(test)
