#pragma once

#include <DNSServer.h>
#include <WebServer.h>
#include <WiFi.h>

#include "PetEngine.h"

class WebPortal {
  public:
    explicit WebPortal(PetEngine &pet);
    void begin();
    void loop();
    const String &ssid() const;
    const String &password() const;
    uint8_t connectedClients() const;

  private:
    PetEngine &pet_;
    WebServer server_{80};
    DNSServer dns_;
    String ssid_;
    String password_;

    void configureRoutes();
    void sendState();
    void handleAction();
    void handleName();
    static String jsonEscape(const String &value);
};

