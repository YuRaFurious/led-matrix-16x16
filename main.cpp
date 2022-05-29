#include "animations.h"
#include "settings.h"

void setup() {
    Serial.begin(115200);

    // Connect to Wi-Fi
    // WiFiConnect();

    matrix.Begin();
    matrix.ClearTo(HslColor(0.0f, 0.0f, 0.0f));
    matrix.Show();

    animations.StartAnimation(0, 10000, meteorRain);
}

void loop() {
    animations.UpdateAnimations();
    // matrix.SetBrightness(10);
    matrix.Show();
}
