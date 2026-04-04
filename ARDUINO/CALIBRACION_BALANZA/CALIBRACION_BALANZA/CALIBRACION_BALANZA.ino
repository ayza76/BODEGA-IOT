#include "HX711.h"

// PINES (Los que ya configuramos)
const int LOADCELL_DOUT_PIN = 11;
const int LOADCELL_SCK_PIN = 10;

HX711 scale;

// Valor inicial de calibración (Empezamos "a ciegas")
float calibration_factor = -100.0; 

void setup() {
  Serial.begin(115200);
  Serial.println("--- INICIANDO CALIBRACION INTERACTIVA ---");
  Serial.println("1. NO coloque nada en la balanza aun.");
  Serial.println("2. Espere a que se tare (poner a cero)...");

  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  scale.set_scale();
  scale.tare(); // Resetea la balanza a 0

  Serial.println("--- LISTO ---");
  Serial.println("3. Coloque ahora su PESO CONOCIDO (ej: celular, 1kg arroz) sobre la balanza.");
  Serial.println("4. Envie 'a', 's', 'd', 'f' para AUMENTAR el factor de calibracion.");
  Serial.println("5. Envie 'z', 'x', 'c', 'v' para DISMINUIR el factor de calibracion.");
  Serial.println("   (Las letras mas a la derecha hacen cambios mas grandes)");
  Serial.println("OBJETIVO: Que la lectura coincida con el peso real de su objeto.");
}

void loop() {
  scale.set_scale(calibration_factor); // Aplica el factor actual

  Serial.print("Leyendo: ");
  Serial.print(scale.get_units(), 1); // Muestra el peso con 1 decimal
  Serial.print(" g"); 
  Serial.print(" | Factor Actual (copia este valor al final): ");
  Serial.println(calibration_factor);

  if(Serial.available()) {
    char temp = Serial.read();
    
    // Ajuste fino
    if(temp == 'a') calibration_factor += 10;
    else if(temp == 'z') calibration_factor -= 10;
    
    // Ajuste medio
    else if(temp == 's') calibration_factor += 100;
    else if(temp == 'x') calibration_factor -= 100;
    
    // Ajuste grande
    else if(temp == 'd') calibration_factor += 1000;
    else if(temp == 'c') calibration_factor -= 1000;
    
    // Ajuste enorme
    else if(temp == 'f') calibration_factor += 10000;
    else if(temp == 'v') calibration_factor -= 10000;
  }
  
  delay(200); // Pequeña pausa para leer bien
}