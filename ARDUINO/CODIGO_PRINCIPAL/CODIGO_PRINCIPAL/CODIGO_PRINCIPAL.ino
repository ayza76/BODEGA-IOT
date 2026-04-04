#include "HX711.h" 
#include "DHT.h" 

// --- CONFIGURACIÓN DE PINES ---
const int HX711_DOUT = 11; // Pin de Datos Balanza
const int HX711_SCK  = 10; // Pin de Reloj Balanza
const int DHT_PIN = 7;     // Pin del Sensor Temp/Humedad
#define DHTTYPE DHT11      

// --- CALIBRACIÓN (¡LO QUE DEBES EDITAR!) ---
// Pon aquí el número que obtuviste en la calibración interactiva.
// Si no estás seguro, prueba con -98.9 o -100.0
const float CALIBRATION_FACTOR = -98.90; 

HX711 scale; 
DHT dht(DHT_PIN, DHTTYPE); 

void setup() {
  Serial.begin(115200); // Velocidad para Python
  Serial.println("--- INICIANDO SISTEMA IOT ---");

  dht.begin();
  
  // Iniciar Balanza
  scale.begin(HX711_DOUT, HX711_SCK); 
  scale.set_scale(CALIBRATION_FACTOR); // Aplica tu factor de calibración
  
  // AUTO-TARA (Poner a cero al iniciar)
  // IMPORTANTE: Encender el Arduino SIN peso sobre la balanza
  scale.tare(); 
  
  Serial.println("Sensores listos. Balanza en 0g.");
}

void loop() {
  // 1. LECTURA DE DATOS
  // Leemos el promedio de 5 lecturas para evitar saltos locos
  float peso = scale.get_units(5); 
  
  // Evitar pesos negativos pequeños por ruido (ej: -0.5g)
  if (peso < 0) {
    peso = 0.00;
  }

  float humedad = dht.readHumidity();
  float temperatura = dht.readTemperature();
    
  // Validar si el sensor DHT está conectado bien
  if (isnan(humedad) || isnan(temperatura)) {
    Serial.println("Error de lectura DHT11");
    delay(2000);
    return;
  }
  
  // 2. CONSTRUCCIÓN Y ENVÍO DEL JSON
  // El formato debe ser EXACTO para que Python lo entienda
  String json_data = "{\"peso\":";
  json_data += String(peso, 2);      // 2 decimales para peso
  json_data += ",\"temperatura\":";
  json_data += String(temperatura, 1); // 1 decimal para temp
  json_data += ",\"humedad\":";
  json_data += String(humedad, 1);     // 1 decimal para hum
  json_data += "}";

  // 3. ENVIAR POR USB
  Serial.println(json_data);

  // 4. ESPERA
  // Enviamos datos cada 5 segundos para no saturar la base de datos
  // (Puedes cambiarlo a 10000 para 10 segundos)
  delay(5000); 
}