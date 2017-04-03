#include <LiquidCrystal.h>

#define LED 6
#define SENSOR A0

#define BIT6 32
#define BIT5 16
#define BIT4 8
#define BIT3 4
#define BIT2 2
#define BIT1 1

long longDuration = 1000;
long shortDuration = 500;
long offDuration = 200;
long betweenNumbersDuration = 1500;

long calculateNextChange(unsigned char c, unsigned bit);

void receiver(long currentTime);
void sender(long currentTime);


char charactersBits[] = {
  111, // 1
  103, // 2
  99,  // 3
  97   // 4
};

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

void setup() {
  randomSeed(analogRead(0));
  pinMode(LED, OUTPUT);
  pinMode(SENSOR, INPUT);
  lcd.begin(16,2);
  lcd.setCursor(0, 0);
  lcd.clear();
}

void loop() {
  long currentTime = millis();
  receiver(currentTime);
  sender(currentTime);
}

void receiver(long currentTime) {
  static unsigned char state = 1;
  static long lastChange = 0;
  static long lastCheck = 0;
  static unsigned char index = 1;
  static unsigned char number = 200;
  static bool lastState = false;
  static unsigned char current = 96;
  
  switch(state) {
    case 1: // Start
      index = 1;
      current = 96;
      state = 2;
      break;
    case 2: // Read
      if (currentTime - lastCheck > 5) {
          bool currentState = analogRead(SENSOR) < 100;
          if (currentState != lastState) {
            if (currentState) {
              if (currentTime - lastChange > betweenNumbersDuration-50) {
                state = 1;
                for(int i = 0; i < 4; i++) {
                  if (charactersBits[i] == current) {
                    lcd.clear();
                    lcd.print(i+1);
                    break;
                  }
                }
              }
                
            } else {
                switch (index) {
                case 1:
                  if (currentTime - lastChange > longDuration) current |= BIT5;
                  break;
                case 2:
                  if (currentTime - lastChange > longDuration) current |= BIT4;
                  break;
                case 3:
                  if (currentTime - lastChange > longDuration) current |= BIT3;
                  break;
                case 4:
                  if (currentTime - lastChange > longDuration) current |= BIT2;
                  break;
                case 5:
                  if (currentTime - lastChange > longDuration) current |= BIT1;
                  break;
                }
                index++;               
            }
            lastChange = currentTime;
            lastState = currentState;
          }
          lastCheck = currentTime;
      }
      break;
  }
}


void sender(long currentTime) {
  static unsigned char currentNumber = 1;
  static unsigned char numberPartIndex = 1;
  static unsigned char state = 1;
  static long lastChange = 0;
  static long duration = 0;
  static unsigned char length = 0;
  
  switch(state) {
    case 1: // Start state
      currentNumber = charactersBits[random(4)];
      if (currentNumber & BIT6) length = 5;
      else if (currentNumber & BIT5) length = 4;
      else if (currentNumber & BIT4) length = 3;
      else if (currentNumber & BIT3) length = 2;
      else if (currentNumber & BIT2) length = 1;
      numberPartIndex = 1;
      state = 2;
      break;
    case 2: // Led is off
      if (currentTime - lastChange > duration) {
        digitalWrite(LED, HIGH);
        lastChange = currentTime;
        duration = nextChange(currentNumber, numberPartIndex);
        state = 3;
      }
      break;
    case 3: // Led is on
      if (currentTime - lastChange > duration) {
        digitalWrite(LED, LOW);
        lastChange = currentTime;
        if (numberPartIndex >= length) {
          duration = betweenNumbersDuration;
          state = 4;
        } else {
          duration = offDuration;
          numberPartIndex++;
          state = 2;
        }
      }          
      break;
    case 4: // Between letters
      if (currentTime - lastChange > duration) {
        state = 1;       
      }
  }
  
}

long nextChange(unsigned char c, unsigned char index) {
  switch(index) {
    case 1:
      if (c & BIT5) return longDuration;
      else return shortDuration;
      break;
    case 2:
      if (c & BIT4) return longDuration;
      else return  shortDuration;
      break;
    case 3:
      if (c & BIT3) return longDuration;
      else return  shortDuration;
      break;
    case 4:
      if (c & BIT2) return longDuration;
      else return  shortDuration;
      break;
    case 5:
      if (c & BIT1) return longDuration;
      else return  shortDuration;
      break;
  }
}

