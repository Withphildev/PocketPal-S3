#include "SoundSensor.h"

#include <algorithm>
#include <cmath>

namespace {
constexpr char kSoundNamespace[] = "palsound";
}

void SoundSensor::begin() {
    preferences_.begin(kSoundNamespace, false);
    muted_ = preferences_.getBool("muted", false);
    sensitivity_ = std::max<uint8_t>(1, std::min<uint8_t>(3, preferences_.getUChar("sensitivity", 2)));
    if (muted_) {
        level_ = SoundLevel::Muted;
        return;
    }
    startMicrophone();
}

bool SoundSensor::startMicrophone() {
    M5.Speaker.end();
    available_ = M5.Mic.isEnabled() && M5.Mic.begin();
    requestActive_ = false;
    resetCalibration();
    level_ = available_ ? SoundLevel::Calibrating : SoundLevel::Unavailable;
    nextSampleAt_ = millis();
    return available_;
}

void SoundSensor::stopMicrophone() {
    M5.Mic.end();
    requestActive_ = false;
}

void SoundSensor::resetCalibration() {
    calibrated_ = false;
    calibrationCount_ = 0;
    calibrationTotal_ = 0;
    noiseFloor_ = 80;
    smoothedRms_ = 0;
    meter_ = 0;
    candidate_ = SoundLevel::Quiet;
    candidateCount_ = 0;
}

void SoundSensor::tick(uint32_t now) {
    if (muted_ || !available_) return;

    if (requestActive_) {
        if (M5.Mic.isRecording() != 0) return;
        requestActive_ = false;
        processWindow();
        nextSampleAt_ = now + kSampleIntervalMs;
        return;
    }

    if (static_cast<int32_t>(now - nextSampleAt_) < 0) return;
    if (M5.Mic.record(samples_, kSampleCount, kSampleRate, false)) requestActive_ = true;
    else nextSampleAt_ = now + 250;
}

void SoundSensor::processWindow() {
    int64_t sum = 0;
    for (const int16_t sample : samples_) sum += sample;
    const int32_t mean = static_cast<int32_t>(sum / static_cast<int64_t>(kSampleCount));

    uint64_t squareTotal = 0;
    for (const int16_t sample : samples_) {
        const int32_t centered = static_cast<int32_t>(sample) - mean;
        squareTotal += static_cast<uint64_t>(static_cast<int64_t>(centered) * centered);
    }
    const float rms = std::sqrt(static_cast<float>(squareTotal) / kSampleCount);
    smoothedRms_ = smoothedRms_ == 0 ? rms : (smoothedRms_ * 0.68f + rms * 0.32f);

    if (!calibrated_) {
        calibrationTotal_ += rms;
        ++calibrationCount_;
        level_ = SoundLevel::Calibrating;
        if (calibrationCount_ >= kCalibrationWindows) {
            noiseFloor_ = std::max(40.0f, calibrationTotal_ / calibrationCount_);
            calibrated_ = true;
            level_ = SoundLevel::Quiet;
        }
        return;
    }

    if (smoothedRms_ < noiseFloor_ * 1.35f) {
        noiseFloor_ = noiseFloor_ * 0.985f + smoothedRms_ * 0.015f;
    }

    const float factor = sensitivity_ == 3 ? 0.72f : (sensitivity_ == 1 ? 1.35f : 1.0f);
    const float highThreshold = std::max(75.0f, noiseFloor_) * 5.2f * factor;
    meter_ = static_cast<uint8_t>(std::min(100.0f, smoothedRms_ * 100.0f / highThreshold));

    const SoundLevel next = classify(smoothedRms_);
    if (next == level_) {
        candidate_ = next;
        candidateCount_ = 0;
        return;
    }
    if (next != candidate_) {
        candidate_ = next;
        candidateCount_ = 1;
        return;
    }
    ++candidateCount_;
    const bool rising = static_cast<uint8_t>(next) > static_cast<uint8_t>(level_);
    if (candidateCount_ >= (rising ? 2 : 4)) {
        level_ = next;
        candidateCount_ = 0;
    }
}

SoundLevel SoundSensor::classify(float rms) const {
    const float factor = sensitivity_ == 3 ? 0.72f : (sensitivity_ == 1 ? 1.35f : 1.0f);
    const float floor = std::max(75.0f, noiseFloor_);
    if (rms >= floor * 5.2f * factor) return SoundLevel::High;
    if (rms >= floor * 2.8f * factor) return SoundLevel::Medium;
    if (rms >= floor * 1.6f * factor) return SoundLevel::Low;
    return SoundLevel::Quiet;
}

void SoundSensor::setMuted(bool muted) {
    if (muted_ == muted) return;
    muted_ = muted;
    preferences_.putBool("muted", muted_);
    if (muted_) {
        stopMicrophone();
        level_ = SoundLevel::Muted;
        meter_ = 0;
    } else {
        startMicrophone();
    }
}

void SoundSensor::toggleMuted() { setMuted(!muted_); }

bool SoundSensor::setSensitivity(uint8_t sensitivity) {
    if (sensitivity < 1 || sensitivity > 3) return false;
    sensitivity_ = sensitivity;
    preferences_.putUChar("sensitivity", sensitivity_);
    return true;
}

SoundSnapshot SoundSensor::snapshot() const {
    return {level_, levelName(level_), meter_, sensitivity_, static_cast<uint16_t>(smoothedRms_),
            static_cast<uint16_t>(noiseFloor_), available_, muted_, calibrated_};
}

SoundLevel SoundSensor::level() const { return level_; }

const char *SoundSensor::levelName(SoundLevel level) {
    switch (level) {
        case SoundLevel::Unavailable: return "unavailable";
        case SoundLevel::Muted: return "muted";
        case SoundLevel::Calibrating: return "calibrating";
        case SoundLevel::Quiet: return "quiet";
        case SoundLevel::Low: return "low";
        case SoundLevel::Medium: return "medium";
        case SoundLevel::High: return "high";
    }
    return "unknown";
}

const char *SoundSensor::shortLabel(SoundLevel level) {
    switch (level) {
        case SoundLevel::Unavailable: return "MIC?";
        case SoundLevel::Muted: return "MUTE";
        case SoundLevel::Calibrating: return "CAL";
        case SoundLevel::Quiet: return "QUIET";
        case SoundLevel::Low: return "LOW";
        case SoundLevel::Medium: return "MED";
        case SoundLevel::High: return "HIGH";
    }
    return "?";
}
