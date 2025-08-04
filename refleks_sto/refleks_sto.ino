#include "sto.h"

#define GAMEOVER_SCREENSAVER_DELAY 2000 //ms
#define SCREENSAVER_PERIOD 250 //ms
#define POTREBAN_ZETON true

bool stanjaL[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0}, stanjaD[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};

int randomL, randomD;

int interruptCounter=0; //broji tajmerske interrupte i omogucava biranje nove sijalice svake sekunde, odnosno svaki 100-ti put

bool screenSaver = false;
bool screenSaverPos = LOW;
bool screenSaverDelitelj = LOW;
int screenSaverIndeks=0;

bool coinPrev = true;

unsigned int zetoni=0;

unsigned long lastGameOver=0, lastScreenSaverFlip=0;


Sto refleksSto;


ISR(TIMER1_COMPA_vect) { //10ms
  OCR1A += 20000;
  
  if(refleksSto.gameOn) {
    
    interruptCounter++;

    //ispis novih nasumicnih stanja
    if(interruptCounter==33) { //tri puta u sekundi
      interruptCounter=0;
      refleksSto.protekloVreme++; //dodaj jednu sekundu
      //Serial.println(refleksSto.protekloVreme);
      if(!punNiz(stanjaL)) {
        do {
          randomL = random(9);
        } while(stanjaL[randomL]);
        stanjaL[randomL] = 1;
      }
      if(!punNiz(stanjaD)) {
        do {
          randomD = random(9);
        } while(stanjaD[randomD]);
        stanjaD[randomD] = 1;
      }
      refleksSto.WriteSvetla(stanjaL, stanjaD);
      refleksSto.Send();
    }

    //iscitavanje pritisnutih tastera
    refleksSto.ReadTasteri();
  
    for(int i=0; i<9; i++) {
      if(refleksSto.tasteri1[i] && stanjaL[i]) {
        stanjaL[i] = 0;
        refleksSto.scoreL++;
        refleksSto.PustiMuziku(0);
      }
      if(refleksSto.tasteri2[i] && stanjaD[i]) {
        stanjaD[i] = 0;
        refleksSto.scoreD++;
        refleksSto.PustiMuziku(0);
      }
    }
    refleksSto.WriteRezultat(refleksSto.scoreL, refleksSto.scoreD);
    refleksSto.WriteSvetla(stanjaL, stanjaD);
    refleksSto.Send();
  }
}

void setup() {
  Serial.begin(115200);
  refleksSto.gameOn = false;
  
  TCCR1A = 0;           // Init Timer1A
  TCCR1B = 0;           // Init Timer1B
  TCCR1B |= B00000010;  // Prescaler = 8
  OCR1A = 20000;        // Timer Compare1A Register
  TIMSK1 |= B00000010;  // Enable Timer COMPA Interrupt

  
  refleksSto.begin();
  refleksSto.clear();
  randomSeed(analogRead(A0));
  zetoni = 0;
  screenSaver = true;
  delay(1000);
}

void loop() {
  if(!digitalRead(START) && ((POTREBAN_ZETON && zetoni > 0) || (!POTREBAN_ZETON) )) {
    zetoni = zetoni-1;
    for(int i=0;i<9;i++) {stanjaL[i]=0;stanjaD[i]=0;}
    refleksSto.clear();
    refleksSto.zapocniIgru(30);
    lastGameOver = millis();
  }
  if(digitalRead(COIN)==HIGH && POTREBAN_ZETON) {
    zetoni++;
    delay(150);
    refleksSto.clear();
    screenSaver = false;
    lastGameOver = millis();
  }
  Serial.println(zetoni);


  if(millis()-lastGameOver > GAMEOVER_SCREENSAVER_DELAY) {
    screenSaver = true;
  } else screenSaver = false;
  
  if(screenSaver && millis() - lastScreenSaverFlip > SCREENSAVER_PERIOD) {
    screenSaverDelitelj = !screenSaverDelitelj;
    if(screenSaverDelitelj) {
      refleksSto.sendScreenSaver(screenSaverPos);
      screenSaverPos = !screenSaverPos;
    }
    
    if(screenSaverIndeks==9) {
      for(int i=0;i<9;i++) {stanjaL[i]=0;stanjaD[i]=0;}
      screenSaverIndeks=0;
    } else {
      stanjaL[screenSaverIndeks] = 1;
      stanjaD[screenSaverIndeks] = 1;
      screenSaverIndeks++;
    }
    refleksSto.WriteSvetla(stanjaL, stanjaD);
    refleksSto.Send();
    
    lastScreenSaverFlip = millis();
  }
}


bool punNiz(bool a[9]) {
  for(int i=0; i<9; i++) {
    if(!a[i]) return 0;
  }
  return 1;
}
