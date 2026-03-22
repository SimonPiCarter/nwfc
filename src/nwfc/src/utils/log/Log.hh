#pragma once

#include <iostream>
#include <sstream>

struct NullLogger : std::stringstream {
    template<typename T>
    NullLogger& operator<<(const T&) {
        return *this;
    }

    // Handle std::endl and other stream manipulators
    NullLogger& operator<<(std::ostream& (*)(std::ostream&)) {
        return *this;
    }

    static NullLogger &get_instance() {
        return instance;
    }
private:
    NullLogger() = default;
    static NullLogger instance;
};

// #define DEBUG_LEVEL

#ifndef DEBUG_LEVEL
#define DEBUG_LOG NullLogger::get_instance()
#else
#define DEBUG_LOG std::cerr
#endif
#define INFO_LOG std::cerr
