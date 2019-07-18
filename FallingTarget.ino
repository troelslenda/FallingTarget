#include <TM1637Display.h>

int button[] = {4, 5};
// int button[] = {5 ,6, 7, 8};

int buttonSize = sizeof(button) / sizeof(int);

unsigned long startTime;
unsigned long stopTime;
unsigned long currentTime;

#define CLK 2
#define DIO 3

void displayTime(long time, TM1637Display display, bool show100 = false, bool alwaysColon = false)
{
  uint8_t digit1 = time / 100000 ? display.encodeDigit((time / 100000) % 10) : 0x0;
  uint8_t digit2 = time / 10000 ? display.encodeDigit((time / 10000) % 10) : 0x0;
  uint8_t digit3 = time / 1000 ? display.encodeDigit((time / 1000) % 10) : 0x0;
  uint8_t digit4 = time / 100 ? display.encodeDigit(time / 100 % 10) : 0x0;

  if (time < 9999)
  {
    digit1 = time / 1000 ? display.encodeDigit((time / 1000) % 10) : digit1;
    if ((time / 100))
    {
      digit2 = display.encodeDigit((time / 100) % 10);
      // Colon every half because it looks better.
      digit2 = (((time / 10) % 10) >= 5 || alwaysColon) ? 0x80 | digit2 : digit2;
    }

    digit3 = display.encodeDigit((time / 10) % 10);
    digit4 = show100 ? display.encodeDigit(time % 10) : 0x0;
  }
  // We won't show number of more than 9999 seconds.
  if (time > 999999)
  {
    digit1 = digit2 = digit3 = digit4 = 0b01000000;
  }

  int8_t data[] = {digit1, digit2, digit3, digit4};
  display.setSegments(data);
}

TM1637Display display(CLK, DIO);

void setup()
{
  Serial.begin(9600);
  startTime = millis();
  display.setBrightness(3);

  // Set the trigger pins.
  for (int i = 0; i < buttonSize; i++)
  {
    pinMode(button[i], INPUT_PULLUP);
  }
}

int programmerPin = 8;
int programmerPinPresses = 0;

void loop()
{
  currentTime = millis();
  int pressed = 0;
  for (int i = 0; i < buttonSize; i++)
  {
    pressed += digitalRead(button[i]) == LOW;
  }
  if (!stopTime && pressed == buttonSize)
  {
    stopTime = millis();
  }
  if (stopTime)
  {
    displayTime((stopTime - startTime) / 10, display, true, true);
  }
  else if (startTime)
  {
    displayTime((millis() - startTime) / 10, display);
  }
  if (!startTime && pressed > 0)
  {
    startTime = millis();
  }
  // Reset the number when all buttons are released.
  if (pressed == 0)
  {
    startTime = stopTime = NULL;
    displayTime(0, display);
  }
}
