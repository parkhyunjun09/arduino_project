#include <SoftwareSerial.h>
#include <LiquidCrystal_I2C.h>
#include<Wire.h>

#define SoIL_HUMI A0
#define BT_RX 2
#define BT_TX 3
#define PWM_1 6
#define PWM_2 7
#define RGB_R 10
#define RGB_G 9
#define RGB_B 11

#define On_Time 2000
#define PUMP_OFF 0
#define PUMP_START 170
#define PUMP_MAX 230
#define LCD_ADDR 0x27
LiquidCrystal_I2C lcd(LCD_ADDR,16,2);
SoftwareSerial bt(BT_TX,BT_RX);
unsigned long pm = 0;
int soilHumidity;

void initPin(){
  Serial.begin(9600);
  bt.begin(9600);
  lcd.init();
  pinMode(PWM_1,OUTPUT);
  pinMode(PWM_2,OUTPUT);
  pinMode(RGB_R,OUTPUT);
  pinMode(RGB_G,OUTPUT);
  pinMode(RGB_B,OUTPUT);
  digitalWrite(RGB_R,LOW);
  digitalWrite(RGB_G,LOW);
  digitalWrite(RGB_B,LOW);
  analogWrite(PWM_1,0);
  analogWrite(PWM_2,0);
}
void introLcd(){
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0,0);
  lcd.print("SMART PLANTING");
}
void printlcd(){
  lcd.init();
  lcd.clear();
  lcd.backlight();
  lcd.setCursor(0,0);
  lcd.print(soilHumidity);
  lcd.print("%");
  lcd.setCursor(0,1);
  if(soilHumidity < 20) lcd.print("Need Water");
  else if(soilHumidity < 30) lcd.print("Dry");
  else if(soilHumidity < 60) lcd.print("Wet");
  else lcd.print("Enough");
}
void writeRGB(bool R,bool G,bool B){
  digitalWrite(RGB_R,R);
  digitalWrite(RGB_G,G);
  digitalWrite(RGB_B,B);
}
void calSoilHumidity(){
  SoilHumidity = map(analogRead(SOIL_HUMI),1000,400,0,100);
  if(SoilHumidity > 100){
    SoilHumidity = 100;
  }
  else if(SoilHumidity < 0){
    SoilHumidity = 0;
  }
}
void turn_on_pump(){
  if(SoilHumidity < 20){
    delay(2000);
    lcd.clear();
    lcd.noBacklight();
    delay(250);
    for(int i=PUMP_START;i<PUMP_MAX;i++){
      analogWrite(PWM_1,i);
      delay(5);
    }
    delay(On_Time);
    analogWrite(PWM_1,PUMP_OFF);
    delay(250);
  }
  else{
    analogWrite(PWM_1,PUMP_OFF);
  }
}
void chkBT(){
  if(bt.available()>0){
    byte data = bt.read();
    if(data == 1){
      writeRGB(HIGH,LOW,HIGH);
    }
    else if(data ==2){
      writeRGB(LOW,LOW,LOW);
    }
    else if(data == 3){
      turn_on_pump();
    }
  }
}
void setup() {
  initPin();
  introLcd();
  delay(On_Time);
  writeRGB(HIGH,LOW,HIGH);
}

void loop() {
  calcSoilHumidity();
  unsigned long cm = millis();
  if(cm-pm>=1000){
    pm=cm;
    bt.print(soilHumidity);
  }
  printLcd();
  chkBT();
  if(soilHumidity<20){
    bt.print(soilHumidity);
    delay(500);
    turn_on_pump();
  }
}
