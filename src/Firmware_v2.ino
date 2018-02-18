/*
 * Project Firmware_v2
 * Description:
 * Author: Sam Hall
 * Date: 2018-02-18
 */

 int onboardLED = D7;
 int count = 0;

 void setup() {

   pinMode(onboardLED, OUTPUT);

 }

 void loop() {
   // To blink the LED, first we'll turn it on...
   digitalWrite(onboardLED, HIGH);

   delay(500);

   // Then we'll turn it off...
   digitalWrite(onboardLED, LOW);

   delay(500);

   if(count > 60) {
     // Get some data
     String data = "Message from photon! ;)";
     // Trigger the integration
     Particle.publish("weighme-heartbeat", data, PRIVATE);

     count = 0;
   }

   count++;
 }
