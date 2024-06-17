#include <Servo.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
#include <Password.h>

#define BUZZER_PIN 12

Servo servo;
LiquidCrystal_I2C lcd(0x27, 16, 2);

Password password("659825");

const byte MAX_PASS_LENGTH = 6;
byte currentPasswordLength = 0;

const byte ROWS = 4;
const byte COLS = 4;

char keys[ROWS][COLS] = {
  {'D', 'C', 'B', 'A'},
  {'#', '9', '6', '3'},
  {'0', '8', '5', '2'},
  {'*', '7', '4', '1'},
};

byte rowPins[ROWS] = {2, 3, 4, 5};
byte colPins[COLS] = {6, 7, 8, 9};

Keypad keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

void setup() {
  Serial.begin(9600);
  pinMode(BUZZER_PIN, OUTPUT);
  servo.attach(11); // Attach the servo to pin 13
  servo.write(90);  // Set initial position to 90 degrees (middle position)
  lcd.init();
  lcd.backlight();
  lcd.setCursor(3, 0);
  lcd.print("WELCOME TO");
  lcd.setCursor(0, 1);
  lcd.print("DOOR LOCK SYSTEM");
  delay(3000);
  lcd.clear();
}

void loop() {
  lcd.setCursor(1, 0);
  lcd.print("ENTER PASSWORD");

  char key = keypad.getKey();
  if (key != NO_KEY) {
    delay(60);
    if (key == 'C') {
      resetPassword();
    } else if (key == 'D') {
      if (password.evaluate()) {
        if (servo.read() == 90) { // Check if servo is in the middle position
          doorOpen();
        } else {
          doorLocked();
        }
      } else {
        invalidPassword();
      }
    } else {
      processNumberKey(key);
    }
  }
}

void processNumberKey(char key) {
  if (currentPasswordLength < MAX_PASS_LENGTH) {
    lcd.setCursor(currentPasswordLength + 5, 1);
    lcd.print("*");
    currentPasswordLength++;
    password.append(key);
  }
}

void doorOpen() {
  digitalWrite(BUZZER_PIN, HIGH);
  delay(300);
  digitalWrite(BUZZER_PIN, LOW);
  servo.write(10); // Open position (adjust as necessary)
  delay(100);
  lcd.clear();
  lcd.print("CORRECT PASSWORD");
  lcd.setCursor(0, 1);
  lcd.print("DOOR UNLOCKED");
  delay(2000);
  lcd.clear();
  resetPassword();
}

void doorLocked() {
  digitalWrite(BUZZER_PIN, HIGH);
  delay(300);
  digitalWrite(BUZZER_PIN, LOW);
  servo.write(170); // Locked position (adjust as necessary)
  delay(100);
  lcd.clear();
  lcd.print("CORRECT PASSWORD");
  lcd.setCursor(2, 1);
  lcd.print("DOOR LOCKED");
  delay(2000);
  lcd.clear();
  resetPassword();
}

void invalidPassword() {
  for (int i = 0; i < 3; i++) {
    digitalWrite(BUZZER_PIN, HIGH);
    delay(200);
    digitalWrite(BUZZER_PIN, LOW);
    delay(200);
  }
  lcd.clear();
  lcd.print("INCORRECT PASSWORD");
  lcd.setCursor(0, 1);
  lcd.print("TRY AGAIN");
  delay(2000);
  lcd.clear();
  resetPassword();
}

void resetPassword() {
  password.reset();
  currentPasswordLength = 0;
  for (byte i = 0; i < MAX_PASS_LENGTH; i++) {
    lcd.setCursor(i + 5, 1);
    lcd.print(" ");
  }
}
