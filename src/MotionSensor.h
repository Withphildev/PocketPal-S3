#pragma once

#include <Arduino.h>
#include <M5Unified.h>
#include <Preferences.h>

enum class MotionState : uint8_t {
    Unavailable,
    Calibrating,
    Still,
    TiltLeft,
    TiltRight,
    TiltForward,
    TiltBack,
    Shake,
    Rocking,
};

struct MotionSnapshot {
    MotionState state;
    String stateName;
    uint8_t meter;
    uint8_t sensitivity;
    bool available;
    bool calibrated;
};

class MotionSensor {
  public:
    void begin();
    void tick(uint32_t now);
    bool setSensitivity(uint8_t sensitivity);
    MotionSnapshot snapshot() const;
    MotionState state() const;
    bool consumeShake();
    bool consumeRocking();
    static const char *stateName(MotionState state);
    static const char *shortLabel(MotionState state);

  private:
    static constexpr uint32_t kSampleIntervalMs = 40;
    static constexpr uint8_t kCalibrationSamples = 25;

    Preferences preferences_;
    bool available_ = false;
    bool calibrated_ = false;
    bool shakePending_ = false;
    bool rockingPending_ = false;
    uint8_t sensitivity_ = 2;
    uint8_t calibrationCount_ = 0;
    uint8_t meter_ = 0;
    uint8_t shakeHits_ = 0;
    uint8_t rockAlternations_ = 0;
    int8_t lastRockSign_ = 0;
    float gravityX_ = 0;
    float gravityY_ = 0;
    float gravityZ_ = 1;
    float previousX_ = 0;
    float previousY_ = 0;
    float previousZ_ = 1;
    MotionState state_ = MotionState::Unavailable;
    uint32_t nextSampleAt_ = 0;
    uint32_t lastShakeAt_ = 0;
    uint32_t lastRockAt_ = 0;
    uint32_t lastRockCrossingAt_ = 0;
    uint32_t eventUntil_ = 0;

    void classifyTilt();
    void updateRocking(uint32_t now, float dynamic, float shakeThreshold);
};
