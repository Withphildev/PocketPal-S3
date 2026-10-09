#include "MotionSensor.h"

#include <algorithm>
#include <cmath>

namespace {
constexpr char kMotionNamespace[] = "palmotion";
constexpr uint32_t kShakeCooldownMs = 8000;
constexpr uint32_t kRockCooldownMs = 15000;
}

void MotionSensor::begin() {
    preferences_.begin(kMotionNamespace, false);
    sensitivity_ = std::max<uint8_t>(1, std::min<uint8_t>(3, preferences_.getUChar("sensitivity", 2)));
    available_ = M5.Imu.isEnabled();
    state_ = available_ ? MotionState::Calibrating : MotionState::Unavailable;
    nextSampleAt_ = millis();
}

void MotionSensor::tick(uint32_t now) {
    if (!available_ || static_cast<int32_t>(now - nextSampleAt_) < 0) return;
    nextSampleAt_ = now + kSampleIntervalMs;

    float x = 0;
    float y = 0;
    float z = 0;
    if (!M5.Imu.getAccel(&x, &y, &z)) {
        available_ = false;
        state_ = MotionState::Unavailable;
        return;
    }

    if (!calibrated_) {
        if (calibrationCount_ == 0) {
            gravityX_ = previousX_ = x;
            gravityY_ = previousY_ = y;
            gravityZ_ = previousZ_ = z;
        } else {
            gravityX_ = gravityX_ * 0.9f + x * 0.1f;
            gravityY_ = gravityY_ * 0.9f + y * 0.1f;
            gravityZ_ = gravityZ_ * 0.9f + z * 0.1f;
            previousX_ = x;
            previousY_ = y;
            previousZ_ = z;
        }
        if (++calibrationCount_ >= kCalibrationSamples) {
            calibrated_ = true;
            state_ = MotionState::Still;
        }
        return;
    }

    const float jerkX = x - previousX_;
    const float jerkY = y - previousY_;
    const float jerkZ = z - previousZ_;
    const float jerk = std::sqrt(jerkX * jerkX + jerkY * jerkY + jerkZ * jerkZ);
    const float magnitude = std::sqrt(x * x + y * y + z * z);
    const float dynamic = jerk + std::fabs(magnitude - 1.0f) * 0.65f;
    previousX_ = x;
    previousY_ = y;
    previousZ_ = z;

    gravityX_ = gravityX_ * 0.9f + x * 0.1f;
    gravityY_ = gravityY_ * 0.9f + y * 0.1f;
    gravityZ_ = gravityZ_ * 0.9f + z * 0.1f;

    const float sensitivityFactor = sensitivity_ == 3 ? 0.72f : (sensitivity_ == 1 ? 1.30f : 1.0f);
    const float shakeThreshold = 0.82f * sensitivityFactor;
    meter_ = static_cast<uint8_t>(std::min(100.0f, dynamic * 100.0f / (shakeThreshold * 1.5f)));

    if (dynamic >= shakeThreshold) {
        shakeHits_ = std::min<uint8_t>(shakeHits_ + 1, 5);
    } else if (shakeHits_ > 0) {
        --shakeHits_;
    }
    if (shakeHits_ >= 3 && (lastShakeAt_ == 0 || now - lastShakeAt_ >= kShakeCooldownMs)) {
        shakeHits_ = 0;
        lastShakeAt_ = now;
        eventUntil_ = now + 1300;
        state_ = MotionState::Shake;
        shakePending_ = true;
        rockAlternations_ = 0;
        lastRockSign_ = 0;
        return;
    }

    updateRocking(now, dynamic, shakeThreshold);
    if ((state_ == MotionState::Shake || state_ == MotionState::Rocking) &&
        static_cast<int32_t>(eventUntil_ - now) > 0) return;
    classifyTilt();
}

void MotionSensor::updateRocking(uint32_t now, float dynamic, float shakeThreshold) {
    // The supported landscape orientation has the Face button on the left.
    // In that orientation screen X is the IMU's Y axis.
    const float axis = gravityY_;
    const float amount = std::fabs(axis);
    if (dynamic >= shakeThreshold * 0.82f || amount < 0.17f || amount > 0.78f) return;

    const int8_t sign = axis > 0 ? 1 : -1;
    if (lastRockSign_ == 0) {
        lastRockSign_ = sign;
        lastRockCrossingAt_ = now;
        return;
    }
    if (sign == lastRockSign_) return;

    const uint32_t interval = now - lastRockCrossingAt_;
    lastRockSign_ = sign;
    lastRockCrossingAt_ = now;
    if (interval >= 180 && interval <= 1250) ++rockAlternations_;
    else rockAlternations_ = 1;

    if (rockAlternations_ >= 5 && (lastRockAt_ == 0 || now - lastRockAt_ >= kRockCooldownMs)) {
        rockAlternations_ = 0;
        lastRockAt_ = now;
        eventUntil_ = now + 2200;
        state_ = MotionState::Rocking;
        rockingPending_ = true;
    }
}

void MotionSensor::classifyTilt() {
    const float threshold = sensitivity_ == 3 ? 0.24f : (sensitivity_ == 1 ? 0.42f : 0.33f);
    // Rotate the normalized board axes 90 degrees left to match the landscape
    // screen: Face button on the left, display text upright.
    const float screenX = gravityY_;
    const float screenY = -gravityX_;
    if (std::fabs(screenX) >= std::fabs(screenY) && std::fabs(screenX) >= threshold) {
        state_ = screenX < 0 ? MotionState::TiltRight : MotionState::TiltLeft;
    } else if (std::fabs(screenY) >= threshold) {
        state_ = screenY < 0 ? MotionState::TiltForward : MotionState::TiltBack;
    } else {
        state_ = MotionState::Still;
    }
}

bool MotionSensor::setSensitivity(uint8_t sensitivity) {
    if (sensitivity < 1 || sensitivity > 3) return false;
    sensitivity_ = sensitivity;
    preferences_.putUChar("sensitivity", sensitivity_);
    return true;
}

MotionSnapshot MotionSensor::snapshot() const {
    return {state_, stateName(state_), meter_, sensitivity_, available_, calibrated_};
}

MotionState MotionSensor::state() const { return state_; }

bool MotionSensor::consumeShake() {
    const bool pending = shakePending_;
    shakePending_ = false;
    return pending;
}

bool MotionSensor::consumeRocking() {
    const bool pending = rockingPending_;
    rockingPending_ = false;
    return pending;
}

const char *MotionSensor::stateName(MotionState state) {
    switch (state) {
        case MotionState::Unavailable: return "unavailable";
        case MotionState::Calibrating: return "calibrating";
        case MotionState::Still: return "still";
        case MotionState::TiltLeft: return "tilt-left";
        case MotionState::TiltRight: return "tilt-right";
        case MotionState::TiltForward: return "tilt-forward";
        case MotionState::TiltBack: return "tilt-back";
        case MotionState::Shake: return "shake";
        case MotionState::Rocking: return "rocking";
    }
    return "unknown";
}

const char *MotionSensor::shortLabel(MotionState state) {
    switch (state) {
        case MotionState::Unavailable: return "IMU?";
        case MotionState::Calibrating: return "CAL";
        case MotionState::Still: return "STILL";
        case MotionState::TiltLeft: return "LEFT";
        case MotionState::TiltRight: return "RIGHT";
        case MotionState::TiltForward: return "FWD";
        case MotionState::TiltBack: return "BACK";
        case MotionState::Shake: return "SHAKE";
        case MotionState::Rocking: return "ROCK";
    }
    return "?";
}
