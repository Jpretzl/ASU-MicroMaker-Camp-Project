#include <LiquidCrystal.h>
#include <Servo.h>
#include <IRremote.h>

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
Servo servo_10;

int dispensed = 0;
bool open = false;
int lastDispensed = -1;
int pos = 0;
int RECV_PIN = 7;
bool IRbuttonPressed = false;
int cooldown = 0;

uint8_t Heart[8] = {
  0x00,
  0x00,
  0x0A,
  0x1F,
  0x0E,
  0x04,
  0x00,
  0x00
};

void setup()
{
  pinMode(A0, INPUT);
  Serial.begin(9600);
  pinMode(6, OUTPUT);
  pinMode(7, INPUT);
  lcd.begin(16, 2);
  lcd.createChar(0, Heart);
  servo_10.attach(10);
  servo_10.write(0);
  IrReceiver.begin(RECV_PIN, DISABLE_LED_FEEDBACK);
  IrReceiver.blink13(true);
}

unsigned long readInfrared() {
  unsigned long result = -1;
  if (IrReceiver.decode()) {
    unsigned long code = IrReceiver.decodedIRData.decodedRawData;
    result = code;
  }
  IrReceiver.resume();
  return result;
}

void loop()
{
  unsigned long button = readInfrared();
  Serial.println(dispensed);

  if (button != 0xFFFFFFFF && button != 0x00000000) {
    IRbuttonPressed = true;
  } else { IRbuttonPressed = false; };
  int sensorValue = (IRbuttonPressed ? 1023 : analogRead(A0));
  int valueDiff = 1000 - sensorValue;

  if ((sensorValue >= 1020) && (open == false) && (cooldown <= 0)) {
    dispensed += 1;
    open = true;
    cooldown = 2000;
  }
  else if ((sensorValue <= 1020) && (open == true) && (cooldown <= 1900)) {
    open = false;
  }

  if (open == false) {
    servo_10.write(0);
  } else if (open == true) {
    servo_10.write(90);
  }
  if (dispensed != lastDispensed) {
    lcd.home();
    lcd.print("Treats: ");
    lcd.print(dispensed);
    lcd.print(" ");
    lcd.write((uint8_t)0);
    lastDispensed = dispensed;
  }
  delay(10);
  cooldown -= 10;
  cooldown = max(0, cooldown);
};
