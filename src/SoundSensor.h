#pragma once

#include <Arduino.h>
#include <M5Unified.h>
#include <Preferences.h>

enum class SoundLevel : uint8_t {
    Unavailable,
    Muted,
    Calibrating,
    Quiet,
    Low,
    Medium,
    High,
};

struct SoundSnapshot {
    SoundLevel level;
    String levelName;
    uint8_t meter;
    uint8_t sensitivity;
    uint16_t rms;
    uint16_t noiseFloor;
    bool available;
    bool muted;
    bool calibrated;
};

class SoundSensor {
  public:
    void begin();
    void tick(uint32_t now);
    void setMuted(bool muted);
    void toggleMuted();
    bool setSensitivity(uint8_t sensitivity);
    SoundSnapshot snapshot() const;
    SoundLevel level() const;
    static const char *levelName(SoundLevel level);
    static const char *shortLabel(SoundLevel level);

  private:
    static constexpr size_t kSampleCount = 128;
    static constexpr uint32_t kSampleRate = 8000;
    static constexpr uint32_t kSampleIntervalMs = 100;
    static constexpr uint8_t kCalibrationWindows = 24;

    Preferences preferences_;
    int16_t samples_[kSampleCount] = {};
    bool available_ = false;
    bool muted_ = false;
    bool requestActive_ = false;
    bool calibrated_ = false;
    uint8_t sensitivity_ = 1;
    uint8_t calibrationCount_ = 0;
    float calibrationTotal_ = 0;
    float noiseFloor_ = 80;
    float smoothedRms_ = 0;
    uint8_t meter_ = 0;
    SoundLevel level_ = SoundLevel::Unavailable;
    SoundLevel candidate_ = SoundLevel::Quiet;
    uint8_t candidateCount_ = 0;
    uint32_t nextSampleAt_ = 0;

    bool startMicrophone();
    void stopMicrophone();
    void resetCalibration();
    void processWindow();
    SoundLevel classify(float rms) const;
};
