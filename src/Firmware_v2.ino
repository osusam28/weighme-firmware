#include "HX711ADC.h"

// HX711.DOUT	- pin #A1
// HX711.PD_SCK	- pin #A0
// tare pin - pin #D1

HX711ADC scale;		// parameter "gain" is ommited; the default value 128 is used by the library
int tare_pin = D1;
int led_pin = D7;
bool do_tare = false;

float weight = 0;

int pub_count = 0;

void set_tare();

void setup() {

  scale = HX711ADC(A1, A0);

  Serial.begin(38400);

  pinMode(led_pin, OUTPUT);
  pinMode(tare_pin, INPUT);
  attachInterrupt(tare_pin, set_tare, RISING);

  scale.set_scale(220000.95);                      // this value is obtained by calibrating the scale with known weights; see the README for details
  scale.tare();				        // reset the scale to 0
}

void loop() {
  pub_count++;
  weight = scale.get_units(10);

  Serial.print("raw: ");
  Serial.print(scale.read(), 1);
  Serial.print(" | value: ");
  Serial.print(scale.get_value(10), 1);
  Serial.print(" | units (kg): ");
  Serial.println(weight, 3);

  if(do_tare) {
    Serial.println("zeroing...");
    scale.tare();
    do_tare = false;
  }

  if(pub_count > 10) {
    Serial.println("sending to cloud...");

    String w = String(weight);

    Particle.publish("weighme-heartbeat", w, PRIVATE);
    pub_count = 0;
  }

  scale.power_down(); // put the ADC in sleep mode
  digitalWrite(led_pin, HIGH);
  delay(1000);
  digitalWrite(led_pin, LOW);
  delay(500);
  scale.power_up();
}

void set_tare() {
  do_tare = true;
}
