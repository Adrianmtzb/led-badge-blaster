# Changelog

Formato basado en [Keep a Changelog](https://keepachangelog.com/es-ES/1.1.0/).
Este proyecto sigue [Versionado Semántico](https://semver.org/lang/es/).

## [Sin publicar]

### Añadido

- Pestaña **Canal** en el panel: un campo y un botón para poner un badge en un
  canal. Por debajo encadena `Set Group ID` y `Set Group Sel`, que hacen falta
  los dos: sin el segundo el badge sigue con el grupo anterior. Va marcada y
  separada del catálogo porque escribe memoria persistente del badge. La trama la
  calcula el propio panel en el navegador y sale por `/api/raw`; el firmware
  sigue sin lógica de protocolo. El codificador está implementado desde la
  documentación de `jamesw343/PixMob_IR` (MIT) y se valida reproduciendo los 67
  presets del catálogo byte a byte.
- `POST /api/raw` y el comando serie `raw <trama>` emiten una trama PRONTO que
  no está en el catálogo, para probar tramas nuevas sin recompilar. Validan
  longitud y juego de caracteres, van detrás del mismo `X-Requested-With` que el
  resto de rutas que emiten, y no tocan la selección actual.
- La versión del firmware se muestra en el pie del panel, leída de
  `/api/state`, y la que va a escribir el instalador aparece junto al botón de
  la página pública. Comparar las dos dice si una reinstalación entró.

### Corregido

- El doble toque en el panel ya no hace zoom ni arrastra el retardo de ~300 ms
  que el navegador espera para distinguirlo de un toque simple
  (`touch-action: manipulation`). El zoom con dos dedos sigue funcionando.
- Tocar efectos rápido ya no encola una petición por toque. La selección se
  pinta al instante y, mientras hay un envío en vuelo, los toques siguientes se
  resumen en el último; el dispositivo atiende de una en una y se quedaba atrás.

## [2.0.0] - 2026-09-11

Se renombra el proyecto y se reemplaza el catálogo de efectos por completo.

### Cambios incompatibles

- **El proyecto pasa a llamarse `led-badge-blaster`.** Cambia la carpeta del
  sketch (`firmware/led_badge_blaster/`), el nombre del binario y la URL del
  instalador. Quien tuviera el repositorio clonado necesita volver a clonar o
  actualizar el remoto.
- **El catálogo se sustituye entero.** Las categorías `SPECIAL`, `PULSO` y
  `FADE` desaparecen; los índices de `/api/send` ya no corresponden a los
  mismos efectos y el rango pasa de 0–54 a 0–66.
- **`/api/commands` devuelve un objeto**, no un array: `{categories, commands}`.
  Cada comando trae `id`, `name`, `note` y `color` en lugar de `label`.
- **`/api/state` devuelve `name`** (el nombre legible) en lugar de `label`.

### Añadido

- 67 efectos en seis categorías (`CAPTURAS`, `BASICOS`, `ENVOLVENTES`,
  `MULTITUD`, `ESCENAS`, `GRUPOS`), cada uno con nombre y color propios.
- Las tramas se generan desde `data/presets/*.json` con
  `scripts/gen_catalog.py`; `scripts/export_presets.mjs` reexporta esos presets
  del generador web. `make check-catalog` impide que se desincronicen.
- `docs/catalog.json`: la paleta real del firmware, que consume la página de
  instalación para no tener una segunda copia de los colores.
- Comandos `cats` y `list` en la consola serie.
- `THIRD_PARTY_NOTICES.md` con las atribuciones MIT del protocolo y las notas
  sobre marcas y patentes de terceros.
- Sección de uso aceptable en el README, en el panel y en la página pública.

### Cambiado

- **Panel rediseñado**: los efectos se eligen por su color en una rejilla de
  muestras, con el efecto activo en cabecera. La configuración de red queda
  plegada, porque no es lo que se viene a hacer.
- **Página de instalación rediseñada**, con la paleta real del firmware, los
  pasos de instalación y las atribuciones.
- **El LED de estado muestra el color del efecto seleccionado**, no la
  categoría. El destello al emitir pasa de rojo a blanco, para que se vea
  también sobre los rojos y naranjas del catálogo.
- La navegación por categoría ya no mantiene un mapa de índices en RAM: los
  comandos llegan agrupados y basta con el primer índice y la longitud.

## [1.0.0] - 2026-09-11

Primera versión pública.

### Añadido

- Emisión de 55 tramas PRONTO en tres categorías: `SPECIAL`, `PULSO` y `FADE`.
- Portal cautivo: sin credenciales guardadas el dispositivo levanta la red
  `LED-Badge-XXXX` y sirve su propio panel de control.
- Modo estación con mDNS: al guardar una red, queda accesible en
  `http://led-badge.local`.
- Panel web adaptado a móvil, con selección de efecto por categoría y
  configuración de WiFi con escaneo de redes.
- API HTTP (`/api/commands`, `/api/state`, `/api/scan`, `/api/send`,
  `/api/wifi`, `/api/forget`) protegida contra CSRF por cabecera.
- Control por el botón de la placa: clic para emitir y avanzar, doble clic para
  cambiar de categoría y tres segundos para olvidar el WiFi.
- LED de estado que indica la categoría activa y destella al emitir.
- Consola serie de depuración a 115200 baudios.
- Instalador web con ESP Web Tools, sin necesidad de instalar nada.

[Sin publicar]: https://github.com/Adrianmtzb/led-badge-blaster/compare/v2.0.0...HEAD
[2.0.0]: https://github.com/Adrianmtzb/led-badge-blaster/releases/tag/v2.0.0
[1.0.0]: https://github.com/Adrianmtzb/led-badge-blaster/releases/tag/v1.0.0
