#include "web_api.h"
#include "config.h"
#include "app.h"
#include "catalog.h"
#include "ir_sender.h"
#include "net_portal.h"
#include "web_ui.h"

#include <WiFi.h>
#include <WebServer.h>

static WebServer server(80);

// Escapa lo que venga del exterior (nombres de red) antes de meterlo en JSON.
static String jsonEscape(const String& s) {
  String out;
  out.reserve(s.length() + 8);
  for (size_t i = 0; i < s.length(); i++) {
    const char c = s[i];
    switch (c) {
      case '"':  out += "\\\""; break;
      case '\\': out += "\\\\"; break;
      case '\n': out += "\\n";  break;
      case '\r': out += "\\r";  break;
      case '\t': out += "\\t";  break;
      default:
        if ((uint8_t)c < 0x20) {
          char b[7];
          snprintf(b, sizeof(b), "\\u%04x", c);
          out += b;
        } else {
          out += c;
        }
    }
  }
  return out;
}

// Un formulario de otra web puede hacer POST a esta IP sin poder leer la
// respuesta. Exigir una cabecera personalizada obliga a un preflight CORS que
// el dispositivo no responde, así que corta ese CSRF.
static bool sameOrigin() {
  if (server.hasHeader("X-Requested-With")) return true;
  server.send(403, "application/json", "{\"error\":\"forbidden\"}");
  return false;
}

static void handleRoot() {
  server.send_P(200, "text/html; charset=utf-8", INDEX_HTML);
}

// Un solo GET con todo lo que la página necesita para pintarse: las categorías
// y los comandos con su color ya cuantizado.
static void handleCommands() {
  char hex[8];

  // ~100 B por comando. Reservar de una vez evita que String crezca a
  // realloc por realloc y fragmente el heap con el catálogo entero.
  String out;
  out.reserve(128 * COMMAND_COUNT);
  out += "{\"categories\":[";
  for (uint8_t m = 0; m < MODE_COUNT; m++) {
    if (m) out += ',';
    out += "{\"key\":\"";
    out += CATEGORIES[m].key;
    out += "\",\"label\":\"";
    out += jsonEscape(CATEGORIES[m].label);
    out += "\",\"blurb\":\"";
    out += jsonEscape(CATEGORIES[m].blurb);
    out += "\",\"count\":";
    out += catalogCount((Mode)m);
    out += '}';
  }

  out += "],\"commands\":[";
  for (uint16_t i = 0; i < COMMAND_COUNT; i++) {
    if (i) out += ',';
    snprintf(hex, sizeof(hex), "#%06X", (unsigned)COMMANDS[i].rgb);
    out += "{\"i\":";
    out += i;
    out += ",\"id\":\"";
    out += COMMANDS[i].id;
    out += "\",\"name\":\"";
    out += jsonEscape(COMMANDS[i].name);
    out += "\",\"note\":\"";
    out += jsonEscape(COMMANDS[i].note);
    out += "\",\"color\":\"";
    out += hex;
    out += "\",\"mode\":\"";
    out += modeName(COMMANDS[i].mode);
    out += "\"}";
  }
  out += "]}";
  server.send(200, "application/json", out);
}

static void handleState() {
  uint16_t absIdx = 0;
  const bool has = catalogCurrentIndex(absIdx);

  String out = "{\"mode\":\"";
  out += modeName(catalogMode());
  out += "\",\"position\":";
  out += catalogPosition();
  out += ",\"count\":";
  out += catalogCurrentCount();
  out += ",\"index\":";
  out += has ? (int)absIdx : -1;
  out += ",\"name\":\"";
  out += has ? jsonEscape(COMMANDS[absIdx].name) : String("");
  out += "\",\"net\":{\"mode\":\"";
  out += (netMode() == NetMode::STA) ? "STA" : "AP";
  out += "\",\"ssid\":\"";
  out += jsonEscape(netSsid());
  out += "\",\"ip\":\"";
  out += netIp();
  out += "\"},\"version\":\"" FW_VERSION "\"}";

  server.send(200, "application/json", out);
}

static void handleSend() {
  if (!sameOrigin()) return;

  if (!server.hasArg("index")) {
    server.send(400, "application/json", "{\"error\":\"falta index\"}");
    return;
  }

  const long idx = server.arg("index").toInt();
  if (idx < 0 || idx >= (long)COMMAND_COUNT) {
    server.send(400, "application/json", "{\"error\":\"index fuera de rango\"}");
    return;
  }

  if (!appSendAbsolute((uint16_t)idx)) {
    server.send(500, "application/json", "{\"error\":\"no se pudo emitir\"}");
    return;
  }

  server.send(200, "application/json", "{\"ok\":true}");
}

// Emite una trama PRONTO arbitraria. Es la vía para probar tramas nuevas sin
// recompilar; queda fuera del catálogo y no altera la selección actual.
static void handleRaw() {
  if (!sameOrigin()) return;

  if (!server.hasArg("pronto")) {
    server.send(400, "application/json", "{\"error\":\"falta pronto\"}");
    return;
  }

  const String pronto = server.arg("pronto");

  // Se acota antes de mirar el contenido: el buffer del emisor es fijo y no
  // tiene sentido recorrer una cadena que nunca va a caber.
  if (pronto.length() < 16 || pronto.length() > PRONTO_MAX_CHARS) {
    server.send(400, "application/json", "{\"error\":\"longitud fuera de rango\"}");
    return;
  }

  for (size_t i = 0; i < pronto.length(); i++) {
    const char c = pronto[i];
    // Misma tolerancia que el parser del emisor: hex y espacios, nada más.
    const bool ok = isxdigit((unsigned char)c) || c == ' ' || c == '\t' ||
                    c == '\r' || c == '\n';
    if (!ok) {
      server.send(400, "application/json", "{\"error\":\"solo hex y espacios\"}");
      return;
    }
  }

  if (!appSendRaw(pronto.c_str())) {
    server.send(400, "application/json", "{\"error\":\"trama PRONTO invalida\"}");
    return;
  }

  server.send(200, "application/json", "{\"ok\":true}");
}

static void handleScan() {
  const int n = WiFi.scanNetworks();

  String out = "[";
  for (int i = 0; i < n; i++) {
    if (i) out += ',';
    out += "{\"ssid\":\"";
    out += jsonEscape(WiFi.SSID(i));
    out += "\",\"rssi\":";
    out += WiFi.RSSI(i);
    out += ",\"lock\":";
    out += (WiFi.encryptionType(i) == WIFI_AUTH_OPEN) ? "false" : "true";
    out += '}';
  }
  out += ']';

  WiFi.scanDelete();
  server.send(200, "application/json", out);
}

static void handleWifi() {
  if (!sameOrigin()) return;

  const String ssid = server.arg("ssid");
  const String pass = server.arg("pass");

  if (!netSaveCredentials(ssid, pass)) {
    server.send(400, "application/json", "{\"error\":\"credenciales invalidas\"}");
    return;
  }

  server.send(200, "application/json", "{\"ok\":true,\"restarting\":true}");
}

static void handleForget() {
  if (!sameOrigin()) return;
  netForget();
  server.send(200, "application/json", "{\"ok\":true,\"restarting\":true}");
}

// En modo portal, cualquier ruta desconocida redirige a la raíz: es lo que
// dispara la ventana de "iniciar sesión en la red" en iOS y Android.
static void handleNotFound() {
  if (netMode() == NetMode::AP) {
    server.sendHeader("Location", String("http://") + netIp() + "/", true);
    server.send(302, "text/plain", "");
    return;
  }
  server.send(404, "text/plain", "not found");
}

void webBegin() {
  static const char* kHeaders[] = { "X-Requested-With" };
  server.collectHeaders(kHeaders, 1);

  server.on("/", HTTP_GET, handleRoot);
  server.on("/api/commands", HTTP_GET, handleCommands);
  server.on("/api/state", HTTP_GET, handleState);
  server.on("/api/scan", HTTP_GET, handleScan);
  server.on("/api/send", HTTP_POST, handleSend);
  server.on("/api/raw", HTTP_POST, handleRaw);
  server.on("/api/wifi", HTTP_POST, handleWifi);
  server.on("/api/forget", HTTP_POST, handleForget);
  server.onNotFound(handleNotFound);

  server.begin();
  Serial.printf("[web] http://%s/\n", netIp().c_str());
}

void webUpdate() {
  server.handleClient();
}
