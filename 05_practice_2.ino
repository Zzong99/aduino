int pin = 7;

void setup() {
  // put your setup code here, to run once:
  pinMode(pin, OUTPUT);

}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(pin, 0);
  delay(1000);
  for (int i = 0; i < 5; i++) {   
    digitalWrite(pin, 1); 
    delay(200);              
    digitalWrite(pin, 0);   
    delay(200);
  }     
  digitalWrite(pin, 1); 
  while(1) {}

}
