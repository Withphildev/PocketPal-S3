#pragma once

#include "MotionSensor.h"
#include "PetEngine.h"
#include "SoundSensor.h"

// This ordering matches the generated PetAssets image tables.
enum class PetPose : uint8_t { Idle, Happy, Listening, Excited, Sleeping, Surprised };

inline PetPose selectPetPose(const PetSnapshot &pet, SoundLevel sound, MotionState motion) {
    if (pet.sleeping) return PetPose::Sleeping;
    if (sound == SoundLevel::High) return PetPose::Surprised;
    if (motion == MotionState::Shake || sound == SoundLevel::Medium || pet.celebrating) return PetPose::Excited;
    if (sound == SoundLevel::Low) return PetPose::Listening;
    if (pet.mood == "joyful") return PetPose::Happy;
    return PetPose::Idle;
}

inline const char *petPoseName(PetPose pose) {
    switch (pose) {
        case PetPose::Idle: return "idle";
        case PetPose::Happy: return "happy";
        case PetPose::Listening: return "listening";
        case PetPose::Excited: return "excited";
        case PetPose::Sleeping: return "sleeping";
        case PetPose::Surprised: return "surprised";
    }
    return "idle";
}
