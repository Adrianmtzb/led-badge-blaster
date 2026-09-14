#pragma once
#include "catalog.h"

// Capa que une catálogo, emisor IR y LED. La usan tanto el botón como la API.

void appBegin();

// 'extra' son repeticiones adicionales de la trama; IR_REPEATS es el valor
// razonable por defecto (ver config.h: el badge se duerme y hay que despertarlo).
bool appSendCurrent(uint8_t extra);
bool appSendAbsolute(uint16_t absIdx, uint8_t extra);

// Emite una trama PRONTO que no está en el catálogo. No cambia la selección
// actual ni el color de reposo del LED.
bool appSendRaw(const char* pronto, uint8_t extra);

void appSetMode(Mode m);
void appCycleMode();
void appSetPosition(uint16_t pos);
void appNext();

void appRefreshLed();
void appPrintStatus();
