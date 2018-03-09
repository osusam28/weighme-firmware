#include "HX711ADC.h"

// HX711.DOUT	- pin #A1
// HX711.PD_SCK	- pin #A0
// tare pin - pin #D1

HX711ADC scale;		// parameter "gain" is ommited; the default value 128 is used by the library
int tare_pin = D1;
int led_pin = D7;
bool do_tare = false;

float threshold = 0.015;
float weight = 0;
String name = "";

int pub_count = 0;

void nameHandler(const char *topic, const char *data);
void tareEvent();
void weighEvent();

void setup() {

  scale = HX711ADC(A1, A0);

  Serial.begin(38400);

  pinMode(led_pin, OUTPUT);
  pinMode(tare_pin, INPUT);
  attachInterrupt(tare_pin, tareEvent, RISING);

  Particle.subscribe("particle/device/name", nameHandler);
  Particle.publish("particle/device/name");

  scale.set_scale(220000.95);                      // this value is obtained by calibrating the scale with known weights; see the README for details
  scale.tare();				        // reset the scale to 0
}

void loop() {
  pub_count++;
  weight = scale.get_units(3);

  Serial.print("raw: ");
  Serial.print(scale.read(), 1);
  Serial.print(" | value: ");
  Serial.print(scale.get_value(3), 1);
  Serial.print(" | units (kg): ");
  Serial.println(weight, 3);

  if(do_tare) {
    Serial.println("zeroing...");
    scale.tare();
    do_tare = false;
  }

  if(pub_count > 10) {
    Serial.println("sending to cloud...");
    weighEvent();

    digitalWrite(led_pin, HIGH);
    delay(250);
    digitalWrite(led_pin, LOW);
    delay(250);

    pub_count = 0;
  }

  scale.power_down(); // put the ADC in sleep mode
  digitalWrite(led_pin, HIGH);
  delay(250);
  digitalWrite(led_pin, LOW);
  delay(1500);
  scale.power_up();
}

void nameHandler(const char *topic, const char *data) {
  name = String(data);
}

void tareEvent() {
  do_tare = true;
}

void weighEvent() {
  String data =  "{ \"data\": { \"deviceName\": \"<name>\", \"reading\": {"
    "\"weight\": \"<weight>\", \"units\": \"<units>\" }, \"offset\": \"<offset>\", "
    "\"scaler\": \"<scaler>\", \"threshold\": \"<threshold>\" }}";
  data = data.replace("<name>", name);
  data = data.replace("<weight>", String(weight));
  data = data.replace("<units>", "kg");
  data = data.replace("<offset>", String(scale.get_offset()));
  data = data.replace("<scaler>", String(scale.get_scale()));
  data = data.replace("<threshold>", String(threshold));

  Particle.publish("weighme-heartbeat", data, PRIVATE);
}
