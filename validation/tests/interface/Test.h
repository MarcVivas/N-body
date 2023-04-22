#include <cstddef>
#ifndef N_BODY_TEST_H
#define N_BODY_TEST_H

class Test{
public:
    virtual ~Test() = default;
    Test() = default;
    virtual void runTest(const size_t iterations, const double stepSize) = 0;
};
#endif //N_BODY_TEST_H
