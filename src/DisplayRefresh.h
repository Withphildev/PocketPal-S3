#pragma once
#include <stdint.h>

// Application redraw scheduling, not the LCD panel's hardware scan rate.
class DisplayRefresh {
 public:
    enum class Update { None, Full, Pet };
    Update step(uint32_t now, bool changed, bool moving, bool care) {
        const bool activityChanged = moving != moving_ || care != care_;
        moving_ = moving;
        care_ = care;
        const uint32_t interval = care ? 120 : 900;
        if (changed) { last_ = now; return Update::Full; }
        if (activityChanged || ((moving || care) && now - last_ >= interval)) {
            last_ = now;
            return Update::Pet;
        }
        return Update::None;
    }
 private:
    uint32_t last_ = 0;
    bool moving_ = false;
    bool care_ = false;
};
