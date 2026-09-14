#pragma once
#include <Arduino.h>

// Palabras que caben en el buffer PRONTO. Se expone para que la API pueda
// rechazar una trama demasiado larga sin llegar a copiarla.
static const uint16_t PRONTO_MAX_WORDS = 64;

// Cota de texto para esas palabras: 4 dígitos y un separador cada una.
static const size_t PRONTO_MAX_CHARS = PRONTO_MAX_WORDS * 5;

void irBegin();

// Emite la trama 1 + extra veces. Devuelve false si está malformada, en cuyo
// caso no se emite nada.
bool irSendPronto(const char* pronto, uint8_t extra);
