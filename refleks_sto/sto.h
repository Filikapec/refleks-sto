#ifndef STO_H
#define STO_H

#include "pinovi.h"
#include "arduino.h"

#define DEFAULT_TRAJANJE_IGRE 30 //s

const int cifre[10][8] = {
  1, 1, 1, 0, 1, 1, 0, 1, // 0
  0, 1, 0, 0, 0, 0, 0, 1, // 1
  1, 1, 0, 1, 1, 1, 0, 0, // 2
  1, 1, 0, 1, 0, 1, 0, 1, // 3
  0, 1, 1, 1, 0, 0, 0, 1, // 4
  1, 0, 1, 1, 0, 1, 0, 1, // 5
  1, 0, 1, 1, 1, 1, 0, 1, // 6
  1, 1, 0, 0, 0, 0, 0, 1, // 7
  1, 1, 1, 1, 1, 1, 0, 1, // 8
  1, 1, 1, 1, 0, 1, 0, 1  // 9
};

const bool screen_saver_display[2][8] = {
  0, 0, 0, 1, 1, 1, 0, 1,
  1, 1, 1, 1, 0, 0, 0, 0
};

class Sto {
  private:
    short brojZetona;
    bool readBuff1[16], readBuff2[16];
    void prepakuj();
    bool gameOver = false;
  public:
    int scoreL, scoreD;
    bool tasteri1[9], tasteri2[9];
    bool writeBuff1[32], writeBuff2[32];
    bool gameOn;
    int protekloVreme = 0; //broji koliko je puta aktiviran vremenski interrupt
    void pustiMuziku(int n);
    
    Sto();
    void begin();
    void clear();
    void zapocniIgru();
    void zapocniIgru(int trajanjeSekunde); //trajanje igre u sekundama
    void ReadTasteri();
    void WriteSvetla(bool svetla1[9], bool svetla2[9]);
    void WriteRezultat(int levi, int desni);
    void Kraj();
    void PustiMuziku(int n);
    void Send();
    void send32(bool buff[32]);
    void send32(bool buff1[32], bool buff2[32]);
    void sendScreenSaver(bool gd);
};

#endif
