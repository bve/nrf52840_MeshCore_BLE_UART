#include <led.h>

void ledSetup()
{
  pinMode(LED, OUTPUT); // Set the LED to output mode.
  digitalWrite(LED, LOW);  // Ensure the LED is off initially.
}

void ledBlink(int delayMs)
{
  digitalWrite(LED, HIGH); // Turn the LED on.
  delay(delayMs);          // Wait for a specified time.
  digitalWrite(LED, LOW);  // Turn the LED off.
  delay(delayMs);          // Wait for a specified time.
}