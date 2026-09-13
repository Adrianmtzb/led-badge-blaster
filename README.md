# LED Badge Blaster

[![CI](https://github.com/Adrianmtzb/led-badge-blaster/actions/workflows/ci.yml/badge.svg)](https://github.com/Adrianmtzb/led-badge-blaster/actions/workflows/ci.yml)
[![Licencia: MIT](https://img.shields.io/badge/licencia-MIT-blue.svg)](LICENSE)

Firmware libre para **M5Stack ATOM Lite** que emite efectos de color por
infrarrojos a badges LED compatibles con PixMob. Se instala desde el navegador
y se maneja desde el móvil: el propio dispositivo levanta una red WiFi y sirve
su panel de control.

Proyecto independiente, desarrollado por la comunidad para interoperabilidad,
preservación y estudio del protocolo. **No está afiliado ni respaldado por
PixMob / Eski Inc.**

---

## Uso aceptable

Esto emite infrarrojos a dispositivos reales, así que las reglas van antes que
las instrucciones:

> Úsalo solo con dispositivos **propios** o que tengas **permiso explícito**
> para probar.
>
> **No lo uses en eventos en directo**, ni para interferir con espectáculos,
> equipos de recinto o dispositivos de otras personas.

Los 67 efectos del catálogo son tramas de color único: encienden un badge con un
color y una envolvente, y no dejan rastro al apagarlo. Aparte va la pestaña
**Canal**, que sí **escribe en la EEPROM del badge** para cambiar a qué grupo
responde. Está separada del catálogo y marcada como avanzada precisamente porque
ese cambio es permanente. Se publica sin garantía de ningún tipo; verifica
siempre con tu propio hardware.

---

## Qué hace

- **67 efectos** repartidos en seis categorías, cada uno con su nombre y su
  color: `CAPTURAS` (25), `BASICOS` (10), `ENVOLVENTES` (8), `MULTITUD` (6),
  `ESCENAS` (13) y `GRUPOS` (5).
- **Panel con los colores a la vista.** Se elige el efecto por su color, no por
  un número; el LED de la placa muestra el que está seleccionado.
- **Pestaña Canal (avanzada)**: reprograma el grupo al que responde un badge.
  Escribe memoria persistente, así que va aparte de los colores.
- **Portal cautivo**: sin credenciales guardadas crea la red `LED-Badge-XXXX` y
  abre solo el panel al conectarte.
- **Modo red**: si le das tu WiFi, se une a ella y queda en `http://led-badge.local`.
- **API HTTP** para automatizar desde curl, Postman o lo que quieras.
- **Control físico** con el único botón de la placa.

## Qué hace falta

| | |
|---|---|
| Placa | M5Stack ATOM Lite (ESP32) |
| Emisor | LED IR en GPIO12 (el del ATOM, o uno externo) |
| Navegador | Chrome o Edge de escritorio, solo para instalar |

---

## Instalación

La forma fácil es desde el navegador, con el cable USB conectado:

**https://adrianmtzb.github.io/led-badge-blaster/**

Usa Web Serial, así que el firmware se descarga en tu navegador y se escribe
directo al dispositivo; no pasa por ningún servidor. Necesita Chrome o Edge de
escritorio: Firefox y Safari no implementan Web Serial, y en móvil no existe.

Si prefieres compilarlo tú, mira [Compilar desde el código](#compilar-desde-el-código).

---

## Primer arranque

1. Conéctate a la red **`LED-Badge-XXXX`** (las X son los últimos bytes de la MAC)
   con la clave **`ledbadge`**.
2. El panel se abre solo. Si tu móvil no lo detecta, entra a `http://192.168.4.1`.
3. Toca un color y ya está emitiendo.
4. Opcional: en **Red y ajustes**, pulsa *Buscar redes*, elige la tuya y guarda.
   El dispositivo se reinicia, se une a tu red y pasa a responder en
   `http://led-badge.local`.

## El botón

| Gesto | Acción |
|---|---|
| Clic | Envía el efecto actual y avanza al siguiente |
| Doble clic | Cambia de categoría |
| Mantener 3 s | Olvida el WiFi y vuelve al portal |

El LED de estado muestra **el color del efecto seleccionado**. Destella en
blanco cada vez que emite, y en magenta al olvidar las credenciales.

---

## El catálogo

Los efectos no se escriben a mano. Son exportaciones literales de un generador
de tramas que corre en el navegador
([led-badge-encoder](https://github.com/Adrianmtzb/led-badge-encoder), MIT), y de
ahí salen tanto el firmware como la paleta de la página de instalación:

```
data/presets/*.json                 exportación literal del generador
        │
        └── scripts/gen_catalog.py
                ├── firmware/led_badge_blaster/pronto_data.h    las 67 tramas
                ├── firmware/led_badge_blaster/catalog_data.cpp nombres y colores
                └── docs/catalog.json                           paleta de la web
```

```bash
make presets   # reexporta los JSON desde el generador (Playwright + Chrome)
make catalog   # regenera los tres ficheros
```

`make check-catalog` falla si lo que hay en el disco no cuadra con los presets,
así que el CI no deja que se desincronicen. Los ficheros generados llevan una
cabecera que lo avisa: no se editan a mano.

Cada JSON conserva, además de la trama, el color solicitado, el color ya
cuantizado por el badge, los bytes lógicos y codificados y el bitstream. El
firmware solo necesita tres de esos campos; el resto se guarda porque es lo que
permite auditar de dónde salió cada byte.

---

## API HTTP

Las rutas que cambian estado exigen la cabecera `X-Requested-With` (cualquier
valor sirve). Es un cortafuegos contra CSRF: una web ajena no puede añadir
cabeceras propias en una petición `no-cors`, y el dispositivo no responde
preflight. Los `GET` no la necesitan.

Los parámetros van como query string o `x-www-form-urlencoded`. **No acepta JSON**:
el `WebServer` del core ESP32 no lo parsea.

| Método | Ruta | Parámetros | Devuelve |
|---|---|---|---|
| `GET` | `/api/commands` | — | Categorías y efectos, con nombre y color |
| `GET` | `/api/state` | — | Categoría, posición, efecto actual y estado de red |
| `GET` | `/api/scan` | — | Redes WiFi visibles |
| `POST` | `/api/send` | `index` (0–66), `repeat` (opc. 0–9) | `{"ok":true}` |
| `POST` | `/api/raw` | `pronto` (hex PRONTO), `repeat` (opc. 0–9) | `{"ok":true}` |
| `POST` | `/api/wifi` | `ssid`, `pass` | Guarda y reinicia |
| `POST` | `/api/forget` | — | Borra credenciales y reinicia |

```bash
curl http://led-badge.local/api/commands
# {"categories":[{"key":"CAPTURAS","label":"Capturas","blurb":"…","count":25}, …],
#  "commands":[{"i":0,"id":"CAP_00","name":"Rojo","note":"P_PULSO_00",
#               "color":"#F02000","mode":"CAPTURAS"}, …]}

curl -X POST http://led-badge.local/api/send \
  -H 'X-Requested-With: curl' \
  -d 'index=12'
```

### Repeticiones

El badge apaga los LEDs y duerme el MCU tras **unos 60 s sin recibir nada**, y la
primera trama después de ese reposo se suele perder. Por eso cada envío repite el
paquete: `repeat=2` (tres envíos) por defecto, que es lo que hacen también el
botón de la placa y la consola serie. Cada repetición cuesta unos 77 ms de aire,
medidos contra la placa. El parámetro `repeat` lo ajusta entre 0 y 9; la pestaña
Canal usa 4, porque ahí perder una trama deja el cambio a medias.

Ojo si tocas esto: el parámetro `repeat` de `sendPronto` en IRremoteESP8266 solo
repite la *segunda* secuencia del PRONTO, y estas tramas la llevan vacía. La
repetición tiene que ser un bucle de llamadas, no ese argumento.

`/api/raw` emite una trama que no está en el catálogo. Existe para probar tramas
nuevas sin recompilar: no cambia la selección ni el color de reposo del LED, y
sólo acepta dígitos hex y espacios, hasta 64 palabras PRONTO. El emisor rechaza
cualquier cosa que no encaje en ese formato. Lo mismo desde el puerto serie con
`raw <trama>`.

```bash
curl -X POST http://led-badge.local/api/raw \
  -H 'X-Requested-With: curl' \
  --data-urlencode 'pronto=0000 006D 0013 0000 0035 006A 001B 0050 001B 001B 001B 001B 0035 001B 001B 006A 001B 0035 001B 001B 0035 0050 0035 001B 001B 0035 001B 0050 0035 001B 0035 0050 0035 001B 0035 0035 001B 0035 0035 001B 001B 04E7'
```

Emitir tramas arbitrarias amplía lo que el dispositivo puede hacer más allá del
catálogo, así que aplica igual el [uso aceptable](#uso-aceptable): sólo con
equipos propios o con permiso, y nunca en un evento en marcha.

### La pestaña Canal

Un campo y un botón: escribes el canal (1–31), apuntas al badge y pulsas.

Por debajo son dos comandos del protocolo, y hacen falta los dos: `Set Group ID`
guarda el canal en la EEPROM del badge, y `Set Group Sel` le hace releerla — sin
el segundo, el badge sigue con el grupo anterior hasta que reinicie. El panel los
manda seguidos, por eso es un solo paso. De las ocho ranuras de grupo que tiene
el protocolo usa siempre la 0, porque exponerlas no aporta nada a quien solo
quiere poner un badge en un canal.

La orden no distingue destinatario: llega a **todos los badges a la vista**, así
que hazlo de uno en uno.

Las tramas **se calculan en el navegador**, con el codificador que lleva el
propio panel, y se emiten por `/api/raw`. El firmware no tiene lógica de
protocolo: solo emite lo que recibe. El codificador está implementado desde la
[documentación del protocolo](https://github.com/jamesw343/PixMob_IR/blob/HEAD/docs/ir_protocol.md)
(MIT, ver [THIRD_PARTY_NOTICES.md](THIRD_PARTY_NOTICES.md)) y se valida
reproduciendo los 67 presets del catálogo byte a byte.

> [!WARNING]
> Este cambio **sobrevive al apagado del badge**. A diferencia de los colores, no
> se deshace solo. Úsalo únicamente con badges propios.

`/api/wifi` y `/api/forget` responden y **reinician 1,2 s después**, así que la
conexión se corta: el cliente suele estar hablando por el AP que se va a tumbar.

---

## Compilar desde el código

Hace falta [arduino-cli](https://arduino.github.io/arduino-cli/) con el core de
M5Stack y dos librerías. `make setup` comprueba lo que tienes e instala el resto:

```bash
make setup
make build
make flash
```

O a mano, si prefieres:

```bash
arduino-cli core install m5stack:esp32
arduino-cli lib install "IRremoteESP8266" "Adafruit NeoPixel"

cd firmware/led_badge_blaster
arduino-cli compile --fqbn m5stack:esp32:m5stack_atom --export-binaries .
arduino-cli upload  --fqbn m5stack:esp32:m5stack_atom -p /dev/cu.usbserial-XXXX .
```

La carpeta tiene que llamarse igual que el `.ino`; es un requisito de Arduino.
Tarda un par de minutos la primera vez. Si la subida falla con *port is busy*,
casi siempre es un monitor serie abierto ocupando el puerto.

Hay además una consola serie a 115200 baudios para depurar: escribe `help`.
`cats` lista las categorías y `list` los efectos de la actual con su color.

---

## Estructura

```
firmware/led_badge_blaster/
├── led_badge_blaster.ino  solo setup() y loop()
├── config.h               pines, tiempos, identidad
├── pronto_data.h          las 67 tramas IR en crudo      (generado)
├── catalog_data.cpp       nombres, colores y categorías  (generado)
├── catalog.*              navegación por categoría
├── ir_sender.*            parser PRONTO y emisión
├── status_led.*           LED de estado, sin bloqueos
├── button.*               clic, doble clic y pulsación larga
├── net_portal.*           AP con portal cautivo o STA con mDNS
├── web_ui.h               el panel, en PROGMEM
├── web_api.*              rutas HTTP
└── console.*              consola serie

data/presets/              tramas exportadas del generador (fuente de verdad)
scripts/
├── export_presets.mjs     reexporta los presets del generador
├── gen_catalog.py         presets → firmware + docs/catalog.json
└── check-manifest.py      valida offsets y versiones del instalador
docs/                      instalador web (los .bin los genera el CI)
```

---

## Licencia y atribuciones

Este firmware es [MIT](LICENSE) © Adrianmb.

El protocolo que emite lo documentó la comunidad mediante ingeniería inversa y
se publicó bajo MIT. Las atribuciones completas, junto con las notas sobre
marcas y patentes de terceros, están en
[THIRD_PARTY_NOTICES.md](THIRD_PARTY_NOTICES.md).

Este repositorio no contiene firmware extraído, volcados de memoria ni código
propietario de terceros.

PixMob es una marca de su titular y se menciona solo para describir
compatibilidad. Si representas a un titular de derechos y ves un problema
concreto, abre un [issue](https://github.com/Adrianmtzb/led-badge-blaster/issues)
o usa los canales de [SECURITY.md](SECURITY.md).

## Contribuir

Se aceptan PRs. Lee [CONTRIBUTING.md](CONTRIBUTING.md) para el modelo de ramas y
cómo probar los cambios. Si usas un asistente de IA, [AGENTS.md](AGENTS.md) tiene
el contexto y las trampas conocidas del proyecto.

Otros documentos: [CHANGELOG.md](CHANGELOG.md) y [SECURITY.md](SECURITY.md), que
explica qué protege este firmware y qué no.
