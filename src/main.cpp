#include <Arduino.h>
#include <M5Unified.h>

#include "PetEngine.h"
#include "WebPortal.h"

namespace {
constexpr char kVersion[] = "v0.1.1";
constexpr uint8_t kSelectPin = 11;
constexpr uint8_t kNextPin = 12;
constexpr uint8_t kBrightness = 125;
constexpr uint32_t kFrameMs = 280;

PetEngine pet;
WebPortal portal(pet);
size_t selectedAction = 0;
uint32_t frameNumber = 0;
uint32_t lastFrameAt = 0;

enum class Screen : uint8_t { ConnectionInfo, WifiQr, PetHome };
enum class FaceGesture : uint8_t { None, Single, Double };
Screen screen = Screen::ConnectionInfo;

const char *kActionNames[] = {"FEED", "PLAY", "CLEAN", "SLEEP", "PET"};
const PetAction kActions[] = {PetAction::Feed, PetAction::Play, PetAction::Clean, PetAction::Sleep, PetAction::Pet};

bool pressed(uint8_t pin) {
    static uint32_t lastPress[49] = {};
    if (digitalRead(pin) != LOW || millis() - lastPress[pin] < 280) return false;
    lastPress[pin] = millis();
    return true;
}

FaceGesture pollFaceGesture(uint32_t now) {
    static bool rawDown = false;
    static bool stableDown = false;
    static uint32_t rawChangedAt = 0;
    static bool clickPending = false;
    static uint32_t firstClickAt = 0;

    const bool currentRawDown = digitalRead(kSelectPin) == LOW;
    if (currentRawDown != rawDown) {
        rawDown = currentRawDown;
        rawChangedAt = now;
    }
    if (rawDown != stableDown && now - rawChangedAt >= 28) {
        stableDown = rawDown;
        if (stableDown) {
            if (clickPending && now - firstClickAt <= 420) {
                clickPending = false;
                return FaceGesture::Double;
            }
            clickPending = true;
            firstClickAt = now;
        }
    }
    if (clickPending && now - firstClickAt > 420) {
        clickPending = false;
        return FaceGesture::Single;
    }
    return FaceGesture::None;
}

uint16_t moodColor(const String &mood) {
    if (mood == "joyful") return TFT_GREEN;
    if (mood == "sleeping" || mood == "sleepy") return TFT_BLUE;
    if (mood == "hungry") return TFT_ORANGE;
    if (mood == "messy" || mood == "unwell") return TFT_RED;
    if (mood == "lonely") return TFT_MAGENTA;
    return TFT_CYAN;
}

void drawStat(int x, int y, const char *label, uint8_t value, uint16_t color) {
    M5.Display.setTextSize(1);
    M5.Display.setTextColor(TFT_LIGHTGREY, TFT_BLACK);
    M5.Display.setCursor(x, y);
    M5.Display.print(label);
    M5.Display.drawRoundRect(x + 18, y, 65, 7, 3, TFT_DARKGREY);
    const int width = (61 * value) / 100;
    if (width > 0) M5.Display.fillRoundRect(x + 20, y + 2, width, 3, 1, color);
}

void drawPet(const PetSnapshot &s, bool blink) {
    const int bob = (frameNumber % 4 < 2) ? 0 : 2;
    const int cx = 57;
    const int cy = 62 + bob;
    const uint16_t body = s.health < 35 ? TFT_LIGHTGREY : TFT_GREEN;
    M5.Display.fillTriangle(cx - 25, cy - 23, cx - 12, cy - 43, cx - 2, cy - 21, body);
    M5.Display.fillTriangle(cx + 25, cy - 23, cx + 12, cy - 43, cx + 2, cy - 21, body);
    M5.Display.fillEllipse(cx, cy, 37, 34, body);
    M5.Display.fillEllipse(cx - 22, cy + 5, 7, 4, TFT_PINK);
    M5.Display.fillEllipse(cx + 22, cy + 5, 7, 4, TFT_PINK);

    if (s.sleeping || blink) {
        M5.Display.drawLine(cx - 19, cy - 4, cx - 8, cy - 4, TFT_BLACK);
        M5.Display.drawLine(cx + 8, cy - 4, cx + 19, cy - 4, TFT_BLACK);
    } else {
        M5.Display.fillEllipse(cx - 14, cy - 5, 4, 7, TFT_BLACK);
        M5.Display.fillEllipse(cx + 14, cy - 5, 4, 7, TFT_BLACK);
        M5.Display.drawPixel(cx - 13, cy - 7, TFT_WHITE);
        M5.Display.drawPixel(cx + 15, cy - 7, TFT_WHITE);
    }

    if (s.mood == "hungry" || s.mood == "unwell") M5.Display.drawCircle(cx, cy + 14, 5, TFT_BLACK);
    else {
        M5.Display.drawLine(cx - 6, cy + 11, cx, cy + 15, TFT_BLACK);
        M5.Display.drawLine(cx, cy + 15, cx + 6, cy + 11, TFT_BLACK);
    }

    if (s.sleeping) {
        M5.Display.setTextColor(TFT_SKYBLUE, TFT_BLACK);
        M5.Display.setTextSize(2);
        M5.Display.setCursor(88, 31);
        M5.Display.print("z");
    }
}

void drawMainScreen() {
    const PetSnapshot s = pet.snapshot();
    M5.Display.fillScreen(TFT_BLACK);
    M5.Display.fillRect(0, 0, 240, 17, 0x2104);
    M5.Display.setTextColor(TFT_WHITE, 0x2104);
    M5.Display.setTextSize(1);
    M5.Display.setCursor(5, 5);
    M5.Display.print("PocketPal S3 ");
    M5.Display.print(kVersion);
    M5.Display.setCursor(173, 5);
    M5.Display.printf("2x<  W%u", portal.connectedClients());

    drawPet(s, frameNumber % 18 == 0);
    M5.Display.setTextColor(TFT_WHITE, TFT_BLACK);
    M5.Display.setTextSize(2);
    M5.Display.setCursor(111, 23);
    M5.Display.print(s.name.substring(0, 10));
    M5.Display.setTextSize(1);
    M5.Display.setTextColor(moodColor(s.mood), TFT_BLACK);
    M5.Display.setCursor(112, 41);
    M5.Display.print(s.mood);

    drawStat(112, 57, "F", s.fullness, TFT_ORANGE);
    drawStat(112, 69, "H", s.happiness, TFT_MAGENTA);
    drawStat(112, 81, "E", s.energy, TFT_BLUE);
    drawStat(112, 93, "C", s.cleanliness, TFT_CYAN);

    M5.Display.fillRoundRect(6, 110, 228, 20, 7, 0x3186);
    M5.Display.setTextColor(TFT_LIGHTGREY, 0x3186);
    M5.Display.setTextSize(1);
    M5.Display.setCursor(12, 117);
    M5.Display.print("M5 next");
    M5.Display.setTextColor(TFT_WHITE, 0x3186);
    M5.Display.setCursor(87, 117);
    M5.Display.printf("< %s >", kActionNames[selectedAction]);
    M5.Display.setTextColor(TFT_LIGHTGREY, 0x3186);
    M5.Display.setCursor(180, 117);
    M5.Display.print("Face do");
}

void drawConnectionInfo() {
    M5.Display.fillScreen(0x20A4);
    M5.Display.setTextColor(TFT_WHITE, 0x20A4);
    M5.Display.setTextSize(2);
    M5.Display.setCursor(10, 9);
    M5.Display.println("PocketPal S3");
    M5.Display.setTextSize(1);
    M5.Display.setTextColor(TFT_CYAN, 0x20A4);
    M5.Display.setCursor(10, 38);
    M5.Display.println("1 / 3  CONNECT WEBUI");
    M5.Display.setTextColor(TFT_WHITE, 0x20A4);
    M5.Display.setTextSize(2);
    M5.Display.setCursor(10, 54);
    M5.Display.println(portal.ssid());
    M5.Display.setTextSize(1);
    M5.Display.setCursor(10, 82);
    M5.Display.print("Password: ");
    M5.Display.println(portal.password());
    M5.Display.setCursor(10, 99);
    M5.Display.println("Open http://192.168.4.1");
    M5.Display.setTextColor(TFT_LIGHTGREY, 0x20A4);
    M5.Display.setCursor(10, 117);
    M5.Display.println("Face: show QR");
}

void drawWifiQr() {
    auto &d = M5.Display;
    d.fillScreen(TFT_BLACK);
    d.setTextSize(2);
    d.setTextColor(TFT_CYAN, TFT_BLACK);
    d.setCursor(6, 4);
    d.println("2 / 3  Wi-Fi QR");
    d.setTextSize(1);
    d.setTextColor(TFT_WHITE, TFT_BLACK);
    d.setCursor(6, 28);
    d.println("Scan to join");
    d.setTextColor(TFT_YELLOW, TFT_BLACK);
    d.setCursor(6, 43);
    d.println(portal.ssid());
    d.setCursor(6, 57);
    d.println(portal.password());

    const String payload = "WIFI:T:WPA;S:" + portal.ssid() + ";P:" + portal.password() + ";H:false;;";
    d.qrcode(payload.c_str(), 115, 5, 120, 4, false);

    d.setTextColor(TFT_LIGHTGREY, TFT_BLACK);
    d.setCursor(6, 105);
    d.println("Face: start pet");
    d.setCursor(6, 120);
    d.println("2x Face: back");
}

void drawActiveScreen() {
    if (screen == Screen::ConnectionInfo) drawConnectionInfo();
    else if (screen == Screen::WifiQr) drawWifiQr();
    else drawMainScreen();
}

void goBack() {
    if (screen == Screen::PetHome) screen = Screen::WifiQr;
    else if (screen == Screen::WifiQr) screen = Screen::ConnectionInfo;
    drawActiveScreen();
}

void goForwardOrAct() {
    if (screen == Screen::ConnectionInfo) screen = Screen::WifiQr;
    else if (screen == Screen::WifiQr) screen = Screen::PetHome;
    else pet.apply(kActions[selectedAction]);
    drawActiveScreen();
}
} // namespace

void setup() {
    Serial.begin(115200);
    auto config = M5.config();
    M5.begin(config);
    M5.Display.setRotation(3);
    M5.Display.setBrightness(kBrightness);
    M5.Power.setExtOutput(false);
    pinMode(kSelectPin, INPUT_PULLUP);
    pinMode(kNextPin, INPUT_PULLUP);
    pet.begin();
    portal.begin();
    drawConnectionInfo();
}

void loop() {
    const uint32_t now = millis();
    portal.loop();
    pet.tick(now);
    bool redraw = false;
    if (screen == Screen::PetHome && pressed(kNextPin)) {
        selectedAction = (selectedAction + 1) % (sizeof(kActions) / sizeof(kActions[0]));
        redraw = true;
    }

    const FaceGesture gesture = pollFaceGesture(now);
    if (gesture == FaceGesture::Double) goBack();
    else if (gesture == FaceGesture::Single) goForwardOrAct();

    if (screen == Screen::PetHome && (redraw || now - lastFrameAt >= kFrameMs)) {
        ++frameNumber;
        drawMainScreen();
        lastFrameAt = now;
    }
    delay(2);
}
