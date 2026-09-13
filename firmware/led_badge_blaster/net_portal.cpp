#include "net_portal.h"
#include "config.h"

#include <WiFi.h>
#include <DNSServer.h>
#include <ESPmDNS.h>
#include <Preferences.h>

static Preferences prefs;
static DNSServer dns;

static NetMode mode = NetMode::AP;
static String apSsid;
static bool restartPending = false;
static uint32_t restartAtMs = 0;

static const IPAddress AP_IP(192, 168, 4, 1);

static void buildApSsid() {
  uint8_t mac[6];
  WiFi.macAddress(mac);
  char buf[32];
  snprintf(buf, sizeof(buf), "LED-Badge-%02X%02X", mac[4], mac[5]);
  apSsid = buf;
}

static void startAp() {
  // AP_STA en vez de AP a secas: la interfaz STA es la que permite escanear
  // redes desde el portal sin tumbar el punto de acceso.
  WiFi.mode(WIFI_AP_STA);
  WiFi.softAPConfig(AP_IP, AP_IP, IPAddress(255, 255, 255, 0));
  WiFi.softAP(apSsid.c_str(), AP_PASSWORD);

  // Portal cautivo: toda consulta DNS apunta al propio dispositivo.
  dns.setErrorReplyCode(DNSReplyCode::NoError);
  dns.start(53, "*", AP_IP);

  mode = NetMode::AP;

  Serial.printf("[net] Portal activo. SSID=%s pass=%s IP=%s\n",
                apSsid.c_str(), AP_PASSWORD, WiFi.softAPIP().toString().c_str());
}

static bool startSta(const String& ssid, const String& pass) {
  Serial.printf("[net] Conectando a \"%s\"...\n", ssid.c_str());

  WiFi.mode(WIFI_STA);
  WiFi.setSleep(false);

  // Sin IPv6, mDNS publica el registro A pero deja la consulta AAAA sin
  // respuesta. Los clientes preguntan las dos a la vez y se comen el timeout
  // entero del resolutor (5 s en macOS) antes de usar la IPv4 que ya tenían.
  WiFi.enableIPv6(true);

  WiFi.begin(ssid.c_str(), pass.c_str());

  const uint32_t deadline = millis() + WIFI_CONNECT_TIMEOUT_MS;
  while (WiFi.status() != WL_CONNECTED && (int32_t)(millis() - deadline) < 0) {
    delay(200);
  }

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("[net] Sin conexion, vuelvo al portal.");
    WiFi.disconnect(true);
    return false;
  }

  mode = NetMode::STA;
  Serial.printf("[net] Conectado. IP=%s\n", WiFi.localIP().toString().c_str());

  if (MDNS.begin(MDNS_HOST)) {
    MDNS.addService("http", "tcp", 80);
    Serial.printf("[net] http://%s.local\n", MDNS_HOST);
  }
  return true;
}

void netBegin() {
  buildApSsid();

  prefs.begin("ledbadge", false);
  const String ssid = prefs.getString("ssid", "");
  const String pass = prefs.getString("pass", "");

  if (ssid.length() && startSta(ssid, pass)) return;

  startAp();
}

void netUpdate() {
  if (mode == NetMode::AP) dns.processNextRequest();

  if (restartPending && (int32_t)(millis() - restartAtMs) >= 0) {
    Serial.println("[net] Reiniciando...");
    Serial.flush();
    ESP.restart();
  }
}

NetMode netMode()  { return mode; }
String  netApSsid() { return apSsid; }

String netSsid() {
  return (mode == NetMode::STA) ? WiFi.SSID() : apSsid;
}

String netIp() {
  return (mode == NetMode::STA) ? WiFi.localIP().toString()
                                : WiFi.softAPIP().toString();
}

bool netHasCredentials() {
  return prefs.getString("ssid", "").length() > 0;
}

bool netSaveCredentials(const String& ssid, const String& pass) {
  if (ssid.length() == 0 || ssid.length() > SSID_MAX_LEN) return false;
  if (pass.length() > PASS_MAX_LEN) return false;

  prefs.putString("ssid", ssid);
  prefs.putString("pass", pass);

  // El cliente está hablando por el AP que estamos a punto de tumbar, así que
  // el cambio se aplica reiniciando después de contestar.
  netScheduleRestart(1200);
  return true;
}

void netForget() {
  prefs.remove("ssid");
  prefs.remove("pass");
  netScheduleRestart(1200);
}

void netScheduleRestart(uint32_t delayMs) {
  restartPending = true;
  restartAtMs = millis() + delayMs;
}
