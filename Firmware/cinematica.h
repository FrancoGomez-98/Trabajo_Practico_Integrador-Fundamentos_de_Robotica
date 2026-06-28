// Pestaña: cinematica.h (CORREGIDA PARA FLEXIÓN ADELANTE)
#ifndef CINEMATICA_H
#define CINEMATICA_H

#include <Arduino.h>
#include <math.h>

const float L1 = 80.0;
const float L2 = 60.0;

struct AngulosRobot {
  float theta1; 
  float theta2; 
  bool valido;  
};

AngulosRobot calcularCinematicaInversa(float x, float y) {
  AngulosRobot resultado;
  resultado.valido = false;

  float d2 = (x * x) + (y * y);
  float d = sqrt(d2);

  if (d > (L1 + L2) || d < abs(L1 - L2) || d == 0.0) {
    return resultado; 
  }

  float cos_beta = (L1*L1 + L2*L2 - d2) / (2.0 * L1 * L2);
  cos_beta = constrain(cos_beta, -1.0, 1.0);
  float beta = acos(cos_beta); 

  float q2_relativo = M_PI - beta; 

  float alfa1 = atan2(y, x);
  float cos_alfa2 = (L1*L1 + d2 - L2*L2) / (2.0 * L1 * d);
  cos_alfa2 = constrain(cos_alfa2, -1.0, 1.0);
  float alfa2 = acos(cos_alfa2);

  float q1 = alfa1 + alfa2; 

  float t1_deg = q1 * 180.0 / M_PI;
  
  // Restamos el ángulo relativo en lugar de sumarlo
  float t2_deg = t1_deg - (q2_relativo * 180.0 / M_PI);

  while (t1_deg < -180.0) t1_deg += 360.0;
  while (t1_deg > 180.0)  t1_deg -= 360.0;
  while (t2_deg < -180.0) t2_deg += 360.0;
  while (t2_deg > 180.0)  t2_deg -= 360.0;

  resultado.theta1 = t1_deg;
  resultado.theta2 = t2_deg;
  resultado.valido = true;

  return resultado;
}

#endif