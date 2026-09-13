#pragma once
#include <Arduino.h>

// Catálogo de comandos IR: datos estáticos + navegación por categoría.
//
// Los datos (COMMANDS, CATEGORIES y las tramas de pronto_data.h) los genera
// scripts/gen_catalog.py a partir de data/presets/*.json. No se editan a mano.

enum class Mode : uint8_t {
  CAPTURAS = 0,
  BASICOS,
  ENVOLVENTES,
  MULTITUD,
  ESCENAS,
  GRUPOS,
};
static const uint8_t MODE_COUNT = 6;

struct Command {
  const char* id;      // identificador estable: CAP_00, BAS_03…
  const char* name;    // nombre legible en español: "Rojo"
  const char* nameEn;  // el mismo, en inglés: "Red"
  const char* note;    // detalle corto, puede ser cadena vacía
  const char* noteEn;  // el mismo, en inglés
  uint32_t    rgb;     // color ya cuantizado por el badge, 0xRRGGBB
  const char* pronto;  // PRONTO_HEX
  Mode        mode;
};

struct CategoryInfo {
  const char* key;    // nombre del modo en la API: "CAPTURAS"
  const char* label;    // nombre para mostrar
  const char* labelEn;  // el mismo, en inglés
  const char* blurb;    // una línea de contexto
  const char* blurbEn;  // la misma, en inglés
};

extern const Command COMMANDS[];
extern const uint16_t COMMAND_COUNT;
extern const CategoryInfo CATEGORIES[MODE_COUNT];

const char* modeName(Mode m);
bool modeFromName(const char* name, Mode& out);

void catalogBegin();

Mode     catalogMode();
uint16_t catalogPosition();
uint16_t catalogCount(Mode m);
uint16_t catalogCurrentCount();

// Devuelve false si la categoría actual no tiene comandos.
bool catalogCurrentIndex(uint16_t& outAbsIdx);

void catalogSetMode(Mode m);   // reinicia la posición a 0
void catalogCycleMode();
void catalogSetPosition(uint16_t pos);
void catalogNext();

// Fija categoría y posición a partir de un índice absoluto de COMMANDS[].
bool catalogSelect(uint16_t absIdx);
