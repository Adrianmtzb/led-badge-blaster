#include "console.h"
#include "app.h"
#include "config.h"
#include "catalog.h"
#include "net_portal.h"
#include "ir_sender.h"

// LINE_MAX es una macro de limits.h en este toolchain, de ahí el nombre propio.
// Da para 'raw ' más una trama PRONTO completa, que es la línea más larga.
static const uint16_t kLineMax = PRONTO_MAX_CHARS + 8;

static String readLine() {
  static String buf;
  while (Serial.available()) {
    const char c = (char)Serial.read();
    if (c == '\n') {
      String out = buf;
      buf = "";
      out.trim();
      return out;
    }
    if (buf.length() < kLineMax) buf += c;  // descarta el resto de líneas largas
  }
  return "";
}

static void handleLine(const String& line) {
  String lower = line;
  lower.toLowerCase();

  if (lower == "help") {
    Serial.println("Comandos:");
    Serial.println("  status              estado actual");
    Serial.println("  cats                lista de categorias");
    Serial.println("  mode <categoria>    ver 'cats'");
    Serial.println("  pos <n>             posicion dentro de la categoria");
    Serial.println("  send [n]            emite el actual, o el indice absoluto n");
    Serial.println("  raw <pronto hex>    emite una trama suelta, sin tocar la seleccion");
    Serial.println("  next                avanza posicion");
    Serial.println("  list                comandos de la categoria actual");
    Serial.println("  net                 estado de red");
    Serial.println("  forget              borra el WiFi y reinicia en portal");
    return;
  }

  if (lower == "status") { appPrintStatus(); return; }

  if (lower == "cats") {
    for (uint8_t i = 0; i < MODE_COUNT; i++) {
      Serial.printf("  %-12s %2u comandos  %s\n",
                    CATEGORIES[i].key, catalogCount((Mode)i), CATEGORIES[i].blurb);
    }
    return;
  }

  if (lower == "list") {
    const Mode m = catalogMode();
    for (uint16_t i = 0; i < COMMAND_COUNT; i++) {
      if (COMMANDS[i].mode != m) continue;
      Serial.printf("  %3u %-8s #%06X  %s%s%s\n",
                    i, COMMANDS[i].id, (unsigned)COMMANDS[i].rgb, COMMANDS[i].name,
                    COMMANDS[i].note[0] ? " — " : "", COMMANDS[i].note);
    }
    return;
  }

  if (lower == "net") {
    Serial.printf("NET mode=%s ssid=%s ip=%s\n",
                  netMode() == NetMode::STA ? "STA" : "AP",
                  netSsid().c_str(), netIp().c_str());
    return;
  }

  if (lower == "forget") { netForget(); return; }
  if (lower == "next")   { appNext(); appPrintStatus(); return; }
  if (lower == "send")   { appSendCurrent(IR_REPEATS); return; }

  if (lower.startsWith("mode ")) {
    String m = lower.substring(5); m.trim();
    Mode parsed;
    if (modeFromName(m.c_str(), parsed)) appSetMode(parsed);
    else Serial.println("ERR: categoria desconocida (escribe 'cats')");
    return;
  }

  if (lower.startsWith("pos ")) {
    String n = lower.substring(4); n.trim();
    const long p = n.toInt();
    if (p < 0 || catalogCurrentCount() == 0) { Serial.println("ERR: posicion invalida"); return; }
    appSetPosition((uint16_t)(p % catalogCurrentCount()));
    return;
  }

  if (lower.startsWith("raw ")) {
    String hex = lower.substring(4); hex.trim();
    if (!appSendRaw(hex.c_str(), IR_REPEATS)) Serial.println("ERR: trama PRONTO invalida");
    return;
  }

  if (lower.startsWith("send ")) {
    String n = lower.substring(5); n.trim();
    const long idx = n.toInt();
    if (idx < 0 || idx >= (long)COMMAND_COUNT) { Serial.println("ERR: indice fuera de rango"); return; }
    appSendAbsolute((uint16_t)idx, IR_REPEATS);
    return;
  }

  Serial.println("ERR: comando desconocido (escribe 'help')");
}

void consoleUpdate() {
  const String line = readLine();
  if (line.length()) handleLine(line);
}
