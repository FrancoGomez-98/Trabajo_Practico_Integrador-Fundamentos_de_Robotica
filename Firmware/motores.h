// Pestaña: motores.h
#ifndef MOTORES_H
#define MOTORES_H

#include <Arduino.h>

const int ENA = 14; const int IN1 = 27; const int IN2 = 26;
const int IN3 = 25; const int IN4 = 33; const int ENB = 32;

const int canalM1 = 0; const int canalM2 = 1;
const int freqPWM = 5000; 
const int resPWM = 8;     

void configurarMotores() {
  pinMode(IN1, OUTPUT); pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT); pinMode(IN4, OUTPUT);

  ledcAttachChannel(ENA, freqPWM, resPWM, canalM1);
  ledcAttachChannel(ENB, freqPWM, resPWM, canalM2);
}

// INVERTIDO LÓGICAMENTE PARA CAMBIAR SENTIDO DE MARCHA DE M1
void moverMotor1(int velocidad) {
  if (velocidad > 0) {
    digitalWrite(IN1, LOW); digitalWrite(IN2, HIGH);
    ledcWrite(ENA, velocidad);
  } else if (velocidad < 0) {
    digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW);
    ledcWrite(ENA, abs(velocidad));
  } else {
    digitalWrite(IN1, LOW); digitalWrite(IN2, LOW); ledcWrite(ENA, 0); 
  }
}

// INVERTIDO LÓGICAMENTE PARA CAMBIAR SENTIDO DE MARCHA DE M2
void moverMotor2(int velocidad) {
  if (velocidad > 0) {
    digitalWrite(IN3, LOW); digitalWrite(IN4, HIGH);
    ledcWrite(ENB, velocidad);
  } else if (velocidad < 0) {
    digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW);
    ledcWrite(ENB, abs(velocidad));
  } else {
    digitalWrite(IN3, LOW); digitalWrite(IN4, LOW); ledcWrite(ENB, 0); 
  }
}

#endif