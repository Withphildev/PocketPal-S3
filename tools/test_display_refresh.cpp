#include "../src/DisplayRefresh.h"
#include <cassert>
#include <cstdio>
int main() {
    using U = DisplayRefresh::Update;
    DisplayRefresh r;
    assert(r.step(0, true, false, false) == U::Full);
    for (uint32_t t=1; t<60000; t+=17) assert(r.step(t, false, false, false)==U::None);
    assert(r.step(60000, true, false, false)==U::Full); // Need/name/control change.
    assert(r.step(60001, false, true, false)==U::Pet);
    assert(r.step(60900, false, true, false)==U::None);
    assert(r.step(60901, false, true, false)==U::Pet);
    assert(r.step(60902, false, false, false)==U::Pet); // Clear movement once.
    assert(r.step(62000, false, false, false)==U::None);
    assert(r.step(62001, false, false, true)==U::Pet);
    assert(r.step(62120, false, false, true)==U::None);
    assert(r.step(62121, false, false, true)==U::Pet);
    assert(r.step(62122, true, false, true)==U::Full);
    assert(r.step(62123, false, false, false)==U::Pet); // Remove expired overlay.
    assert(r.step(64000, false, false, false)==U::None);
    assert(r.step(UINT32_MAX-50, true, false, true)==U::Full);
    assert(r.step(68, false, false, true)==U::None);
    assert(r.step(69, false, false, true)==U::Pet); // Unsigned millis rollover.
    puts("PASS: idle has no timer redraws; visible changes, motion cadence, care cadence/cleanup, and millis rollover.");
}
