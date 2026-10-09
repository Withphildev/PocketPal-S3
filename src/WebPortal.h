#pragma once

#include <DNSServer.h>
#include <WebServer.h>
#include <WiFi.h>

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

    void configureRoutes();
    void sendState();
    void handleAction();
    void handleName();
    void handleSound();
    void handleMotion();
    static String jsonEscape(const String &value);
};
