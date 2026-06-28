// Pestaña: control.h
#ifndef CONTROL_H
#define CONTROL_H

#include "motores.h"
#include "encoders.h"

// ==========================================
// CONSTANTES TUNING PID (VERSIÓN FINAL ESTABLE)
// ==========================================

float Kp_M1 = 6.0; float Ki_M1 = 0.25; float Kd_M1 = 0.15;


float Kp_M2 = 1.8; float Ki_M2 = 0.15; float Kd_M2 = 0.08;

// Potencia total liberada para los 12V
const int PWM_MAX = 255; 

// Pisos de fuerza para romper la inercia sin pasarse de largo
const int PWM_MIN_M1 = 175; 
const int PWM_MIN_M2 = 130; 

// VARIABLES INTERNAS DEL CONTROLADOR
unsigned long tiempoAnteriorPID = 0;
float errorAnteriorM1 = 0.0; float integralM1 = 0.0;
float errorAnteriorM2 = 0.0; float integralM2 = 0.0;

// Función auxiliar para normalizar diferencias de ángulos entre -180 y 180
float normalizarErrorAngulo(float angulo) {
  while (angulo < -180.0) angulo += 360.0;
  while (angulo > 180.0)  angulo -= 360.0;
  return angulo;
}

void actualizarControlPID(float objetivoFinalM1, float objetivoFinalM2) {
  unsigned long tiempoActual = micros();
  if (tiempoAnteriorPID == 0) tiempoAnteriorPID = tiempoActual; 
  float dt = (tiempoActual - tiempoAnteriorPID) / 1000000.0; 
  tiempoAnteriorPID = tiempoActual;

  if (dt <= 0.0 || dt > 0.01) dt = 0.005; 

  // Lecturas físicas de los encoders
  float actualM1 = obtenerAnguloM1();
  float actualM2 = obtenerAnguloM2();

  // Cálculo de errores con filtro cíclico para evitar desbocamientos
  float errorM1 = normalizarErrorAngulo(objetivoFinalM1 - actualM1);
  float errorM2 = normalizarErrorAngulo(objetivoFinalM2 - actualM2);

  // PID - Motor 1 (Base)
  float P_M1 = errorM1 * Kp_M1;
  if (abs(errorM1) > 0.5) { integralM1 += errorM1 * dt; } else { integralM1 = 0.0; }
  integralM1 = constrain(integralM1, -40.0, 40.0); 
  float I_M1 = integralM1 * Ki_M1;
  float D_M1 = ((errorM1 - errorAnteriorM1) / dt) * Kd_M1;
  errorAnteriorM1 = errorM1; 
  float accionM1 = P_M1 + I_M1 + D_M1;

  // PID - Motor 2 (Codo)
  float P_M2 = errorM2 * Kp_M2;
  if (abs(errorM2) > 0.5) { integralM2 += errorM2 * dt; } else { integralM2 = 0.0; }
  integralM2 = constrain(integralM2, -30.0, 30.0); 
  float I_M2 = integralM2 * Ki_M2;
  float D_M2 = ((errorM2 - errorAnteriorM2) / dt) * Kd_M2;
  errorAnteriorM2 = errorM2;
  float accionM2 = P_M2 + I_M2 + D_M2;

  // Ejecución física Motor 1 - Tolerancia de arribo a 2.5° para eliminar temblores por peso
  if (abs(errorM1) > 2.5) { 
    int pwmM1 = constrain((int)accionM1, -PWM_MAX, PWM_MAX);
    if (pwmM1 > 0 && pwmM1 < PWM_MIN_M1) pwmM1 = PWM_MIN_M1;
    if (pwmM1 < 0 && pwmM1 > -PWM_MIN_M1) pwmM1 = -PWM_MIN_M1;
    moverMotor1(pwmM1);
  } else {
    moverMotor1(0); // Freno seco y silencioso al llegar a la zona muerta óptima
    integralM1 = 0.0;
  }

  // Ejecución física Motor 2 - Tolerancia de arribo a 0.5°
  if (abs(errorM2) > 0.5) {
    int pwmM2 = constrain((int)accionM2, -PWM_MAX, PWM_MAX);
    if (pwmM2 > 0 && pwmM2 < PWM_MIN_M2) pwmM2 = PWM_MIN_M2;
    if (pwmM2 < 0 && pwmM2 > -PWM_MIN_M2) pwmM2 = -PWM_MIN_M2;
    moverMotor2(pwmM2);
  } else {
    moverMotor2(0); // Freno seco del codo
    integralM2 = 0.0;
  }
}

#endif