#include "app.h"
#include "config.h"
#include "ir_sender.h"
#include "status_led.h"

void appBegin() {
  catalogBegin();
  irBegin();
  appRefreshLed();
}

// El LED de reposo muestra el color del efecto seleccionado: es la pista más
// directa de qué va a salir por el emisor al pulsar el botón.
void appRefreshLed() {
  uint16_t absIdx = 0;
  if (!catalogCurrentIndex(absIdx)) {
    ledSetBase(0, 0, 0);
    return;
  }

  const uint32_t rgb = COMMANDS[absIdx].rgb;
  ledSetBase((rgb >> 16) & 0xFF, (rgb >> 8) & 0xFF, rgb & 0xFF);
}

bool appSendCurrent(uint8_t extra) {
  uint16_t absIdx = 0;
  if (!catalogCurrentIndex(absIdx)) {
    Serial.printf("ERR: la categoria %s esta vacia\n", modeName(catalogMode()));
    return false;
  }

  const Command& cmd = COMMANDS[absIdx];
  Serial.printf("SEND mode=%s pos=%u abs=%u id=%s name=%s x%u\n",
                modeName(catalogMode()), catalogPosition(), absIdx, cmd.id, cmd.name,
                (unsigned)(extra + 1));

  if (!irSendPronto(cmd.pronto, extra)) {
    Serial.println("ERR: trama PRONTO invalida");
    return false;
  }

  // Blanco, no rojo: el reposo ya es el color del efecto y un destello rojo
  // se perdería encima de los rojos y naranjas del catálogo.
  ledFlash(255, 255, 255, FLASH_MS);
  return true;
}

// La trama viene de fuera del catálogo, así que no hay color asociado y la
// selección no se toca: el LED vuelve solo al color del efecto elegido.
bool appSendRaw(const char* pronto, uint8_t extra) {
  Serial.printf("SEND raw len=%u x%u\n", (unsigned)strlen(pronto), (unsigned)(extra + 1));

  if (!irSendPronto(pronto, extra)) {
    Serial.println("ERR: trama PRONTO invalida");
    return false;
  }

  ledFlash(255, 255, 255, FLASH_MS);
  return true;
}

bool appSendAbsolute(uint16_t absIdx, uint8_t extra) {
  if (!catalogSelect(absIdx)) return false;
  appRefreshLed();
  return appSendCurrent(extra);
}

void appSetMode(Mode m) {
  catalogSetMode(m);
  appRefreshLed();
  appPrintStatus();
}

void appCycleMode() {
  catalogCycleMode();
  appRefreshLed();
  appPrintStatus();
}

void appSetPosition(uint16_t pos) {
  catalogSetPosition(pos);
  appRefreshLed();
  appPrintStatus();
}

void appNext() {
  catalogNext();
  appRefreshLed();
}

void appPrintStatus() {
  uint16_t absIdx = 0;
  if (!catalogCurrentIndex(absIdx)) {
    Serial.printf("STATUS mode=%s (vacia)\n", modeName(catalogMode()));
    return;
  }

  Serial.printf("STATUS mode=%s pos=%u/%u abs=%u id=%s name=%s #%06X\n",
                modeName(catalogMode()),
                catalogPosition(),
                (unsigned)(catalogCurrentCount() - 1),
                absIdx,
                COMMANDS[absIdx].id,
                COMMANDS[absIdx].name,
                (unsigned)COMMANDS[absIdx].rgb);
}
