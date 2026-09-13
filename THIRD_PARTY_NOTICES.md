# Avisos de terceros

Este proyecto es una implementación independiente. El protocolo de infrarrojos
que emite no lo inventó este repositorio: lo documentó la comunidad mediante
ingeniería inversa y lo publicó bajo licencia MIT. Aquí quedan los avisos de
copyright y licencia que esa licencia exige conservar.

Nada de este repositorio contiene firmware extraído, volcados de memoria ni
código propietario de terceros.

---

## Documentación del protocolo

### pixmob-ir-reverse-engineering

- **Autoría:** Dani Weidman
- **Origen:** <https://github.com/danielweidman/pixmob-ir-reverse-engineering>
- **Licencia:** MIT
- **Uso aquí:** documentación de la tabla de sustitución, el checksum y la
  temporización del protocolo.

```
MIT License

Copyright (c) 2022 Dani Weidman

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
```

### PixMob_IR

- **Autoría:** James Wang
- **Origen:** <https://github.com/jamesw343/PixMob_IR>
- **Documento:** `docs/ir_protocol.md`
- **Licencia:** MIT
- **Uso aquí:** documentación del comando configurable de 9 bytes (attack,
  sustain, release, chance, grupo) y de las capturas de referencia. También la
  del pipeline de codificación (tabla de sustitución de 64 entradas, checksum,
  serialización LSB-first) y la de los comandos `Set Group ID` y
  `Set Group Sel`, que usa la pestaña Canal del panel.
- **Nota:** el codificador de `web_ui.h` está **implementado desde ese
  documento**, no copiado del código de PixMob_IR. Se verifica reproduciendo los
  67 presets del catálogo byte a byte.

```
MIT License

Copyright (c) 2024-2025 James Wang

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
```

---

## Generación de tramas

### led-badge-encoder

- **Autoría:** Adrianmtzb
- **Origen:** <https://github.com/Adrianmtzb/led-badge-encoder>
- **Licencia:** MIT
- **Uso aquí:** las tramas de `data/presets/` son exportaciones literales de
  esta herramienta. `scripts/export_presets.mjs` reproduce la exportación.

```
MIT License

Copyright (c) 2026 Adrianmtzb

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
```

---

## Dependencias de compilación

Se instalan con `make deps` y no se redistribuyen en este repositorio. Sus
licencias viajan con cada paquete.

| Librería | Autoría | Licencia |
|---|---|---|
| [IRremoteESP8266](https://github.com/crankyoldgit/IRremoteESP8266) | David Conran y colaboradores | LGPL-2.1 |
| [Adafruit NeoPixel](https://github.com/adafruit/Adafruit_NeoPixel) | Adafruit Industries | LGPL-3.0 |
| [arduino-esp32](https://github.com/espressif/arduino-esp32) | Espressif Systems | LGPL-2.1 |

La página de instalación carga
[esp-web-tools](https://github.com/esphome/esp-web-tools) (Nabu Casa, Apache-2.0)
desde un CDN; tampoco se redistribuye aquí.

---

## Marcas

PixMob es una marca de su titular (Eski Inc.). Este proyecto **no está afiliado
ni respaldado** por PixMob / Eski Inc., y la marca se menciona únicamente para
describir compatibilidad, en el sentido descriptivo que permite el uso
nominativo. Ni el nombre, ni el logotipo, ni la identidad visual de PixMob se
usan en este proyecto.

## Patentes

Existen patentes de terceros sobre sistemas de control de iluminación
distribuida en eventos, entre ellas
[US10863607B2](https://patents.google.com/patent/US10863607B2/en), indicada como
vigente hasta 2037. Se cita como contexto informativo. Implementar un protocolo
de control documentado no implica automáticamente infringir reivindicación
alguna, y este aviso **no es asesoría legal**; si vas a darle a esto un uso
comercial, consulta a un profesional.

## Reclamaciones

Si representas a un titular de derechos y ves un problema concreto, abre un
[issue](https://github.com/Adrianmtzb/led-badge-blaster/issues) o usa los
canales de [SECURITY.md](SECURITY.md). Se atienden reclamaciones razonadas.
