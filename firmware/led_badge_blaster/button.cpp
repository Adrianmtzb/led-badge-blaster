#include "button.h"
#include "config.h"
#include "app.h"
#include "net_portal.h"
#include "status_led.h"

// Clic simple: envía y avanza. Doble clic: cambia de categoría.
// Pulsación larga: olvida el WiFi y reinicia en modo portal.

static bool btnRaw     = false;  // último nivel muestreado
static bool btnPressed = false;  // nivel ya rebotado
static uint32_t btnEdgeMs = 0;
static uint32_t pressStartMs = 0;
static bool longFired = false;

static uint8_t pendingClicks = 0;
static uint32_t firstClickMs = 0;

static bool rawButtonPressed() { return digitalRead(PIN_BUTTON) == LOW; }

static void onSingleClick() {
  appSendCurrent(IR_REPEATS);
  appNext();
}

static void onDoubleClick() { appCycleMode(); }

static void onLongPress() {
  Serial.println("[btn] Pulsacion larga: olvidando WiFi");
  ledSetBase(255, 0, 255);
  netForget();
}

void buttonBegin() {
  // GPIO39 es solo de entrada y no tiene resistencias internas; el pull-up lo
  // pone la propia placa.
  pinMode(PIN_BUTTON, INPUT);
}

void buttonUpdate() {
  const uint32_t now = millis();
  const bool raw = rawButtonPressed();

  if (raw != btnRaw) {
    btnRaw = raw;
    btnEdgeMs = now;
  }

  // Los clics se cuentan en el flanco de pulsación ya rebotado, así mantener
  // el botón apretado nunca genera un clic simple espurio.
  if ((now - btnEdgeMs) >= DEBOUNCE_MS && btnPressed != btnRaw) {
    btnPressed = btnRaw;

    if (btnPressed) {
      pressStartMs = now;
      longFired = false;

      if (pendingClicks == 0) {
        pendingClicks = 1;
        firstClickMs = now;
      } else {
        pendingClicks = 0;
        onDoubleClick();
      }
    }
  }

  if (btnPressed && !longFired && (now - pressStartMs) >= LONGPRESS_MS) {
    longFired = true;
    pendingClicks = 0;
    onLongPress();
  }

  // El clic pendiente se convierte en clic simple al agotarse la ventana de
  // doble clic, pero solo con el botón ya soltado.
  if (pendingClicks == 1 && !btnPressed && (now - firstClickMs) > DOUBLECLICK_MS) {
    pendingClicks = 0;
    onSingleClick();
  }
}
