#ifndef TEST_UTILS_HPP
#define TEST_UTILS_HPP

#include <iostream>
#include <string>

extern int g_test_passed;
extern int g_test_failed;

#define ASSERT(condition) \
    do { \
        if (!(condition)) { \
            std::cerr << "[FAIL] " << __FUNCTION__ << " at " << __FILE__ << ":" << __LINE__ \
                      << " -> " << #condition << std::endl; \
            g_test_failed++; \
        } else { \
            g_test_passed++; \
        } \
    } while (0)

#endif // TEST_UTILS_HPP
