#include <OneWire.h>
#include <DallasTemperature.h>

#define ONE_WIRE_BUS 2
#define NUM_TEMPS 4
#define T_A       0
#define T_V       1
#define T_R       2
#define T_BRW     3

#define NUM_RELAIS  8
#define R_MI_ZU_BIT    0
#define R_MI_AUF_BIT   1
#define R_MI_ZU     0x01
#define R_MI_AUF    0x02
#define R_MI        0x03
#define R_NOT_FBH   0x04
#define R_NOT_RAD   0x08
#define R_KESSEL    0x10
#define R_BRW       0x20
#define R_ZIRK      0x40

#define MISCHER_DURATION 208

#define v   5
#define st  0.4
#define fu  0

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature tempSensors(&oneWire);


float temps[NUM_TEMPS];
float tvSoll, tvIst;

float termR;
float dt;
float miGoal;
float miCurr;
int miCount;
int miStart;

byte relais;

void mischer() {
  float mi;
  termR = termR + 0.1 * ((relais & R_MI_AUF)>>1 - relais & R_MI_ZU) * abs(dt);
  if (miCurr == 1 and termR > 0) {
    termR = 0;
  } else if (termR > 5) {
    termR = 5;
  } else if (termR < -5) {
    termR = -5;
  }
  if (relais & R_MI == 0) {
    termR = termR * 0.99;
  }
  tvSoll = v - st * (temps[T_A] - 20) + 20;
  if (tvSoll > 40) {
    tvSoll = 40;
  }
  tvIst = (temps[T_V] + temps[T_R]) / 2;
  dt = tvSoll -tvIst - termR;
  if (abs(dt) > tvSoll/15 - 0.5 - (relais & R_MI == 0? 0: 0.5)) {
    mi = dt;
  } else {
    mi = 0;
  }
  byte rAlt = bitRead(relais, R_MI_ZU_BIT) - bitRead(relais, R_MI_AUF_BIT);
  relais = relais & ~R_MI;
  if (mi > 0) {
    relais |= R_MI_ZU;
  } else if (mi < 0) {
    relais |= R_MI_AUF;
  }
  if (relais & R_BRW) {
    relais &= ~R_MI;
    relais |= R_NOT_FBH;
  } else {
    relais &= ~R_NOT_FBH;
  }

  if (relais & R_NOT_FBH) {
    return;
  }
  if (bitRead(relais, R_MI_ZU_BIT) - bitRead(relais, R_MI_AUF_BIT) != rAlt) {
    miCount++;
    unsigned int timer = millis();
    if (relais & R_MI) {
      miStart = timer;
    } else if (timer > miStart) {
      miCurr = miCurr - (timer - miStart) * rAlt / MISCHER_DURATION;
    }
    if (miCurr < 0) {
      miCurr = 0;
    } else if (miCurr > 1) {
      miCurr = 1;
    }
  }
}

void brw() {
  
}

void strom() {
  
}

void brennerCalc() {
  
}

void accessRelais() {
  
}

void werte() {
  
}

void storeWerte() {
  
}

void fetchTemps() {
  tempSensors.requestTemperatures();
  for (int i=0; i<NUM_TEMPS; i++) {
    temps[i] = tempSensors.getTempCByIndex(i);
  }
}

void setup() {
  Serial.begin(9600);
  Serial.println("START");
  tempSensors.begin();
}

void loop() {
  fetchTemps();
  mischer();
  brw();
  strom();
  brennerCalc();
  accessRelais();
  werte();
  storeWerte();
}

