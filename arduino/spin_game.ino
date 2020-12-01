#define AIN1 0               // H-bridge A, input pin 1
#define AIN2 1               // H-bridge A, input pin 2
#define BIN1 2               // H-bridge B, input pin 1
#define BIN2 3               // H-bridge B, input pin 2
#define hallA A0             // Hall A input pin
#define hallB A1             // Hall B input pin
#define hallC A2             // Hall C input pin
#define threshold 128        // Hall sensor is near north pole if value > threshold

void setup(){
  analogReadResolution(8);   // set A/D for 8 bit resolution
  pinMode(AIN1,OUTPUT);      // set H-bridge control pins as Teensy outputs
  pinMode(AIN2,OUTPUT);
  pinMode(BIN1,OUTPUT);
  pinMode(BIN2,OUTPUT);
  Serial.begin(9600);
}

void off(int pin1, int pin2){
  digitalWrite(pin1,LOW);
  digitalWrite(pin2,LOW);
}

void off_on(int sensor,int pin1,int pin2, int diff){
  off(pin1, pin2);
  delay(diff);
  if(sensor<threshold){
    digitalWrite(pin2,LOW);     // turn first control pin off
    digitalWrite(pin1,HIGH);    // then turn second control pin on
  }
  else{
    digitalWrite(pin1,LOW);     // turn first control pin off
    digitalWrite(pin2,HIGH);    // then turn second control pin on
  }
}



int callCount = 0;
int difficulties[] = {120, 80, 60, 40, 30};
boolean started = false;
int diffIndex = 0;

byte cHist[1];

byte maxC(){
  byte m = cHist[0];
  for(int i=0;i<sizeof(cHist);i++){
    if(cHist[i]>m)
      m = cHist[i];
  }
  return m;
}

byte minC(){
  byte m = cHist[0];
  for(int i=0;i<sizeof(cHist);i++){
    if(cHist[i]<m)
      m = cHist[i];
  }
  return m;
}

void printC(){
  for(int i=0;i<sizeof(cHist);i++){
    Serial.print(cHist[i]);
    Serial.print(", ");
  }
  Serial.println();
}

void loop() {
  if(!started){
    if(Serial.available() > 0) {
      if(Serial.readStringUntil('\n')=="START"){
        started = true;
      }
    }
    return;
  }
  if(started){
    if(Serial.available() > 0) {
      if(Serial.readStringUntil('\n')=="STOP"){
        int res = analogRead(hallC);
        if(res>133 || res<131 || maxC()>134 || minC()<130){
          Serial.println("SUCCESS");
          if(diffIndex!=sizeof(difficulties)-1)
            diffIndex+=1;
        }
        else{
          Serial.print("FAIL ");
          Serial.print("HallC: ");
          Serial.print(res);
          Serial.println();
        }
        off(AIN2, AIN1);
        off(BIN1, BIN2);
        started=false;
        return;
      }
    }
  }

  off_on(analogRead(hallA),AIN2,AIN1, difficulties[diffIndex]);
  off_on(analogRead(hallB),BIN1,BIN2, difficulties[diffIndex]);
  for(int i=0;i<2;i++){
    for(int j=0;j<sizeof(cHist);j++){
      cHist[j+1] = cHist[j];
    }
    cHist[0] = analogRead(hallC);
    delay(5);
  }
  printC();

//  Serial.println(analogRead(hallC));
}
