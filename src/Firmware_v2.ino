#include "HX711ADC.h"

// HX711.DOUT	- pin #A1
// HX711.PD_SCK	- pin #A0
// tare pin - pin #D1

HX711ADC scale;		// parameter "gain" is ommited; the default value 128 is used by the library
int tare_pin = D1;
bool do_tare = false;

void set_tare();

void setup() {

  scale = HX711ADC(A1, A0);

  Serial.begin(38400);

  pinMode(tare_pin, INPUT);
  attachInterrupt(tare_pin, set_tare, RISING);

  scale.set_scale(227000.95);                      // this value is obtained by calibrating the scale with known weights; see the README for details
  scale.tare();				        // reset the scale to 0
}

void loop() {

  Serial.print("raw: ");
  Serial.print(scale.read(), 1);
  Serial.print(" | value: ");
  Serial.print(scale.get_value(10), 1);
  Serial.print(" | units (kg): ");
  Serial.println(scale.get_units(10), 3);

  if(do_tare) {
    Serial.println("zeroing...");
    scale.tare();
    do_tare = false;
  }

  scale.power_down();			        // put the ADC in sleep mode
  delay(2000);
  scale.power_up();
}

void set_tare() {
  do_tare = true;
}
