// Pestaña: menu.h
#ifndef MENU_H
#define MENU_H

#include <Arduino.h>

enum EstadoMenu {
  MONSTRAR_MENU_PRINCIPAL,
  ESPERANDO_OPCION,
  MODO_MOTOR_1,
  MODO_MOTOR_2,
  MODO_AMBOS_MOTORES,
  MODO_COORDENADAS_XY,
  MODO_TELEMETRIA
};

extern bool motoresHabilitados; 
extern EstadoMenu estadoActualMenu;

void imprimirMenuPrincipal() {
  Serial.println("\n==================================================");
  Serial.println("        TEACH PENDANT CONSOLE - ROBOT 2 GDL       ");
  Serial.println("==================================================");
  
  if (motoresHabilitados) {
    Serial.println(" [0] APAGAR MOTORES (Deshabilitar Potencia - Ejes Libres)");
  } else {
    Serial.println(" [0] ENCENDER MOTORES (Habilitar Potencia - Torque Activo)");
  }
  
  Serial.println(" [1] Mover MOTOR 1 (Eslabón Base)");
  Serial.println(" [2] Mover MOTOR 2 (Eslabón Correa)");
  Serial.println(" [3] Mover Ambos Motores Simultáneamente (M1,M2)");
  Serial.println(" [4] INGRESAR COORDENADAS CARTESIANAS (X,Y en mm)");
  Serial.println(" [5] Monitorear Telemetría en Tiempo Real");
  Serial.println(" [6] PARADA DE EMERGENCIA (E-STOP)");
  Serial.println("==================================================");
  
  if (!motoresHabilitados) {
    Serial.println("--> [ATENCIÓN] Potencia deshabilitada. Encienda antes de operar.");
  }
  Serial.print(">> Seleccione una opción: ");
}

#endif