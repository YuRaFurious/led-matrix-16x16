#ifndef ANIMATION_H
#define ANIMATION_H

#include <Arduino.h>
#include <NeoPixelAnimator.h>
#include <NeoPixelBrightnessBus.h>

#include "images.h"

#define PIN_LEDS 21
#define WIDTH 16
#define HEIGHT 16
#define NUMPIXELS (WIDTH * HEIGHT)

NeoPixelBrightnessBus<NeoGrbFeature, Neo800KbpsMethod> matrix(NUMPIXELS, PIN_LEDS);
NeoTopology<RowMajorAlternatingLayout> topo(WIDTH, HEIGHT);
NeoGamma<NeoGammaEquationMethod> colorGamma;

NeoPixelAnimator animations(1);

void rowsRainbow(const AnimationParam &param) {
    float hue;
    for (uint8_t x = 0; x < WIDTH; x++) {
        for (uint8_t y = 0; y < HEIGHT; y++) {
            hue = param.progress + (float)x / (float)WIDTH;
            if (hue > 1.0f) {
                hue -= 1.0f;
            }
            matrix.SetPixelColor(topo.Map(x, y), colorGamma.Correct(RgbColor(HslColor(hue, 1.0f, 0.5f))));
        }
    }
    if (param.state == AnimationState_Completed) {
        animations.RestartAnimation(0);
    }
}

void cubeRainbow(const AnimationParam &param) {
    float hue;
    for (uint8_t k = 0; k < WIDTH / 2; k++) {
        for (uint8_t i = 0 + k; i < WIDTH - k; i++) {
            for (uint8_t y = 0 + k; y < HEIGHT - k; y++) {
                hue = param.progress + (float)k / (float)(WIDTH / 2);  // +/-
                if (hue > 1.0f) {
                    hue -= 1.0f;
                } else if (hue < 0) {
                    hue += 1.0f;
                }
                matrix.SetPixelColor(topo.Map(i, y), colorGamma.Correct(RgbColor(HslColor(hue, 1.0f, 0.5f))));
            }
        }
    }
    if (param.state == AnimationState_Completed) {
        animations.RestartAnimation(0);
    }
}

void rainbowAnimation(const AnimationParam &param) {
    matrix.ClearTo(colorGamma.Correct(RgbColor(HslColor(param.progress, 1.0f, 0.5f))));
    if (param.state == AnimationState_Completed) {
        animations.RestartAnimation(0);
    }
}

void pixelAnimation(const AnimationParam &param) {
    matrix.ClearTo(HslColor(0.0f, 0.0f, 0.0f));
    matrix.SetPixelColor(ceil(NUMPIXELS * param.progress) - 1, colorGamma.Correct(RgbColor(HslColor(0.0f, 0.0f, 0.5f))));
    if (param.state == AnimationState_Completed) {
        animations.RestartAnimation(0);
    }
}

void fireAnimation(const AnimationParam &param) {
    const uint8_t SpeedDelay = 40, Cooling = 55, Sparking = 120;
    delay(SpeedDelay);
    static uint8_t heat[WIDTH][HEIGHT];
    uint16_t cooldown;
    for (uint8_t i = 0; i < WIDTH; i++) {
        for (uint8_t y = 0; y < HEIGHT; y++) {
            cooldown = random(0, ((Cooling * 10) / HEIGHT) + 2);
            if (cooldown > heat[i][y]) {
                heat[i][y] = 0;
            } else {
                heat[i][y] = heat[i][y] - cooldown;
            }
        }
        for (uint8_t y = HEIGHT - 1; y >= 2; y--) {
            heat[i][y] = (heat[i][y - 1] + heat[i][y - 2] + heat[i][y - 2]) / 3;
        }
        if (random(255) < Sparking) {
            uint8_t y = random(7);
            heat[i][y] = heat[i][y] + random(160, 255);
        }
        for (uint8_t y = 0; y < HEIGHT; y++) {
            uint8_t t192 = round((heat[i][y] / 255.0) * 191);
            uint8_t heatramp = t192 & 63;
            heatramp <<= 2;
            if (t192 > 128) {
                matrix.SetPixelColor(topo.Map(i, y), colorGamma.Correct(RgbColor(255, 255, heatramp)));
            } else if (t192 > 64) {
                matrix.SetPixelColor(topo.Map(i, y), colorGamma.Correct(RgbColor(255, heatramp, 0)));
            } else {
                matrix.SetPixelColor(topo.Map(i, y), colorGamma.Correct(RgbColor(heatramp, 0, 0)));
            }
        }
    }
    if (param.state == AnimationState_Completed) {
        animations.RestartAnimation(0);
    }
}

void meteorRain(const AnimationParam &param) {
    const uint8_t size = 2, speed = 30, decayAmount = 128;
    static uint32_t past[WIDTH]{0};
    static uint32_t restart[WIDTH]{0};
    static uint8_t pos[WIDTH]{0};
    uint32_t now = millis();
    for (uint8_t x = 0; x < WIDTH; x++) {
        if (now - past[x] >= speed) {
            if (pos[x] >= size) {
                for (uint8_t i = 0; i <= pos[x] - size; i++) {
                    if (random(10) > 5) {
                        RgbColor oldColor;
                        uint8_t r, g, b;

                        oldColor = matrix.GetPixelColor(topo.Map(x, i));
                        r = oldColor.R;
                        g = oldColor.G;
                        b = oldColor.B;

                        r = (r <= 10) ? 0 : (int)r - (r * decayAmount / 256);
                        g = (g <= 10) ? 0 : (int)g - (g * decayAmount / 256);
                        b = (b <= 10) ? 0 : (int)b - (b * decayAmount / 256);

                        matrix.SetPixelColor(topo.Map(x, i), RgbColor(r, g, b));
                    }
                }
            }

            if (pos[x] < HEIGHT * 2) {
                if (pos[x] < HEIGHT) {
                    matrix.SetPixelColor(topo.Map(x, pos[x]), RgbColor(255, 255, 255));

                    for (uint8_t i = pos[x] + 1; i < HEIGHT; i++) {
                        if (random(10) > 5) {
                            RgbColor oldColor;
                            uint8_t r, g, b;

                            oldColor = matrix.GetPixelColor(topo.Map(x, i));
                            r = oldColor.R;
                            g = oldColor.G;
                            b = oldColor.B;

                            r = (r <= 10) ? 0 : (int)r - (r * decayAmount / 256);
                            g = (g <= 10) ? 0 : (int)g - (g * decayAmount / 256);
                            b = (b <= 10) ? 0 : (int)b - (b * decayAmount / 256);

                            matrix.SetPixelColor(topo.Map(x, i), RgbColor(r, g, b));
                        }
                    }
                }
                past[x] = now;
                pos[x]++;
            } else {
                if (restart[x] == 0) {
                    past[x] = now;
                    restart[x] = random(500, 3000);
                } else if (now - past[x] >= restart[x]) {
                    past[x] = 0;
                    pos[x] = 0;
                    restart[x] = 0;
                }
            }
        }
    }
    if (param.state == AnimationState_Completed) {
        animations.RestartAnimation(0);
    }
}

void drawImage(const AnimationParam &param) {
    for (uint8_t x = 0; x < WIDTH; x++) {
        for (uint8_t y = 0; y < HEIGHT; y++) {
            matrix.SetPixelColor(topo.Map(x, y), HtmlColor(heart[x * WIDTH + y]));
        }
    }
    if (param.state == AnimationState_Completed) {
        animations.RestartAnimation(0);
    }
}

#endif