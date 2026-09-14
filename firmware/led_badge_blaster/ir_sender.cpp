#include "ir_sender.h"
#include "config.h"

#include <IRremoteESP8266.h>
#include <IRsend.h>

static IRsend irsend(PIN_IRLED);

// PRONTO_MAX_WORDS vive en ir_sender.h; la trama más larga del catálogo usa 46.
static uint16_t prontoBuf[PRONTO_MAX_WORDS];

static bool isHexChar(char c) {
  return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F');
}

static int hexVal(char c) {
  if (c >= '0' && c <= '9') return c - '0';
  if (c >= 'a' && c <= 'f') return 10 + (c - 'a');
  return 10 + (c - 'A');
}

// Lee una palabra PRONTO (1..4 dígitos hex) y avanza p.
static bool parseHexToken(const char*& p, uint16_t& out) {
  uint32_t val = 0;
  int digits = 0;

  while (isHexChar(*p)) {
    if (digits == 4) return false;  // las palabras PRONTO son de 4 dígitos
    val = (val << 4) | (uint32_t)hexVal(*p);
    digits++;
    p++;
  }

  if (digits == 0) return false;
  out = (uint16_t)val;
  return true;
}

// Rellena prontoBuf y devuelve el número de palabras, o 0 si hay basura.
static uint16_t prontoStringToWords(const char* pronto) {
  const char* p = pronto;
  uint16_t count = 0;

  for (;;) {
    while (*p == ' ' || *p == '\t' || *p == '\r' || *p == '\n') p++;
    if (!*p) break;

    if (count >= PRONTO_MAX_WORDS) return 0;
    if (!parseHexToken(p, prontoBuf[count])) return 0;
    count++;
  }

  return count;
}

void irBegin() {
  irsend.begin();
}

bool irSendPronto(const char* pronto, uint8_t extra) {
  if (!pronto) return false;

  const uint16_t len = prontoStringToWords(pronto);
  if (len < 4) return false;

  // El parámetro 'repeat' de sendPronto solo repite la segunda secuencia del
  // PRONTO, y las tramas del badge la llevan vacía: ahí no haría nada. La
  // repetición tiene que ser el bucle. Cada trama acaba en su propio silencio
  // (la última palabra), así que los paquetes ya salen separados.
  if (extra > IR_REPEATS_MAX) extra = IR_REPEATS_MAX;
  for (uint8_t i = 0; i <= extra; i++)
    irsend.sendPronto(prontoBuf, len, kNoRepeat);

  return true;
}
