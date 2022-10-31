// ======= CONFIG =======
#define BT_PAUSE 2    // кнопка паузы
#define BT_RIGHT 3    // кнопка +
#define BT_LEFT 4     // кнопка -

#define GES_TOUT 900  // таймаут жестов
#define HOLD_TOUT 500 // таймаут удержания громкости
#define BTN_DELAY 50  // время удержания кнопки для "клика"

// ======= INCLUDE =======
#include <Wire.h>
#include <paj7620.h>

// ======= SETUP =======
void setup() {
  Serial.begin(9600);
  if (paj7620Init()) Serial.print("INIT ERROR!");
}

// ======= TIMER =======
uint32_t skip_tmr = 0;
bool checkTmr(uint32_t t) {
  return (millis() - skip_tmr >= t);
}
void rstTmr() {
  skip_tmr = millis();
}

// ======= BUTTONS =======
void click(byte pin) {
  press(pin);
  delay(BTN_DELAY);
  release(pin);
}
void press(byte pin) {
  pinMode(pin, OUTPUT);
}
void release(byte pin) {
  pinMode(pin, INPUT);
}

// ======= HOLD FLAGS =======
bool Rflag, Lflag;
void checkLeftHold() {
  if (Lflag) {
    release(BT_LEFT);
    Lflag = 0;
    Serial.println("Release BT_LEFT");
  }
}
void checkRightHold() {
  if (Rflag) {
    release(BT_RIGHT);
    Rflag = 0;
    Serial.println("Release BT_RIGHT");
  }
}

// ======= LOOP =======
void loop() {
  uint8_t data, data1;

  if (checkTmr(HOLD_TOUT) && (Rflag || Lflag)) {
    checkRightHold();
    checkLeftHold();
    rstTmr();
  }

  if (!paj7620ReadReg(0x43, 1, &data)) {
    switch (data) {
      case GES_RIGHT_FLAG:
        if (!checkTmr(GES_TOUT)) return;
        rstTmr();
        click(BT_RIGHT);
        Serial.println("Next Track");
        break;

      case GES_LEFT_FLAG:
        if (!checkTmr(GES_TOUT)) return;
        rstTmr();
        click(BT_LEFT);
        Serial.println("Prev Track");
        break;

      case GES_FORWARD_FLAG:
        if (!checkTmr(GES_TOUT)) return;
        rstTmr();
        click(BT_PAUSE);
        Serial.println("Play/Pause");
        break;

      case GES_CLOCKWISE_FLAG:
        rstTmr();
        checkLeftHold();
        if (!Rflag) {
          Rflag = 1;
          press(BT_RIGHT);
          Serial.println("Press BT_RIGHT");
        }
        Serial.println("Vol Up");
        break;

      case GES_COUNT_CLOCKWISE_FLAG:
        rstTmr();
        checkRightHold();
        if (!Lflag) {
          Lflag = 1;
          press(BT_LEFT);
          Serial.println("Press BT_LEFT");
        }
        Serial.println("Vol Down");
        break;

      default:
        paj7620ReadReg(0x44, 1, &data1);
        break;
    }
  }
  delay(100);
}
