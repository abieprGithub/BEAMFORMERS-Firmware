#include <Arduino.h>
#include "USB.h"
#include "USBCDC.h"

USBCDC USBSerial;          // CDC/ACM instance on the USB-OTG peripheral

static String uartBuffer;  // accumulates chars from UART0

void setup() {
  // UART0 — used for monitoring / receiving commands
  Serial.begin(115200);

  // USB CDC — manually started (ARDUINO_USB_CDC_ON_BOOT=0)
  USBSerial.begin();
  USB.begin();

  Serial.println("[boot] USB CDC initialised — prefix a line with send: to forward to USB");
}

void loop() {
  // --- UART0 → USB CDC forwarding (prefix: "send:") ---
  while (Serial.available()) {
    char c = Serial.read();

    if (c == '\n' || c == '\r') {
      if (uartBuffer.length() > 0) {
        if (uartBuffer.startsWith("send:")) {
          String payload = uartBuffer.substring(5);   // strip the prefix
          USBSerial.println(payload);
          Serial.println("[fwd] → USB: " + payload);
        } else {
          Serial.println("[ignored] no 'send:' prefix");
        }
        uartBuffer = "";
      }
    } else {
      uartBuffer += c;
    }
  }

  // --- USB CDC → UART0 echo (optional, handy for debugging) ---
  while (USBSerial.available()) {
    char c = USBSerial.read();
    Serial.write(c);
  }
}