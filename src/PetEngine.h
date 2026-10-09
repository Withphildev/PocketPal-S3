#pragma once

#include <Arduino.h>
#include <Preferences.h>

enum class PetAction : uint8_t { Feed, Play, Clean, Sleep, Pet };

struct PetSnapshot {
    String name;
    uint8_t fullness;
    uint8_t happiness;
    uint8_t energy;
    uint8_t cleanliness;
    uint8_t health;
    uint32_t ageMinutes;
    bool sleeping;
    String mood;
    String message;
};

class PetEngine {
  public:
    void begin();
    void tick(uint32_t now);
    bool apply(PetAction action);
    bool apply(const String &actionName);
    bool setName(String name);
    void setSleeping(bool sleeping, const String &message);
    PetSnapshot snapshot() const;
    void save();

  private:
    static constexpr uint32_t kNeedTickMs = 5UL * 60UL * 1000UL;
    static constexpr uint32_t kSaveIntervalMs = 30UL * 1000UL;

    Preferences preferences_;
    String name_ = "Pip";
    uint8_t fullness_ = 82;
    uint8_t happiness_ = 86;
    uint8_t energy_ = 78;
    uint8_t cleanliness_ = 90;
    uint8_t health_ = 100;
    uint32_t ageMinutes_ = 0;
    bool sleeping_ = false;
    bool dirty_ = false;
    uint32_t lastNeedTick_ = 0;
    uint32_t lastAgeTick_ = 0;
    uint32_t lastSave_ = 0;
    String message_ = "A new friendship begins!";

    static uint8_t clampStat(int value);
    void updateHealth();
    String mood() const;
};
