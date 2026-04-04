import serial
import json
import mysql.connector
import time
import sys

# --- CONFIGURACIÓN ---
PUERTO_COM = 'COM3'  # <--- ¡VERIFICA QUE ESTE SEA TU PUERTO!
BAUD_RATE = 115200   
DB_CONFIG = {
    'host': 'localhost',
    'user': 'root',      
    'password': '',      
    'database': 'bodega_iot' 
}

def conectar_bd():
    try:
        conn = mysql.connector.connect(**DB_CONFIG)
        return conn
    except mysql.connector.Error as err:
        # Solo imprimimos si es un error grave de base de datos
        print(f"Error crítico DB: {err}")
        return None

def main():
    # 1. Conectar al Arduino
    try:
        arduino = serial.Serial(PUERTO_COM, BAUD_RATE, timeout=1)
        print(f"✅ Conectado al Arduino en {PUERTO_COM}")
        print("⏳ Esperando datos...")
        time.sleep(2) 
    except serial.SerialException:
        print(f"❌ No se detecta el Arduino en {PUERTO_COM}.")
        sys.exit()

    # 2. Bucle infinito
    while True:
        try:
            if arduino.in_waiting > 0:
                # Leer línea
                linea = arduino.readline().decode('utf-8').strip()
                
                # Si la línea está vacía, pasamos
                if not linea:
                    continue

                # Intentar convertir a JSON
                try:
                    datos = json.loads(linea)
                    
                    # Si llegamos aquí, ES UN DATO VÁLIDO
                    peso = datos['peso']
                    temp = datos['temperatura']
                    hum = datos['humedad']

                    print(f"📥 Datos recibidos: Peso={peso} | Temp={temp} | Hum={hum}")

                    # 3. Guardar en Base de Datos
                    conn = conectar_bd()
                    if conn:
                        cursor = conn.cursor()
                        sql = "INSERT INTO datos_sensores (peso, temperatura, humedad) VALUES (%s, %s, %s)"
                        val = (peso, temp, hum)
                        cursor.execute(sql, val)
                        conn.commit()
                        print("   💾 ¡Guardado en BD!")
                        cursor.close()
                        conn.close()

                except json.JSONDecodeError:
                    # AQUÍ ESTÁ EL CAMBIO:
                    # Si la línea no es JSON (es texto de bienvenida), NO hacemos nada.
                    # No imprimimos error, simplemente 'pass' (pasar)
                    pass

                except Exception as e:
                    print(f"   ⚠️ Error procesando datos: {e}")

        except KeyboardInterrupt:
            print("\n🛑 Programa detenido.")
            arduino.close()
            break
        except Exception as e:
            print(f"Error general: {e}")
            break

if __name__ == "__main__":
    main()