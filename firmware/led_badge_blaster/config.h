#pragma once
#include <Arduino.h>

// =====================================================
// Hardware: M5Stack ATOM Lite
// =====================================================
static const uint8_t PIN_BUTTON = 39;  // input-only, pull-up externo en la placa
static const uint8_t PIN_RGBLED = 27;  // WS2812
static const uint8_t PIN_IRLED  = 12;  // LED IR

// =====================================================
// Identidad
// =====================================================
#define FW_NAME    "led-badge-blaster"
#define FW_VERSION "2.0.0"
#define MDNS_HOST  "led-badge"

// =====================================================
// Red
// =====================================================
// WPA2 exige 8 caracteres como mínimo. Sin contraseña el AP quedaría abierto y
// cualquiera dentro del alcance podría controlar el dispositivo.
#define AP_PASSWORD "ledbadge"

static const uint32_t WIFI_CONNECT_TIMEOUT_MS = 15000;
static const uint8_t  SSID_MAX_LEN = 32;
static const uint8_t  PASS_MAX_LEN = 63;  // máximo de WPA2-PSK

// =====================================================
// Botón
// =====================================================
static const uint32_t DEBOUNCE_MS    = 35;
static const uint32_t DOUBLECLICK_MS = 320;
static const uint32_t LONGPRESS_MS   = 3000;

// =====================================================
// Emisor IR
// =====================================================
// El badge apaga los LEDs y duerme el MCU tras ~60 s sin recibir nada, y la
// primera trama tras ese reposo se suele perder. Repetir el paquete lo despierta
// y sube la probabilidad de recepción. Cada repetición cuesta ~78 ms de aire.
static const uint8_t IR_REPEATS     = 2;   // repeticiones extra por envío
static const uint8_t IR_REPEATS_MAX = 9;

// =====================================================
// LED de estado
// =====================================================
static const uint8_t  LED_BRIGHTNESS = 40;
static const uint32_t FLASH_MS       = 60;
