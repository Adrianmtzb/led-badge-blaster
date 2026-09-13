# Cómo contribuir

Gracias por pasarte. Esto es un proyecto pequeño, así que el proceso es ligero a
propósito.

---

## Modelo de ramas

Usamos **trunk-based**: una sola rama larga, `main`, siempre en estado publicable.

```
main ────●────●────●────●──── (siempre desplegable, protegida)
          \        /
           ●──●──●  feat/algo   (vida corta, se fusiona con squash)
```

- `main` está protegida. No se hace push directo.
- Cada cambio va en una rama corta que sale de `main` y vuelve por PR.
- Las PRs se fusionan con **squash**, para que `main` quede con un commit legible
  por cambio.
- Las versiones se marcan con **tags** `vX.Y.Z`, no con ramas.

Prefijos de rama: `feat/`, `fix/`, `docs/`, `refactor/`, `chore/`.

---

## Antes de mandar la PR

El CI compila cada PR automáticamente, así que no hace falta que demuestres que
compila. Lo que el CI **no** puede comprobar es lo importante de este proyecto:

1. **Que funcione en hardware.** El temporizado del botón, la emisión IR y el
   portal cautivo no se pueden verificar sin la placa delante. Cuenta en la PR
   qué probaste.

2. **Si tocaste la red**, prueba los dos modos: arranque sin credenciales
   (portal) y arranque con credenciales válidas (STA).

3. **Si tocaste `web_ui.h`**, ábrelo en un móvil de verdad. El panel se diseñó
   para eso y hay diferencias reales con el escritorio.

4. **Si tocaste los efectos**, no edites los ficheros generados. La fuente de
   verdad es `data/presets/*.json`; se reexporta con `make presets` y se
   regenera con `make catalog`. `make check-catalog` falla si te lo saltas.

Para compilar en local:

```bash
cd firmware/led_badge_blaster
arduino-cli compile --fqbn m5stack:esp32:m5stack_atom .
```

### Los binarios del instalador

No están en el repositorio. `docs/` solo guarda la página y el `manifest.json`;
los `.bin` los compila y publica el CI al crear un tag. Así no hay conflictos
binarios entre PRs ni riesgo de que la página publicada instale una versión
distinta de la que está en el código.

Si cambias los nombres o los offsets del `manifest.json`, tendrás que ajustar
también el job `instalador` del CI, que los valida contra una lista fija.

---

## Estilo

El código y los comentarios están **en español**. Mantenlo.

Los comentarios explican **por qué**, no qué. Si una línea necesita explicar qué
hace, normalmente lo que necesita es un nombre mejor. Los que hay en el código
suelen documentar una trampa concreta; por ejemplo, por qué el LED no se restaura
con `getPixelColor()`, o por qué el AP usa `WIFI_AP_STA`.

Otras convenciones que conviene respetar:

- Nada de `delay()` en `loop()`. El temporizado se hace con `millis()`.
- La categoría de un efecto vive en `Command::mode`, nunca deducida por rangos de
  índices.
- Todo lo que llegue de fuera (parámetros HTTP, tramas PRONTO, SSIDs) se valida
  antes de usarse.
- `pronto_data.h`, `catalog_data.cpp` y `docs/catalog.json` son generados. No se
  editan a mano; llevan cabecera de aviso.

### Posicionamiento del proyecto

Es una herramienta independiente **compatible con** badges PixMob, no un
producto de PixMob. Al contribuir, respeta esto:

- "PixMob" no aparece en nombres de fichero, carpetas ni identificadores. Solo
  en frases que describen compatibilidad, junto al aviso de no afiliación.
- Nada de logotipos ni estética de la marca, ni presets con nombres de artistas,
  giras o recintos.
- Las atribuciones MIT de [THIRD_PARTY_NOTICES.md](THIRD_PARTY_NOTICES.md) son
  obligatorias: la licencia del protocolo exige conservarlas.
- No se añade firmware extraído, volcados de flash ni código propietario.
- El aviso de uso aceptable vive en el README, en `web_ui.h` y en
  `docs/index.html`. Si rediseñas una de las páginas, tiene que seguir estando.
- Todo nombre o nota que añadas a `data/presets/` necesita su traducción en
  `data/i18n/en.json`. `make check-catalog` falla si falta. Las cadenas que no
  cambian entre idiomas se repiten a propósito: así la ausencia es siempre un
  olvido, no una decisión.
- Los comandos persistentes (EEPROM, cambio de grupo, reset) no se mezclan con
  los colores: si se añaden, van en una sección marcada como avanzada.

### Mensajes de commit

Título en imperativo, sin punto final, hasta unos 72 caracteres. El cuerpo, si
hace falta, explica el porqué y el contexto:

```
Mostrar las redes WiFi como lista tocable

El datalist solo sugiere al escribir y en movil practicamente no se ve, asi
que el escaneo ahora pinta botones con el nombre y la intensidad.
```

No añadas firmas ni menciones de herramientas de IA en los commits.

---

## Publicar una versión (mantenedor)

1. Subir la versión en los dos sitios con `make bump VERSION=1.2.3`.
   `FW_VERSION` (en `firmware/led_badge_blaster/config.h`) y `version` (en
   `docs/manifest.json`) **tienen que coincidir**: el CI falla si no.
2. Anotar los cambios en [CHANGELOG.md](CHANGELOG.md).
3. Fusionar eso en `main` por PR, como cualquier otro cambio.
4. Crear el tag y empujarlo:

   ```bash
   git tag -a v1.2.3 -m "LED Badge Blaster v1.2.3"
   git push origin v1.2.3
   ```

El resto lo hace el CI: compila, crea la Release con los binarios adjuntos y
despliega el instalador en GitHub Pages. No hay que tocar ningún `.bin` a mano.

---

## Reportar un fallo

Cuenta qué esperabas, qué pasó, y si puedes, pega la salida de la consola serie a
115200 baudios. Indica también si estabas en modo portal o conectado a tu red:
muchos comportamientos dependen de eso.
