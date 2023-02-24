//Includes and defines for MQ135
#include <MQUnifiedsensor.h>
#define placa "Arduino UNO"       //Arduino board type
#define Voltage_Resolution 5      //MQ voltage res
#define pin A0                    //MQ Pin
#define type "MQ-135"             //MQ version
#define ADC_Bit_Resolution 10     //Bit resolution
#define RatioMQ135CleanAir 3.6    //"Dirty" to clean air ratio of specific MQ
//MQ135
MQUnifiedsensor MQ135(placa, Voltage_Resolution, ADC_Bit_Resolution, pin, type); //MQ setup
const int red_led_aq = 3;
//SETUP Sensor 1
const int trigPin = 5;
const int echoPin = 6;
int red_light_pin= 4;     //RGB LED 1
int green_light_pin = 7;  //RGB LED 1
int blue_light_pin = 8;   //RGB LED 1
//SETUP Sensor 1
const int trigPin2 = 9;
const int echoPin2 = 10;
int red_light_pin2= 11;     //RGB LED 2
int green_light_pin2 = 12;  //RGB LED 2
int blue_light_pin2 = 13;   //RGB LED 2

void setup() {
  //SETUP Sensor 1
  pinMode(trigPin, OUTPUT); 
  pinMode(echoPin, INPUT); 
  pinMode(red_light_pin, OUTPUT);
  pinMode(green_light_pin, OUTPUT);
  pinMode(blue_light_pin, OUTPUT);
  //SETUP Sensor 2
  pinMode(trigPin2, OUTPUT); 
  pinMode(echoPin2, INPUT); 
  pinMode(red_light_pin2, OUTPUT);
  pinMode(green_light_pin2, OUTPUT);
  pinMode(blue_light_pin2, OUTPUT);
  Serial.begin(9600);
  //MQ135
  MQ135.setRegressionMethod(1);
  MQ135.init();  //Initialize MQ
  Serial.print("Calibrating please wait.");
  float calcR0 = 0;
  for(int i = 1; i<=10; i ++) //Calibrate MQ
  {
    MQ135.update(); // Update data, the arduino will read the voltage from the analog pin
    calcR0 += MQ135.calibrate(RatioMQ135CleanAir);
    Serial.print(".");
  }
  MQ135.setR0(calcR0/10);
  Serial.println("  done!.");
  
  if(isinf(calcR0)) {Serial.println("Warning: Conection issue, R0 is infinite (Open circuit detected) please check your wiring and supply"); while(1);}
  if(calcR0 == 0){Serial.println("Warning: Conection issue found, R0 is zero (Analog pin shorts to ground) please check your wiring and supply"); while(1);}
  /*****************************  MQ CAlibration ********************************************/ 
}

void loop() {
  ViewPlace(); //Run car park sensor main function
  aqTest(); //Air Quality Test
   
  delay(4000);
}

void ViewPlace()
{
  int free_Space = 0;
  free_Space = free_Space + run_instance(trigPin,echoPin,red_light_pin,green_light_pin,blue_light_pin);       //adds a free space, or not, depending on the result
  free_Space = free_Space + run_instance(trigPin2,echoPin2,red_light_pin2,green_light_pin2,blue_light_pin2);  //adds a free space, or not, depending on the result

  Serial.println("++++++++++++++++++++++++++++");
  Serial.print("Free Space in Car Park: ");
  Serial.println(free_Space);
  Serial.println("++++++++++++++++++++++++++++");
}

int run_instance(int trigger_pin, int echo_pin, int red_pin, int green_pin, int blue_pin){
  float duration;
  //Default method for calculating distance
  digitalWrite(trigger_pin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigger_pin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigger_pin, LOW);

  duration = pulseIn(echo_pin, HIGH);
  int distance = duration*0.034/2;
  Serial.println("-------------------------");
  Serial.print("Distance: ");
  Serial.println(distance);
  Serial.println("-------------------------");

  if(distance>26)
  {
   RGB_color(0, 255, 0, red_pin, green_pin, blue_pin); // Green
   return 1;
  }
  else //If an object is under the sensor, it returns 0, meaning this sensor is occupied
  {
    RGB_color(255, 0, 0, red_pin, green_pin, blue_pin); // Red
    return 0;
  }
}
//Function for defining RGB colour, given the pin and respective value
void RGB_color(int red_light_value, int green_light_value, int blue_light_value, int red_pin, int green_pin, int blue_pin)
 {
  analogWrite(red_pin, red_light_value);
  analogWrite(green_pin, green_light_value);
  analogWrite(blue_pin, blue_light_value);
}

void aqTest(){
  MQ135.update();
  /*MQ135.setA(110.47); MQ135.setB(-2.862);
  float CO2 = MQ135.readSensor();
  Serial.print("CO2 level: "); Serial.println(CO2); 
  if(CO2>=1){
    Serial.println(" -CO2 LEVEL TOO HIGH!!!"); 
    digitalWrite(red_led_aq, HIGH);
  }
  else{
    digitalWrite(red_led_aq, LOW);
  }*/
  MQ135.setA(77.255); MQ135.setB(-3.18);
  float alc = MQ135.readSensor();
  Serial.print("Alcohol level: "); Serial.println(alc); 
  if(alc>=1.5){
    Serial.println(" -ALCOHOL LEVEL TOO HIGH!!!"); 
    analogWrite(red_led_aq, 100);
  }
  else{
    analogWrite(red_led_aq, 0);
  }
}
