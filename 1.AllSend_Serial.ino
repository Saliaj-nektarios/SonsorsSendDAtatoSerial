/* YourDuinoStarter Example: Sketch Template
 - WHAT IT DOES
 - SEE the comments after "//" on each line below
 - CONNECTIONS:
   - 
   - 
 - V1.00 09/11/12
   Questions: terry@yourduino.com */

/*-----( Import needed libraries )-----*/
  #include <HCSR04.h>
  #include "DHT.h"
/*-----( Declare Constants and Pin Numbers )-----*/
  #define DHTPIN 2    // Digital pin connected to the DHT sensor
  #define SEN_UltraSonicTrigPin 48 //attach pin D3 Arduino to pin Trig  of HC-SR04
  #define SEN_UltraSonicEchoPin 49 // attach pin D4 Arduino to pin Echo of HC-SR04

  const int SEN_LdrOut = A8;
/*-----( Declare objects )-----*/
  // defines variables
  int Val_humi = 0; // Humidity variables
  int Val_temp = 0;// Temperature variables

  int Val_OutLdr = 0;

  long Val_duration; // variable for the duration of sound wave travel
  float Val_distance; // variable for the distance measurement
  int Val_distpercent;

  //xbee side variables
  int analogMSB1 = 0;
  int analogLSB1 = 0;
  int Val_SoilHygro = 0;

  int analogMSB2 = 0;
  int analogLSB2 = 0;
  int Val_InLdr = 0;

  int analogMSB3 = 0;
  int analogLSB3 = 0;
  int Val_Mq07Gas = 0;
/*-----( Declare Variables )-----*/
    #define DHTTYPE DHT11   // DHT 11

    DHT dht(DHTPIN, DHTTYPE);
    HCSR04 hc(SEN_UltraSonicTrigPin, SEN_UltraSonicEchoPin); //initialisation class HCSR04 (trig pin , echo pin)

void setup (){
  Serial.begin(9600);
  
  Serial.println(F("DHT11 test!"));
  Serial.println(F("Ldr Test")); // print some text in Serial Monitor
  Serial.println(F("Ultrasonic Sensor HC-SR04 Test")); // print some text in Serial Monitor
  Serial.println(F("Xbee Router Test")); // print some text in Serial Monitor
  Serial.println("with Arduino UNO R3");
  
  dht.begin();
  
  pinMode(SEN_UltraSonicTrigPin, OUTPUT); // Sets the trigPin as an OUTPUT
  pinMode(SEN_UltraSonicEchoPin, INPUT); // Sets the echoPin as an INPUT
}
void loop() {
  Read_Sen_Dht11_Loop();
  Read_Sen_OutLDR_Loop();
  Read_Sen_UltraSonic_Loop();
  Read_RemoteXbee_Loop();
  Print_Serial_loop();
}

void Read_Sen_Dht11_Loop() { 
  delay(1000);
  Val_humi = dht.readHumidity();// Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  Val_temp = dht.readTemperature(); // Read temperature as Celsius (the default)
  // Check if any reads failed and exit early (to try again).
  if (isnan(Val_humi) || isnan( Val_temp )) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }
}
void Read_Sen_OutLDR_Loop() {
  delay(1000);
  Val_OutLdr = analogRead(SEN_LdrOut) ;
  Val_OutLdr =   map(Val_OutLdr, 100, 1023, 10, 99);
}
void Read_Sen_UltraSonic_Loop() {
  delay(1000);
  Val_distance = hc.dist();
  Val_distpercent =   map(Val_distance, 17, 0, 0, 99);
}
void Read_RemoteXbee_Loop() {
  //checks for Xbee
  if (Serial.available() >= 29) { // ean einai to frame me 29 bits
    if (Serial.read() == 0x7E) { // ean to frame ksekina me to 7E
      for (int i = 1; i < 21; i++) {
        byte discardbyte = Serial.read();
      }
      //Read Soil hygro
      analogMSB1 = Serial.read();
      analogLSB1 = Serial.read();
      Val_SoilHygro = analogLSB1 + (analogMSB1 * 256);
      Val_SoilHygro =   map(Val_SoilHygro, 400, 1023, 99, 0);
      
      //Read LDR SENSOR
      analogMSB2 = Serial.read();
      analogLSB2 = Serial.read();
      Val_InLdr = analogLSB2 + (analogMSB2 * 256);
      Val_InLdr =   map(Val_InLdr, 0, 900, 0, 99);

      //Read Gas Sensor Mq07
      analogMSB3 = Serial.read();
      analogLSB3 = Serial.read();
      Val_Mq07Gas = analogLSB3 + (analogMSB3 * 256);
      Val_Mq07Gas =   map(Val_Mq07Gas, 400, 1030, 0, 99);
    }
  }
}

void Print_Serial_loop(){
  Serial.println("========================================");
  Serial.print(F("Humidity:")); Serial.print(Val_humi); (Serial.println(F("% ")));
  Serial.print(F("Temperature:"));  Serial.print(Val_temp); Serial.println(F("°C "));
  Serial.print("Distance:");Serial.print(Val_distance);Serial.print("cm");Serial.print(" Distance percent:");Serial.print(Val_distpercent);Serial.println("%");
  Serial.print("Out LDR Value is: "); Serial.print(Val_OutLdr);Serial.println("%");
  Serial.println("****************************");
  Serial.print("Value In Ldr is: "); Serial.print(Val_InLdr);Serial.println("%");
  Serial.print("Value of SoilHygro sensor is: "); Serial.print(Val_SoilHygro);Serial.println("%");
  Serial.print("Value of gas Mq07 is: "); Serial.print(Val_Mq07Gas);Serial.println("%");
  Serial.println("========================================");
}

void setRem_LED_ALERT_Org_GasMq07_DIO4(char value) {
  Serial.write(0x7E);
  Serial.write(byte(0x0));
  Serial.write(0x10);
  Serial.write(0x17);
  Serial.write(byte(0x0));
  Serial.write(byte(0x0));
  Serial.write(byte(0x0));
  Serial.write(byte(0x0));
  Serial.write(byte(0x0));
  Serial.write(byte(0x0));
  Serial.write(byte(0x0));
  Serial.write(0xFF);
  Serial.write(0xFF);
  Serial.write(0xFF);
  Serial.write(0xFE);
  Serial.write(0x02);
  Serial.write('D');
  Serial.write('4');
  Serial.write(value);
  long sum = 0x17 + 0xFF + 0xFF + 0xFF + 0xFE + 0x02 + 'D' + '4' + value;
  Serial.write(0xFF - (sum & 0xFF));
}
void setRem_LED_Blue_MOV_Sunshade_DIO11(char value) { //ldr->Sunshade ok2 xbee
  Serial.write(0x7E);
  Serial.write(byte(0x0));
  Serial.write(0x10);
  Serial.write(0x17);
  Serial.write(byte(0x0));
  Serial.write(byte(0x0));
  Serial.write(byte(0x0));
  Serial.write(byte(0x0));
  Serial.write(byte(0x0));
  Serial.write(byte(0x0));
  Serial.write(byte(0x0));
  Serial.write(0xFF);
  Serial.write(0xFF);
  Serial.write(0xFF);
  Serial.write(0xFE);
  Serial.write(0x02);
  Serial.write('P');
  Serial.write('1');
  Serial.write(value);
  long sum = 0x17 + 0xFF + 0xFF + 0xFF + 0xFE + 0x02 + 'P' + '1' + value;
  Serial.write(0xFF - (sum & 0xFF));
}
void setRem_LED_Blue_Yellow_ForggerSpraySytem_DIO12(char value) {
  Serial.write(0x7E);
  Serial.write(byte(0x0));
  Serial.write(0x10);
  Serial.write(0x17);
  Serial.write(byte(0x0));
  Serial.write(byte(0x0));
  Serial.write(byte(0x0));
  Serial.write(byte(0x0));
  Serial.write(byte(0x0));
  Serial.write(byte(0x0));
  Serial.write(byte(0x0));
  Serial.write(0xFF);
  Serial.write(0xFF);
  Serial.write(0xFF);
  Serial.write(0xFE);
  Serial.write(0x02);
  Serial.write('P');
  Serial.write('2');
  Serial.write(value);
  long sum = 0x17 + 0xFF + 0xFF + 0xFF + 0xFE + 0x02 + 'P' + '2' + value;
  Serial.write(0xFF - (sum & 0xFF));
} 
