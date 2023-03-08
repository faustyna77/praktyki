#include <Arduino.h>

#include<SPI.h>

#include <SD.h> //bibliotek do obsługi karty SD

const int CSPin=10;// pin do , którego podpinam karte SD

int czas=0;

#define ENCA 2 // pin nr.2 enkodera

#define ENCB 3 //pin nr 3 do obsługi enkodera

#define PWM 6// pin nr.6 do generacji sygnału PWM max =255

#define IN2 7// pin 7 i 8 do sterowania kierunkiem obrotu silnika (piny wychodzące z układu //scalonego

#define IN1 8

File myFile; // utworzenie obiektu myFile

int pos = 0;// pozycja początkowa

long prevT = 0;// czas poprzedni

float eprev = 0;//bład poprzedni

float eintegral = 0;// część całkująca błedu

void setup() {

Serial.begin(9600);//predkosc portu

pinMode(ENCA,INPUT);//ustawienie pinu ENCA jako wejscie

pinMode(ENCB,INPUT);//ustawienie pinu ENCB jako wejscie

attachInterrupt(digitalPinToInterrupt(ENCA),readEncoder,RISING);// przerwanie //attachInterrupt(digitalPinToInterrupt(PIN), FUNKCJA, REAKCJA_NA);

Serial.println("pozycja doceowa ");

pinMode(CSPin,OUTPUT);// ustwenie pinu karty jako wyjscie

if(!SD.begin(CSPin))// jeżeli nie wykryto karty SD

{

Serial.println("blad");// wypisz na monitorze portu ‘bład ‘

return;

}

Serial.println("jest ok");// jeśli nie ‘jest ok’

}

void loop() {

long currT = micros();//aktualny czas mierzony w microsekundach

float deltaT = ((float) (currT - prevT))/( 1.0e6 );//róznica czasu

prevT = currT;

// ustawiam docelowa pozycje

int target;

target = 25000;

// współczyniki regulatora PID

float kp = 5;//współczynnik człou proporcjonalnego

float kd = 2;//współczynnik członu różniczkujacego

float ki = 0.0;// współczynnik członu całkujacego

// różnica czasu

/* long currT = micros();

float deltaT = ((float) (currT - prevT))/( 1.0e6 );

prevT = currT;*/

// błąd (czyli róznica miedzy aktualna pozycja wału a docelową

int e = (pos-target);

// różniczkowanie błędu

float dedt = (e-eprev)/(deltaT);

// człon calkujący bład e

eintegral = eintegral + e*deltaT;

// sygnał wyjsciowy w zależności od e

float u = kp*e + kd*dedt + ki*eintegral;

// moz silnika

float pwr = fabs(u);

if( pwr > 255 ){

pwr = 255;// maksymalnie może być sygnał tylko do 255

}

// kierunek dla 1-prawo -1 lewo

int dir = 1;

if(u<0){

dir = -1;

}

// funkcja setmotor

setMotor(dir,pwr,PWM,IN1,IN2);// funkcja setMotor słuzaca do ustawienia pracy silnika

//gdzie dir jest kierunkiem 1-’naprzód’ -1 -’w tył’ pwr wartościa bezwzglena z sygnału u

// poprzedni błąd

eprev = e;

myFile=SD.open("postest3.txt",FILE_WRITE); //otwarcie/utworzenie pliku “postest3.txt “ wbudowana funkcja do obsługi kart sd sd.open

if(myFile) // jeśli karta SD wraz z plikiem zostały otwarte to rozpoczyna się zapis

{

czas++;//zaczyna się odliczanie czasu o jedna sekunde

myFile.print(pos);//zapis pozycji do pliku

myFile.print(" ");//spacja

myFile.println(target);//zapis wartosci zadanej do pliku

myFile.println(" ");//spacja

myFile.println(czas);//zapis upływu czasu

myFile.close();//zamkniecie pliku

}

}

void setMotor(int dir, int pwmVal, int pwm, int in1, int in2){

analogWrite(pwm,pwmVal);

if(dir == 1){

digitalWrite(in1,HIGH);

digitalWrite(in2,LOW);

}

else if(dir == -1){

digitalWrite(in1,LOW);

digitalWrite(in2,HIGH);

}

else{

digitalWrite(in1,LOW);

digitalWrite(in2,LOW);

}

}

//funkcja odczytujaca wartosc z pinu ENCB jeśli pojawi się stan wysoki na wejsciu enkodera B to //pozycja ulega zwiekszeniu

void readEncoder(){

int b = digitalRead(ENCB);

if(b > 0){

pos++;

}

else{

pos--;

}

}