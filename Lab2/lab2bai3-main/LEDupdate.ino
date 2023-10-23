int LED =2;

void setup() {
  // put your setup code here, to run once:
 pinMode(LED, OUTPUT);
}
unsigned long previousMillis = 0;

void loop() {
  // put your main code here, to run repeatedly:
if(millis() - previousMillis >=  1000)
  {
  previousMillis = millis();
  digitalWrite(LED, !digitalRead(LED));
  }
}
