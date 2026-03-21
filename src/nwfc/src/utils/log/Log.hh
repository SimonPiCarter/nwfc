#pragma once

#include <iostream>

struct NullLogger : std::ostream {
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

#define DEBUG_LOG NullLogger::get_instance()
#define INFO_LOG std::cerr
