const int M1_ENC_1_PIN=15;
const int M1_ENC_2_PIN=16;
const int M2_ENC_1_PIN=17;
const int M2_ENC_2_PIN=18;

volatile int m1_dir = 0;
volatile int m2_dir = 0;
long m1_steps, m2_steps, m1_steps_last, m2_steps_last;
unsigned long next_print_ms;

const byte buffSize=40;
char inputBuffer[buffSize];
int bytesIn=0;

void setup() {
  m1_steps=m2_steps=0;
  m1_steps_last=m2_steps_last=0;
  next_print_ms=0;

  Serial.begin(115200);
  pinMode(M1_ENC_1_PIN, INPUT);
  pinMode(M1_ENC_2_PIN, INPUT);
  attachInterrupt(M1_ENC_1_PIN, m1_interrupt_change, CHANGE);
  pinMode(M2_ENC_1_PIN, INPUT);
  pinMode(M2_ENC_2_PIN, INPUT);
  attachInterrupt(M2_ENC_1_PIN, m2_interrupt_change, CHANGE);
}

void loop() {
  if (millis()>=next_print_ms || m1_steps!=m1_steps_last || m2_steps!=m2_steps_last){
    // the "last" variables are used to prevent spamming of the same message over and over
    Serial.println((String)"m1="+m1_steps+",m2="+m2_steps);
    
    // ever 10 seconds or so send the steps regardless of whether they're changed or not
    next_print_ms=millis()+10000;
    
    m1_steps_last=m1_steps;
    m2_steps_last=m2_steps;
  }

  getSerialData();
}

void getSerialData(){
  if (Serial.available() > 0){
    char x=Serial.read();
    if (x=='\n'){
      inputBuffer[bytesIn]=0;
      serialDataHandler(inputBuffer);
      bytesIn=0;
    } else {
      inputBuffer[bytesIn]=x;
      bytesIn++;
      if (bytesIn==buffSize){
        bytesIn=buffSize-1;
      }
    }
  }
}

void serialDataHandler(char inputString[buffSize]){
  if (strcmp(inputString, "get_encoders")==0){
    Serial.println((String)"m1="+m1_steps+",m2="+m2_steps);
  } else if (strcmp(inputString, "reset_encoders")==0){
    m1_steps=m2_steps=0;
    Serial.println((String)"m1="+m1_steps+",m2="+m2_steps);
  }
}

void m1_interrupt_change() {
  if (digitalRead(M1_ENC_1_PIN) == digitalRead(M1_ENC_2_PIN)) {
    // encoder channels are the same, direction is positive
    m1_dir = 1;
    m1_steps++;
  } else {
    // encoder channels are not the same, direction is negative
    m1_dir = -1;
    m1_steps--;
  }
}

void m2_interrupt_change() {
  if (digitalRead(M2_ENC_1_PIN) == digitalRead(M2_ENC_2_PIN)) {
    m2_dir = 1;
    m2_steps++;
  } else {
    m2_dir = -1;
    m2_steps--;
  }
}
