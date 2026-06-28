// Pestaña: encoders.h
#ifndef ENCODERS_H
#define ENCODERS_H

#include <Wire.h>
#include <AS5600.h>

const int SDA_1 = 21; const int SCL_1 = 22;
const int SDA_2 = 18; const int SCL_2 = 19;

AS5600 encoderM1(&Wire);
AS5600 encoderM2(&Wire1);

void configurarEncoders() {
  Wire.begin(SDA_1, SCL_1);
  Wire1.begin(SDA_2, SCL_2, 400000);

  encoderM1.begin();
  encoderM2.begin();

  if (encoderM1.isConnected()) Serial.println("-> Encoder Motor 1: DETECTADO");
  else Serial.println("-> Encoder Motor 1: ERROR");

  if (encoderM2.isConnected()) Serial.println("-> Encoder Motor 2: DETECTADO");
  else Serial.println("-> Encoder Motor 2: ERROR");
}

float obtenerAnguloM1() {
  int rawTicks = encoderM1.readAngle();
  float anguloLeido = (rawTicks * 360.0) / 4096.0;
  float anguloCorregido = 360.0 - anguloLeido; // Mapeo antihorario correcto
  if (anguloCorregido >= 360.0) anguloCorregido -= 360.0; 
  return anguloCorregido;
}

// CORRECCIÓN PARA EL MOTOR 2 (ESPEJADO)
float obtenerAnguloM2() {
  int rawTicks = encoderM2.readAngle();
  // Eliminamos el "360.0 -" para que el sentido de conteo se invierta por completo
  float anguloLeido = (rawTicks * 360.0) / 4096.0; 
  float anguloCorregido = anguloLeido; 
  if (anguloCorregido >= 360.0) anguloCorregido -= 360.0;
  return anguloCorregido;
}

#endif