int ledPin = 7;
int i = 0;
void setup() {
  // put your setup code here, to run once:
  pinMode(ledPin, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(ledPin,LOW);
  delay(1000);
  digitalWrite(ledPin,HIGH);
  for (i=0;i<5;i++){
    digitalWrite(ledPin,LOW);
    delay(100);
    digitalWrite(ledPin,HIGH);
    delay(100);
  }
  while(1);
  
}
