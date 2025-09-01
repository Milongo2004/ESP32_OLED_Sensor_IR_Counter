#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET   -1

#define SDA_PIN 5
#define SCL_PIN 4
#define OLED_ADDR 0x3C  

#define SENSOR_PIN 26  // Pin de entrada del sensor infrarrojo

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

int counter = 1;           // Contador inicial
bool lastSensorState = LOW; // Para detectar flancos
unsigned long ultimaDeteccion = 0;
const unsigned long tiempoBloqueo = 500; // 500 ms = medio segundo




void centerPrint(const String &text, uint8_t textSize) {
  int16_t x1, y1;
  uint16_t w, h;
  display.setTextSize(textSize);
  display.getTextBounds(text, 0, 0, &x1, &y1, &w, &h);
  int16_t x = (SCREEN_WIDTH  - w) / 2;
  int16_t y = (SCREEN_HEIGHT - h) / 2;
  display.setCursor(x, y);
  display.print(text);
}

void updateDisplay() {
  display.clearDisplay();
  centerPrint(String(counter), 4); // Tamaño 4 para hasta 3 dígitos
  display.display();
}

void setup() {
  Serial.begin(115200);
  Wire.begin(SDA_PIN, SCL_PIN);

  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
    Serial.println("OLED no encontrado en 0x3C, probando 0x3D...");
    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3D)) {
      Serial.println("No se pudo iniciar el OLED. Revisa SDA/SCL y la dirección.");
      while (true) delay(10);
    }
  }

  pinMode(SENSOR_PIN, INPUT); // Configuramos el pin del sensor como entrada

  display.setTextColor(SSD1306_WHITE);
  updateDisplay();
}

void loop() {
  bool sensorState = digitalRead(SENSOR_PIN);

  // Detectar flanco de LOW -> HIGH (señal de conteo)
  if (sensorState == HIGH && lastSensorState == LOW && millis() - ultimaDeteccion > tiempoBloqueo) {
    counter++;
    ultimaDeteccion = millis();
    if (counter > 999) counter = 1; // Reiniciar al llegar a 999
    updateDisplay();
    Serial.println(counter);
    //delay(200); // Pequeño debounce para evitar múltiples conteos
  }

  lastSensorState = sensorState;
}
