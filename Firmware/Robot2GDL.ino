// Archivo principal: Robot2GDL.ino (ACTUALIZADO CON OFFSETS DE HOME)
#include "motores.h"
#include "encoders.h"
#include "control.h"
#include "menu.h"
#include "cinematica.h"

bool motoresHabilitados = false; 
EstadoMenu estadoActualMenu = MONSTRAR_MENU_PRINCIPAL;

float objetivoM1 = 0.0;
float objetivoM2 = 0.0;

// VARIABLES PARA GUARDAR AL HOME REAL
float offsetM1 = 0.0;
float offsetM2 = 0.0;

unsigned long tiempoAnteriorPrint = 0;

void gestionarMenu() {
  if (estadoActualMenu == MONSTRAR_MENU_PRINCIPAL) {
    imprimirMenuPrincipal();
    estadoActualMenu = ESPERANDO_OPCION;
  }

  if (Serial.available() > 0) {
    String entrada = Serial.readStringUntil('\n');
    entrada.trim();

    if (estadoActualMenu == ESPERANDO_OPCION) {
      if (entrada == "0") {
        motoresHabilitados = !motoresHabilitados;
        if (motoresHabilitados) {
          
          // Guardamos el valor actual del imán como el "Cero Real" (Home)
          offsetM1 = obtenerAnguloM1();
          offsetM2 = obtenerAnguloM2();
          
          // Metas iniciales para el PID
          objetivoM1 = 0.0; // 0 grados relativos al Home
          objetivoM2 = 0.0; // 0 grados relativos al Home
          
          Serial.println("\n[SISTEMA] CALIBRACIÓN EXITOSA. Motores posicionados en HOME (0°, 0°).");
          Serial.print("-> Offsets guardados: M1_Zero = "); Serial.print(offsetM1);
          Serial.print("° | M2_Zero = "); Serial.print(offsetM2); Serial.println("°");
        } else {
          moverMotor1(0);
          moverMotor2(0);
          Serial.println("\n[SISTEMA] MOTORES APAGADOS (Ejes Libres - Modo Seguro).");
        }
        estadoActualMenu = MONSTRAR_MENU_PRINCIPAL;
        return;
      }
      
      // ... (El resto de las opciones 1, 2, 3, 4, 5, 6 se mantienen exactamente igual)
      if (entrada == "1") {
        if (!motoresHabilitados) { Serial.println("\n[ERROR] Debe encender los motores primero (Opción 0)."); estadoActualMenu = MONSTRAR_MENU_PRINCIPAL; return; }
        Serial.print("\n[MODO M1] Ingrese ángulo (0-360) o 'b' para VOLVER: ");
        estadoActualMenu = MODO_MOTOR_1;
      } else if (entrada == "2") {
        if (!motoresHabilitados) { Serial.println("\n[ERROR] Debe encender los motores primero (Opción 0)."); estadoActualMenu = MONSTRAR_MENU_PRINCIPAL; return; }
        Serial.print("\n[MODO M2] Ingrese ángulo (0-360) o 'b' para VOLVER: ");
        estadoActualMenu = MODO_MOTOR_2;
      } else if (entrada == "3") {
        if (!motoresHabilitados) { Serial.println("\n[ERROR] Debe encender los motores primero (Opción 0)."); estadoActualMenu = MONSTRAR_MENU_PRINCIPAL; return; }
        Serial.print("\n[MODO DUAL] Ingrese ángulos (M1,M2) o 'b' para VOLVER: ");
        estadoActualMenu = MODO_AMBOS_MOTORES;
      } else if (entrada == "4") {
        if (!motoresHabilitados) { Serial.println("\n[ERROR] Debe encender los motores primero (Opción 0)."); estadoActualMenu = MONSTRAR_MENU_PRINCIPAL; return; }
        Serial.print("\n[MODO COORDENADAS] Ingrese posición destino 'X,Y' en mm (ej: 100.5,20.0) o 'b' para VOLVER: ");
        estadoActualMenu = MODO_COORDENADAS_XY;
      } else if (entrada == "5") {
        Serial.println("\n--- Monitoreo Activo --- (Presione ENTER para salir y volver al menú)");
        estadoActualMenu = MODO_TELEMETRIA;
      } else if (entrada == "6") {
        motoresHabilitados = false;
        moverMotor1(0);
        moverMotor2(0);
        Serial.println("\n!!! EMERGENCY STOP ACTIVATED !!! Lazo PID desactivado.");
        estadoActualMenu = MONSTRAR_MENU_PRINCIPAL;
      } else {
        Serial.println("[ERROR] Opción inválida.");
        estadoActualMenu = MONSTRAR_MENU_PRINCIPAL;
      }
      return; 
    }

    if (entrada == "b" || entrada == "B") {
      Serial.println("\n[INFO] Operación cancelada.");
      estadoActualMenu = MONSTRAR_MENU_PRINCIPAL;
      return;
    }

    if (estadoActualMenu == MODO_MOTOR_1) {
      float angulo = entrada.toFloat();
      if (angulo >= -180.0 && angulo <= 180.0) { objetivoM1 = angulo; Serial.println("\n-> Meta Motor 1 actualizada."); }
      else { Serial.println("\n[ERROR] Ángulo fuera de rango."); }
      estadoActualMenu = MONSTRAR_MENU_PRINCIPAL;
    }

    if (estadoActualMenu == MODO_MOTOR_2) {
      float angulo = entrada.toFloat();
      if (angulo >= -180.0 && angulo <= 180.0) { objetivoM2 = angulo; Serial.println("\n-> Meta Motor 2 actualizada."); }
      else { Serial.println("\n[ERROR] Ángulo fuera de rango."); }
      estadoActualMenu = MONSTRAR_MENU_PRINCIPAL;
    }

    if (estadoActualMenu == MODO_AMBOS_MOTORES) {
      int posicionComa = entrada.indexOf(',');
      if (posicionComa != -1) {
        float nM1 = entrada.substring(0, posicionComa).toFloat();
        float nM2 = entrada.substring(posicionComa + 1).toFloat();
        objetivoM1 = nM1; objetivoM2 = nM2; Serial.println("\n-> Metas actualizadas.");
      } else { Serial.println("\n[ERROR] Formato incorrecto. Use: M1,M2"); }
      estadoActualMenu = MONSTRAR_MENU_PRINCIPAL;
    }

    if (estadoActualMenu == MODO_COORDENADAS_XY) {
      int posicionComa = entrada.indexOf(',');
      if (posicionComa != -1) {
        float targetX = entrada.substring(0, posicionComa).toFloat();
        float targetY = entrada.substring(posicionComa + 1).toFloat();
        
        AngulosRobot solucion = calcularCinematicaInversa(targetX, targetY);
        
        if (solucion.valido) {
          objetivoM1 = solucion.theta1;
          objetivoM2 = solucion.theta2;
          Serial.print("\n[CINEMÁTICA] Mapeo exitoso -> M1: "); Serial.print(objetivoM1, 2);
          Serial.print("° | M2: "); Serial.print(objetivoM2, 2); Serial.println("°");
        } else {
          Serial.println("\n[ERROR] Coordenada inalcanzable. Supera los límites físicos.");
        }
      } else {
        Serial.println("\n[ERROR] Formato incorrecto. Use 'X,Y'");
      }
      estadoActualMenu = MONSTRAR_MENU_PRINCIPAL;
    }

    if (estadoActualMenu == MODO_TELEMETRIA) {
      estadoActualMenu = MONSTRAR_MENU_PRINCIPAL;
    }
  }
}

void setup() {
  Serial.begin(115200);
  configurarMotores();
  configurarEncoders();
  moverMotor1(0);
  moverMotor2(0);
  motoresHabilitados = false;
}

void loop() {
  gestionarMenu();
  if (motoresHabilitados) {
    // Le pasamos al PID la meta sumada a su offset real para que sepa a dónde ir físicamente
    actualizarControlPID(objetivoM1 + offsetM1, objetivoM2 + offsetM2);
  } else {
    moverMotor1(0);
    moverMotor2(0);
    tiempoAnteriorPID = micros();
  }

  if (estadoActualMenu == MODO_TELEMETRIA) {
    unsigned long tiempoActual = millis();
    if (tiempoActual - tiempoAnteriorPrint >= 150) { 
      tiempoAnteriorPrint = tiempoActual;
      
      // Mostramos la telemetría ya corregida con el offset relativo al Home
      float posM1 = obtenerAnguloM1() - offsetM1;
      float posM2 = obtenerAnguloM2() - offsetM2;
      
      // Ajuste visual estricto para que la pantalla no muestre negativos si pasa los 180°
      while (posM1 < -180.0) posM1 += 360.0;
      while (posM1 > 180.0)  posM1 -= 360.0;
      while (posM2 < -180.0) posM2 += 360.0;
      while (posM2 > 180.0)  posM2 -= 360.0;
      
      // Si el codo trabaja en el cuadrante superior positivo de la cinemática, forzamos visualización positiva
      if (posM2 < 0 && objetivoM2 > 0) posM2 += 360.0;

      Serial.print("M1 Meta: "); Serial.print(objetivoM1, 1);
      Serial.print("° Real: "); Serial.print(posM1, 1);
      Serial.print("° | M2 Meta: "); Serial.print(objetivoM2, 1);
      Serial.print("° Real: "); Serial.print(posM2, 1);
      Serial.print("° | POTENCIA: "); Serial.println(motoresHabilitados ? "CON TORQUE" : "DESHABILITADA");
    }
  }
}