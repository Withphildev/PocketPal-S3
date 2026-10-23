#include "../src/BallScore.h"
#include <cassert>
#include <cstring>
#include <cstdio>
#include <initializer_list>
int main() {
    uint32_t value = 42;
    for (const auto input : {"", "-1", "+1", "1.5", "1e3", " 3", "3 ", "100000", "abc"}) {
        assert(!parseBallScore(input, strlen(input), value));
        assert(value==42);
    }
    for (const auto input : {"0", "1", "123", "99999", "00001"}) assert(parseBallScore(input,strlen(input),value));
    assert(value==1);
    puts("PASS: score bounds, numeric validation, and invalid inputs do not mutate results.");
}
