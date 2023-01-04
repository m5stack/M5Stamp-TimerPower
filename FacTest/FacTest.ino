#include "M5Atom.h"
#include "M5_I2C_BM8563.h"

#define POWER_HOLD_PIN 19
#define EN_3V3         23
uint8_t test_status = 0;
uint32_t last_color = 0;
uint32_t color      = 0;

void bat_init() {
    pinMode(POWER_HOLD_PIN, OUTPUT);
    digitalWrite(POWER_HOLD_PIN, 1);
    pinMode(EN_3V3, OUTPUT);
    digitalWrite(EN_3V3, 1);
}

void bat_disable_output() {
    //    esp_sleep_enable_timer_wakeup(600000000);
    digitalWrite(POWER_HOLD_PIN, 0);
    //    esp_deep_sleep_start();
}

void setup() {
    M5.begin(true, true, true);
    // will hold battery output
    bat_init();
    M5.dis.drawpix(0, 0x00ff00);  // YELLOW stanby
    delay(10);
    color = 0x00ff00;
}

void loop() {
    I2C_BM8563 rtc;
    if (rtc.isEnable()) {
        M5.dis.drawpix(0, color);
        M5.update();
        if (M5.Btn.pressedFor(1000)) {
            while (M5.Btn.read()) {
                color = 0xff0000;
                M5.dis.drawpix(0, color);
            }
            if (rtc.isEnable()) {
                color = 0x00ff00;
                rtc.disableIRQ();
                rtc.clearIRQ();
                bat_disable_output();
            }
            digitalWrite(POWER_HOLD_PIN, 0);
        } else if (M5.Btn.wasReleasefor(100)) {
            switch (test_status) {
                case 0:
                    digitalWrite(EN_3V3, 0);
                    color = 0x0000ff;
                    test_status++;
                    break;
                case 1:
                    digitalWrite(EN_3V3, 1);
                    color = 0xffffff;
                    test_status++;
                    break;
                case 2:
                    if (rtc.isEnable()) {
                        color = 0x00ff00;
                        rtc.WakeAfterSeconds(2);
                        bat_disable_output();
                    } else {
                        color = 0x800080;
                        delay(5000);
                    }
                    test_status++;
                    break;

                default:
                    break;
            }
        }
    } else {
        M5.dis.drawpix(0, 0x800080);
    }
}
