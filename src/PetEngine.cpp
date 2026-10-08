#include "PetEngine.h"

#include <algorithm>

namespace {
constexpr char kNamespace[] = "pocketpal";
}

uint8_t PetEngine::clampStat(int value) {
    return static_cast<uint8_t>(std::max(0, std::min(100, value)));
}

void PetEngine::begin() {
    preferences_.begin(kNamespace, false);
    name_ = preferences_.getString("name", "Pip");
    fullness_ = preferences_.getUChar("full", 82);
    happiness_ = preferences_.getUChar("happy", 86);
    energy_ = preferences_.getUChar("energy", 78);
    cleanliness_ = preferences_.getUChar("clean", 90);
    health_ = preferences_.getUChar("health", 100);
    ageMinutes_ = preferences_.getUInt("age", 0);
    sleeping_ = preferences_.getBool("sleeping", false);
    lastNeedTick_ = millis();
    lastAgeTick_ = millis();
    lastSave_ = millis();
    message_ = "Welcome back, " + name_ + "!";
}

void PetEngine::tick(uint32_t now) {
    while (now - lastAgeTick_ >= 60000UL) {
        ++ageMinutes_;
        lastAgeTick_ += 60000UL;
        dirty_ = true;
    }
    while (now - lastNeedTick_ >= kNeedTickMs) {
        fullness_ = clampStat(fullness_ - (sleeping_ ? 1 : 2));
        happiness_ = clampStat(happiness_ - (sleeping_ ? 0 : 1));
        cleanliness_ = clampStat(cleanliness_ - 1);
        energy_ = clampStat(energy_ + (sleeping_ ? 7 : -2));
        updateHealth();
        lastNeedTick_ += kNeedTickMs;
        dirty_ = true;
    }
    if (dirty_ && now - lastSave_ >= kSaveIntervalMs) save();
}

bool PetEngine::apply(PetAction action) {
    switch (action) {
        case PetAction::Feed:
            sleeping_ = false;
            fullness_ = clampStat(fullness_ + 24);
            cleanliness_ = clampStat(cleanliness_ - 3);
            message_ = "That was delicious!";
            break;
        case PetAction::Play:
            sleeping_ = false;
            if (energy_ < 8) {
                message_ = "Too sleepy to play.";
                return false;
            }
            happiness_ = clampStat(happiness_ + 22);
            energy_ = clampStat(energy_ - 10);
            fullness_ = clampStat(fullness_ - 4);
            message_ = "Whee! Let's do that again!";
            break;
        case PetAction::Clean:
            cleanliness_ = clampStat(cleanliness_ + 38);
            happiness_ = clampStat(happiness_ + 2);
            message_ = "Fresh and sparkly!";
            break;
        case PetAction::Sleep:
            sleeping_ = !sleeping_;
            message_ = sleeping_ ? "Good night... zzz" : "Good morning!";
            break;
        case PetAction::Pet:
            sleeping_ = false;
            happiness_ = clampStat(happiness_ + 12);
            message_ = "You're my favorite human.";
            break;
    }
    updateHealth();
    dirty_ = true;
    save();
    return true;
}

bool PetEngine::apply(const String &actionName) {
    if (actionName == "feed") return apply(PetAction::Feed);
    if (actionName == "play") return apply(PetAction::Play);
    if (actionName == "clean") return apply(PetAction::Clean);
    if (actionName == "sleep") return apply(PetAction::Sleep);
    if (actionName == "pet") return apply(PetAction::Pet);
    return false;
}

bool PetEngine::setName(String name) {
    name.trim();
    if (name.isEmpty() || name.length() > 12) return false;
    for (size_t i = 0; i < name.length(); ++i) {
        const char c = name[i];
        if (!isalnum(static_cast<unsigned char>(c)) && c != ' ' && c != '-' && c != '_') return false;
    }
    name_ = name;
    message_ = "My name is " + name_ + "!";
    dirty_ = true;
    save();
    return true;
}

void PetEngine::updateHealth() {
    const int average = (fullness_ + happiness_ + energy_ + cleanliness_) / 4;
    if (average < 25) health_ = clampStat(health_ - 4);
    else if (average < 45) health_ = clampStat(health_ - 1);
    else if (average > 75) health_ = clampStat(health_ + 2);
}

String PetEngine::mood() const {
    if (sleeping_) return "sleeping";
    if (health_ < 35) return "unwell";
    if (fullness_ < 28) return "hungry";
    if (cleanliness_ < 28) return "messy";
    if (energy_ < 25) return "sleepy";
    if (happiness_ > 78) return "joyful";
    if (happiness_ < 38) return "lonely";
    return "content";
}

PetSnapshot PetEngine::snapshot() const {
    return {name_, fullness_, happiness_, energy_, cleanliness_, health_, ageMinutes_, sleeping_, mood(), message_};
}

void PetEngine::save() {
    preferences_.putString("name", name_);
    preferences_.putUChar("full", fullness_);
    preferences_.putUChar("happy", happiness_);
    preferences_.putUChar("energy", energy_);
    preferences_.putUChar("clean", cleanliness_);
    preferences_.putUChar("health", health_);
    preferences_.putUInt("age", ageMinutes_);
    preferences_.putBool("sleeping", sleeping_);
    dirty_ = false;
    lastSave_ = millis();
}

