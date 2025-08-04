#include "sto.h"

Sto::Sto() {
  brojZetona = 0;
}
void Sto::begin() {
  pinMode(SCLD, OUTPUT);
  pinMode(CSD, OUTPUT);
  pinMode(SCLB, OUTPUT);
  pinMode(CSB, OUTPUT);
  pinMode(DIN1, OUTPUT);
  pinMode(DOUT1, INPUT);
  pinMode(DIN2, OUTPUT);
  pinMode(DOUT2, INPUT);
  pinMode(COIN, INPUT_PULLUP);
  pinMode(BUZ, OUTPUT);
  pinMode(START, INPUT_PULLUP);
  gameOn = false;
}

void Sto::send32(bool buff[32]) {
  digitalWrite(CSD, LOW);
  for(int i=0; i<32; i++) {
    digitalWrite(DIN1, buff[i]);
    digitalWrite(SCLD, HIGH);
    digitalWrite(SCLD, LOW);
  }
  digitalWrite(CSD, HIGH);
}

void Sto::send32(bool buff1[32], bool buff2[32]) {
  digitalWrite(CSD, LOW);
  for(int i=0; i<32; i++) {
    digitalWrite(DIN1, buff1[i]);
    digitalWrite(DIN2, buff2[i]);
    digitalWrite(SCLD, HIGH);
    digitalWrite(SCLD, LOW);
  }
  digitalWrite(CSD, HIGH);
}

void Sto::WriteSvetla(bool svetla1[9], bool svetla2[9]) {
  writeBuff1[5] = svetla1[0];
  writeBuff1[0] = svetla1[1];
  writeBuff1[14] = svetla1[2];
  writeBuff1[7] = svetla1[3];
  writeBuff1[6] = svetla1[4];
  writeBuff1[3] = svetla1[5];
  writeBuff1[4] = svetla1[6];
  writeBuff1[2] = svetla1[7];
  writeBuff1[1] = svetla1[8];

  writeBuff2[5] = svetla2[5];
  writeBuff2[0] = svetla2[6];
  writeBuff2[14] = svetla2[8];
  writeBuff2[7] = svetla2[3];
  writeBuff2[6] = svetla2[4];
  writeBuff2[3] = svetla2[7];
  writeBuff2[4] = svetla2[2];
  writeBuff2[2] = svetla2[0];
  writeBuff2[1] = svetla2[1];
  
}

void Sto::WriteRezultat(int levi, int desni) { //24 16 8
  int rez1[3], rez2[3];
  
  levi = levi%1000;
  levi = levi%1000;

  rez1[0] = levi/100;
  rez1[1] = (levi%100)/10;
  rez1[2] = levi%10;
  rez2[0] = desni/100;
  rez2[1] = (desni%100)/10;
  rez2[2] = desni%10;
  
  for(int i=0; i<8; i++) {
    if(i!=6) {
      if(rez1[0]>0) writeBuff1[24+i] = cifre[rez1[0]][i];
      if(rez2[0]>0) writeBuff2[24+i] = cifre[rez2[0]][i];
      if(rez1[1]>0) writeBuff1[16+i] = cifre[rez1[1]][i];
      if(rez2[1]>0) writeBuff2[16+i] = cifre[rez2[1]][i];
      writeBuff1[8+i] = cifre[rez1[2]][i];
      writeBuff2[8+i] = cifre[rez2[2]][i];
    }
  }
  
}

void Sto::Send() {
  send32(writeBuff1, writeBuff2);
}

void Sto::sendScreenSaver(bool gd) {
  for(int i=0; i<8; i++) {
    if(i!=6) {
      writeBuff1[24+i] = screen_saver_display[gd][i];
      writeBuff2[24+i] = screen_saver_display[gd][i];
      writeBuff1[16+i] = screen_saver_display[!gd][i];
      writeBuff2[16+i] = screen_saver_display[!gd][i];
      writeBuff1[8+i] = screen_saver_display[gd][i];
      writeBuff2[8+i] = screen_saver_display[gd][i];
    }
  }
  Send();
}


void Sto::clear() {
  digitalWrite(CSD, LOW);
  digitalWrite(DIN1, LOW);
  digitalWrite(DIN2, LOW);
  for(int i=0; i<32; i++) {
    digitalWrite(SCLD, HIGH);
    digitalWrite(SCLD, LOW);
  }
  digitalWrite(CSD, HIGH);
  
  for(int i=0; i<32; i++) {
    writeBuff1[i]=0;
    writeBuff2[i]=0;
  }
}

void Sto::zapocniIgru(int trajanjeSekunde) {
  protekloVreme = 0;
  scoreL = 0;
  scoreD = 0;
  WriteRezultat(scoreL, scoreD);
  Send();
  for(int i=3; i>0; i--) {
    WriteRezultat(i, i);
    Send();
    delay(1000);
  }
  gameOn = true;
  //delay(trajanjeSekunde*1000);
  while(protekloVreme < trajanjeSekunde*3) {
    delay(1000);
  }
  gameOn = false;
  delay(1000);
  Kraj();
}

void Sto::zapocniIgru() {
  zapocniIgru(DEFAULT_TRAJANJE_IGRE);
}

void Sto::Kraj() {
  const bool pobednik[9] = {1, 1, 1, 1, 1, 1, 1, 1, 1}, gubitnik[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
  tone(BUZ, 130.81, 100);
  delay(100);
  for(int i=0; i<3; i++) {
    tone(BUZ, 146.83, 100);
    delay(100);
    if(scoreL > scoreD) {
      WriteSvetla(pobednik, gubitnik);
    } else if(scoreD > scoreL) {
      WriteSvetla(gubitnik, pobednik);
    } else {
      WriteSvetla(pobednik, pobednik);
    }
    Send();
    tone(BUZ, 164.81, 500);
    delay(500);
    WriteSvetla(gubitnik, gubitnik);
    Send();
    tone(BUZ, 146.83, 200);
    delay(200);
    tone(BUZ, 130.81, 100);
    delay(100);
  }
  
  
}

void Sto::ReadTasteri() {
  
  digitalWrite(CSB, LOW);
  digitalWrite(CSB, HIGH);
  
  for(int i = 0; i<16; i++) {
    digitalWrite(SCLB, HIGH);
    readBuff1[i] = !digitalRead(DOUT1);
    readBuff2[i] = !digitalRead(DOUT2);
    digitalWrite(SCLB, LOW);
  }

  prepakuj();
}

void Sto::prepakuj() {
  tasteri1[0] = readBuff1[12];
  tasteri1[1] = readBuff1[3];
  tasteri1[2] = readBuff1[2];
  tasteri1[3] = readBuff1[14];
  tasteri1[4] = readBuff1[13];
  tasteri1[5] = readBuff1[6];
  tasteri1[6] = readBuff1[11];
  tasteri1[7] = readBuff1[5];
  tasteri1[8] = readBuff1[4];
  
  tasteri2[0] = readBuff2[5];
  tasteri2[1] = readBuff2[4];
  tasteri2[2] = readBuff2[11];
  tasteri2[3] = readBuff2[14];
  tasteri2[4] = readBuff2[13];
  tasteri2[5] = readBuff2[12];
  tasteri2[6] = readBuff2[3];
  tasteri2[7] = readBuff2[6];
  tasteri2[8] = readBuff2[2];
}

void Sto::PustiMuziku(int n) {
  if(n==0) {
      tone(BUZ, 1800, 200);
  } else if(n==1) {
    for(int i = 0; i<3; i++) {
      tone(BUZ, 130.81, 100);
      delay(100);
      tone(BUZ, 146.83, 100);
      delay(100);
      tone(BUZ, 164.81, 100);
      delay(100);
      tone(BUZ, 164.81, 100);
      delay(100);
      tone(BUZ, 164.81, 100);
      delay(100);
      tone(BUZ, 146.83, 100);
      delay(100);
      tone(BUZ, 146.83, 100);
      delay(100);
      tone(BUZ, 130.81, 100);
      delay(100);
    }
  }
}
