#pragma once

#include <DNSServer.h>
#include <WebServer.h>
#include <WiFi.h>
#include <Preferences.h>

#include "PetEngine.h"
#include "MotionSensor.h"
#include "SoundSensor.h"

class WebPortal {
  public:
    WebPortal(PetEngine &pet, SoundSensor &sound, MotionSensor &motion);
    void begin();
    void loop();
    const String &ssid() const;
    const String &password() const;
    uint8_t connectedClients() const;

  private:
    PetEngine &pet_;
    SoundSensor &sound_;
    MotionSensor &motion_;
    WebServer server_{80};
    DNSServer dns_;
    String ssid_;
    String password_;
    Preferences preferences_;
    String theme_ = "teal";
    uint32_t ballBest_ = 0;

    void configureRoutes();
    void sendState();
    void handleAction();
    void handleName();
    void handleSound();
    void handleMotion();
    void handleTheme();
    void handleBallBest();
    static bool validTheme(const String &theme);
    static String jsonEscape(const String &value);
};
