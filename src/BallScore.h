#pragma once
#include <stddef.h>
#include <stdint.h>

inline bool parseBallScore(const char *text, size_t length, uint32_t &score) {
    if (!length || length > 5) return false;
    uint32_t value = 0;
    for (size_t i = 0; i < length; ++i) {
        if (text[i] < '0' || text[i] > '9') return false;
        value = value * 10 + (text[i] - '0');
    }
    score = value;
    return true;
}
