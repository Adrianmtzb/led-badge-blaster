#pragma once
#include "catalog.h"

// Capa que une catálogo, emisor IR y LED. La usan tanto el botón como la API.

void appBegin();

bool appSendCurrent();
bool appSendAbsolute(uint16_t absIdx);

// Emite una trama PRONTO que no está en el catálogo. No cambia la selección
// actual ni el color de reposo del LED.
bool appSendRaw(const char* pronto);

void appSetMode(Mode m);
void appCycleMode();
void appSetPosition(uint16_t pos);
void appNext();

void appRefreshLed();
void appPrintStatus();
