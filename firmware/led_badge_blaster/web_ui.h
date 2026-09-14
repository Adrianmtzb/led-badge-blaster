#pragma once
#include <Arduino.h>

// Página servida por el propio dispositivo. Se manda tal cual, sin plantillas:
// todo lo dinámico se pide por /api/*. Sin fuentes ni scripts externos, porque
// en modo portal no hay salida a internet.

static const char INDEX_HTML[] PROGMEM = R"HTML(<!DOCTYPE html>
<html lang="es">
<head>
<meta charset="utf-8">
<meta name="viewport" content="width=device-width,initial-scale=1,viewport-fit=cover">
<meta name="color-scheme" content="dark light">
<title>LED Badge Blaster</title>
<style>
:root{
  --ink:#0b0d12; --panel:#141821; --sunk:#0e1118; --line:#242a36;
  --text:#e9ebf2; --dim:#8d94a8; --warn:#ffb454;
  --live:#f05000; --live-ink:#000;
  --r:14px;
}
@media(prefers-color-scheme:light){
  :root{--ink:#f4f5f8; --panel:#fff; --sunk:#eceef3; --line:#dcdfe8;
        --text:#141821; --dim:#5f6779; --warn:#9a5b00}
}
*{box-sizing:border-box}
html{-webkit-text-size-adjust:100%}
body,button,summary{touch-action:manipulation}
button{-webkit-tap-highlight-color:transparent}
body{
  margin:0 auto; max-width:44rem; padding:0 1rem env(safe-area-inset-bottom);
  background:var(--ink); color:var(--text);
  font:16px/1.5 system-ui,-apple-system,"Segoe UI",sans-serif;
  -webkit-font-smoothing:antialiased;
}
button{font:inherit;color:inherit;cursor:pointer}
:focus-visible{outline:2px solid var(--text);outline-offset:2px}

/* --- lectura en vivo: lo que el badge va a emitir --- */
#live{
  background:var(--live); color:var(--live-ink);
  border-radius:0 0 var(--r) var(--r);
  padding:1.3rem 1rem 1.05rem; margin:0 -1rem 1.5rem;
  transition:background .25s ease;
}
#live h1{margin:0;font-size:1.65rem;line-height:1.08;font-weight:640;letter-spacing:-.022em}
#live p{margin:.3rem 0 0;font-size:.86rem;opacity:.78}
#live .tags{display:flex;gap:.9rem;margin-top:.6rem;font-size:.74rem;opacity:.68;
  font-family:ui-monospace,SFMono-Regular,Menlo,monospace}
#live small{display:block;margin-top:.55rem;font-size:.78rem;opacity:.72}

/* --- categorías --- */
#tabs{display:flex;gap:.4rem;overflow-x:auto;scrollbar-width:none;
  margin:0 -1rem 0;padding:0 1rem .2rem}
#tabs::-webkit-scrollbar{display:none}
#tabs button{
  flex:none;padding:.42rem .8rem;border:1px solid var(--line);border-radius:999px;
  background:var(--panel);font-size:.88rem;white-space:nowrap;
}
#tabs button[aria-selected=true]{background:var(--text);border-color:var(--text);color:var(--ink);font-weight:600}
#tabs button.adv{border-style:dashed;color:var(--warn)}
#tabs button.adv[aria-selected=true]{background:var(--warn);border-color:var(--warn);color:#000}

/* --- avanzado: escribe memoria persistente, va aparte del catálogo --- */
[hidden]{display:none!important}
#canal{border:1px dashed var(--warn);border-radius:var(--r);padding:1rem;margin-top:.4rem}
#canal .aviso{margin:0 0 1rem;font-size:.84rem;color:var(--warn);line-height:1.45}
#canal #c-msg{margin:1rem 0 0}
#canal label{display:block;font-size:.78rem;color:var(--dim);margin-bottom:.25rem}
#canal input[type=number]{width:100%;font-size:1.6rem;text-align:center;padding:.5rem}
#canal .row{margin-top:1rem}
#canal .row button{flex:1}
#canal details{margin-top:1rem;font-size:.8rem;color:var(--dim)}
#canal summary{cursor:pointer}
#canal output{display:block;margin-top:.6rem;padding:.6rem;border-radius:8px;white-space:pre-wrap;
  background:var(--sunk);color:var(--dim);word-break:break-all;
  font-family:ui-monospace,SFMono-Regular,Menlo,monospace;font-size:.68rem;line-height:1.5}
#canal .nota{margin:.9rem 0 0;font-size:.78rem;color:var(--dim);line-height:1.45}
#blurb{margin:.8rem 0 1rem;font-size:.85rem;color:var(--dim);max-width:52ch}

/* --- rejilla de efectos: el color ocupa casi todo el botón --- */
#grid{display:grid;grid-template-columns:repeat(auto-fill,minmax(6.2rem,1fr));gap:.6rem}
#grid button{
  display:block;width:100%;padding:0;border:1px solid var(--line);border-radius:12px;
  background:var(--panel);overflow:hidden;text-align:left;
  transition:transform .12s ease,border-color .12s ease;
}
#grid button:active{transform:scale(.97)}
#grid button .sw{display:block;height:3.6rem;border-bottom:1px solid var(--line)}
#grid button .cap{display:block;padding:.42rem .55rem .5rem}
#grid button .nm{font-size:.8rem;line-height:1.25;overflow:hidden;
  display:-webkit-box;-webkit-line-clamp:2;-webkit-box-orient:vertical}
#grid button .hx{display:block;margin-top:.1rem;font-size:.65rem;color:var(--dim);
  font-family:ui-monospace,SFMono-Regular,Menlo,monospace}
#grid button[aria-pressed=true]{border-color:var(--text);box-shadow:inset 0 0 0 1px var(--text)}
#grid button[aria-pressed=true] .nm{font-weight:640}

/* --- ajustes, plegados: no es lo que vienes a hacer --- */
details{margin:2rem 0 0;border-top:1px solid var(--line);padding-top:1rem}
summary{cursor:pointer;font-size:.9rem;color:var(--dim);list-style:none}
summary::-webkit-details-marker{display:none}
summary::before{content:"+ ";font-family:ui-monospace,monospace}
details[open] summary::before{content:"– "}
details[open] summary{margin-bottom:1rem;color:var(--text)}
label{display:block;font-size:.8rem;color:var(--dim);margin:0 0 .3rem}
input{width:100%;padding:.6rem .7rem;border:1px solid var(--line);border-radius:10px;
  background:var(--sunk);color:var(--text);font:inherit;font-size:1rem;margin-bottom:.9rem}
#nets{display:flex;flex-direction:column;gap:.3rem;max-height:13rem;overflow-y:auto;margin-bottom:.9rem}
#nets button{display:flex;align-items:center;justify-content:space-between;gap:.75rem;
  width:100%;padding:.55rem .7rem;border:1px solid var(--line);border-radius:10px;
  background:var(--panel);text-align:left}
#nets .ss{overflow:hidden;text-overflow:ellipsis;white-space:nowrap;font-size:.9rem}
#nets .meta{display:flex;align-items:center;gap:.5rem;flex:none;color:var(--dim);font-size:.72rem}
.bars{display:flex;align-items:flex-end;gap:2px;height:12px}
.bars i{width:3px;border-radius:1px;background:var(--line)}
.bars i.on{background:var(--text)}
.bars i:nth-child(1){height:25%}.bars i:nth-child(2){height:50%}
.bars i:nth-child(3){height:75%}.bars i:nth-child(4){height:100%}
.row{display:flex;gap:.5rem;flex-wrap:wrap}
.row button{padding:.6rem 1rem;border-radius:10px;border:1px solid var(--line);background:var(--panel)}
.row button.primary{background:var(--text);border-color:var(--text);color:var(--ink);font-weight:600}
#msg{font-size:.85rem;color:var(--dim);margin-top:.8rem;min-height:1.3em}
#msg.warn{color:var(--warn)}
footer{margin:2rem 0;padding-top:1rem;border-top:1px solid var(--line);
  font-size:.75rem;line-height:1.55;color:var(--dim)}
footer b{color:var(--text);font-weight:600}
footer p{margin:0 0 .7rem}
#ver{font-family:ui-monospace,SFMono-Regular,Menlo,monospace;font-size:.72rem}
#lang{padding:.3rem .7rem;border:1px solid var(--line);border-radius:999px;
  background:var(--panel);font-size:.72rem;letter-spacing:.04em}
#ver b{font-weight:600}
@media(prefers-reduced-motion:reduce){*{transition:none!important}}
</style>
</head>
<body>

<div id="live">
  <h1 id="live-name">…</h1>
  <p id="live-note"></p>
  <div class="tags"><span id="live-id"></span><span id="live-hex"></span></div>
  <small id="live-net"></small>
</div>

<main>
  <nav id="tabs"></nav>
  <p id="blurb"></p>
  <div id="grid"></div>

  <section id="canal" hidden>
    <p class="aviso"><b data-t="canAvisoB"></b> <span data-t="canAviso"></span></p>

    <label for="c-num" data-t="canLabel"></label>
    <input id="c-num" type="number" min="1" max="31" value="1" inputmode="numeric">

    <div class="row">
      <button type="button" id="c-go" class="primary" data-t="canGo"></button>
    </div>

    <p id="c-msg" class="aviso" hidden></p>
    <details id="c-det" hidden><summary data-t="canVer"></summary>
      <output id="c-frame"></output>
    </details>

    <p class="nota" data-t="canNota"></p>
  </section>
</main>

<details>
  <summary data-t="red"></summary>
  <form id="wf">
    <label for="ssid" data-t="ssid"></label>
    <input id="ssid" autocomplete="off" maxlength="32" required>
    <div id="nets"></div>
    <label for="pass" data-t="pass"></label>
    <input id="pass" type="password" autocomplete="new-password" maxlength="63">
    <div class="row">
      <button class="primary" type="submit" data-t="guardar"></button>
      <button type="button" id="scan" data-t="buscar"></button>
      <button type="button" id="forget" data-t="olvidar"></button>
    </div>
  </form>
  <div id="msg"></div>
</details>

<footer>
  <p id="ver"></p>
  <p><b data-t="legalB"></b> <span data-t="legal"></span></p>
  <p><button type="button" id="lang"></button></p>
</footer>

<script>
const H={'X-Requested-With':'led-badge'};
const $=s=>document.querySelector(s);
let cats=[],cmds=[],st={},tab='';

// --- idioma --------------------------------------------------------------
// Solo las cadenas del panel. Los nombres de efectos y categorías vienen en
// los dos idiomas desde /api/commands, generados desde data/i18n/en.json.
const T={
 es:{conectando:'Conectando…',sinEfecto:'Sin efecto',toca:'Toca un color para emitirlo',
  enRed:(n,i)=>'En la red '+n+', responde en '+i,
  portal:(n,i)=>'Portal propio '+n+', responde en '+i,
  canal:'Canal',canalBlurb:'Avanzado. Reprograma a qué grupo responde un badge.',
  canAvisoB:'Escribe en la memoria persistente del badge.',
  canAviso:'A diferencia de los colores, esto no se deshace solo: el cambio sobrevive al apagado. Úsalo solo en badges propios.',
  canLabel:'Canal',canGo:'Poner el badge en este canal',canVer:'Ver las tramas emitidas',
  canNota:'Apunta el emisor al badge y pulsa. El badge destella al recibirlo. Afecta a todos los badges que estén a la vista, así que hazlo de uno en uno. Las tramas se calculan en este navegador y salen por /api/raw.',
  emitiendo:'Emitiendo…',listo:n=>'Listo: el badge está en el canal '+n+'.',
  falloIr:'No se pudo emitir. Comprueba la conexión con el dispositivo.',
  red:'Red y ajustes',ssid:'Red WiFi',pass:'Contraseña',guardar:'Guardar y conectar',
  buscar:'Buscar redes',olvidar:'Olvidar red',abierta:'abierta',
  buscando:'Buscando redes…',elige:'Toca una red para elegirla.',ninguna:'No se vio ninguna red.',
  falloScan:'El escaneo falló. Inténtalo otra vez.',elegida:n=>'Red elegida: '+n,
  guardando:'Guardando…',reiniciando:'Guardado. El dispositivo se reinicia…',
  falloWifi:'No se pudo guardar. Revisa la contraseña e inténtalo otra vez.',
  olvidarConf:'¿Borrar las credenciales y volver al portal?',
  olvidando:'Credenciales borradas. Reiniciando en modo portal.',
  fw:'Firmware ',desconocida:'desconocida',
  sinRespuesta:'Sin respuesta',
  recarga:'Recarga la página o vuelve a conectarte a la red del dispositivo.',
  legalB:'Herramienta independiente, compatible con badges PixMob.',
  legal:'No está afiliada ni respaldada por PixMob / Eski Inc. Úsala solo con dispositivos propios o que tengas permiso para probar. No la uses en eventos en directo ni para interferir con espectáculos, equipos de recinto o dispositivos ajenos.'},
 en:{conectando:'Connecting…',sinEfecto:'No effect',toca:'Tap a color to send it',
  enRed:(n,i)=>'On network '+n+', answering at '+i,
  portal:(n,i)=>'Own portal '+n+', answering at '+i,
  canal:'Channel',canalBlurb:'Advanced. Reprograms which group a badge responds to.',
  canAvisoB:'Writes to the badge’s persistent memory.',
  canAviso:'Unlike the colors, this does not undo itself: the change survives a power cycle. Use it only on badges you own.',
  canLabel:'Channel',canGo:'Put the badge on this channel',canVer:'Show the frames sent',
  canNota:'Point the emitter at the badge and press. The badge flashes when it receives it. This reaches every badge in sight, so do them one at a time. The frames are computed in this browser and go out through /api/raw.',
  emitiendo:'Sending…',listo:n=>'Done: the badge is on channel '+n+'.',
  falloIr:'Could not send. Check the connection to the device.',
  red:'Network and settings',ssid:'WiFi network',pass:'Password',guardar:'Save and connect',
  buscar:'Scan networks',olvidar:'Forget network',abierta:'open',
  buscando:'Scanning…',elige:'Tap a network to pick it.',ninguna:'No networks found.',
  falloScan:'The scan failed. Try again.',elegida:n=>'Network picked: '+n,
  guardando:'Saving…',reiniciando:'Saved. The device is restarting…',
  falloWifi:'Could not save. Check the password and try again.',
  olvidarConf:'Clear the credentials and go back to the portal?',
  olvidando:'Credentials cleared. Restarting in portal mode.',
  fw:'Firmware ',desconocida:'unknown',
  sinRespuesta:'No response',
  recarga:'Reload the page or reconnect to the device network.',
  legalB:'Independent tool, compatible with PixMob badges.',
  legal:'Not affiliated with or endorsed by PixMob / Eski Inc. Use it only with devices you own or have permission to test. Do not use it at live events or to interfere with performances, venue equipment or devices belonging to others.'},
};

let L=(()=>{
  try{const g=localStorage.getItem('lang');if(T[g])return g}catch(e){}
  return (navigator.language||'').toLowerCase().startsWith('es')?'es':'en';
})();
const t=k=>T[L][k];

// Los nombres del catálogo llegan en ambos idiomas; en inglés puede faltar
// alguno si el firmware es viejo, así que el español queda de reserva.
const cname=o=>(L==='en'&&o.nameEn)||o.name||'';
const lb=c=>(L==='en'&&c.labelEn)||c.label||'';
const bl=c=>(L==='en'&&c.blurbEn)||c.blurb||'';
const nt=o=>L==='en'?(o.noteEn||''):(o.note||'');

function paintStatic(){
  document.documentElement.lang=L;
  if(!cmds.length)$('#live-name').textContent=t('conectando');
  for(const el of document.querySelectorAll('[data-t]'))el.textContent=t(el.dataset.t);
  $('#lang').textContent=L==='es'?'English':'Español';
}

$('#lang').onclick=()=>{
  L=L==='es'?'en':'es';
  try{localStorage.setItem('lang',L)}catch(e){}
  paintStatic();drawVersion();paintLive();drawTabs();drawGrid();msg('');
};

const j=async(u,o)=>{const r=await fetch(u,o);if(!r.ok)throw new Error(r.status);return r.json()};
const msg=(t,warn)=>{const m=$('#msg');m.textContent=t;m.classList.toggle('warn',!!warn)};

// Sobre un color arbitrario, el texto se elige por luminancia: si no, los
// amarillos del catálogo quedan ilegibles con tinta blanca.
function inkFor(hex){
  const n=parseInt(hex.slice(1),16);
  const f=c=>{c/=255;return c<=.03928?c/12.92:Math.pow((c+.055)/1.055,2.4)};
  const L=.2126*f(n>>16&255)+.7152*f(n>>8&255)+.0722*f(n&255);
  return L>.36?'#000':'#fff';
}

function paintLive(){
  const c=cmds.find(c=>c.i===st.index);
  const hex=c?c.color:'#333333';
  document.documentElement.style.setProperty('--live',hex);
  document.documentElement.style.setProperty('--live-ink',inkFor(hex));
  $('#live-name').textContent=c?cname(c):t('sinEfecto');
  $('#live-note').textContent=c?(nt(c)||t('toca')):'';
  $('#live-id').textContent=c?c.id:'';
  $('#live-hex').textContent=c?c.color:'';
  const n=st.net||{};
  $('#live-net').textContent=n.ssid?(n.mode==='STA'?t('enRed'):t('portal'))(n.ssid,n.ip):'';
}

// El catálogo y el canal no son lo mismo: los colores son efímeros y el canal
// escribe EEPROM, así que la pestaña va marcada aparte y no sale del catálogo.
const CANAL='@canal';

function drawTabs(){
  const nav=$('#tabs');nav.textContent='';
  const add=(key,label,adv)=>{
    const b=document.createElement('button');
    b.textContent=label;
    if(adv)b.className='adv';
    b.setAttribute('aria-selected',key===tab);
    b.onclick=()=>{tab=key;drawTabs();drawGrid()};
    nav.appendChild(b);
  };
  for(const c of cats)add(c.key,lb(c),false);
  add(CANAL,t('canal'),true);

  const c=cats.find(c=>c.key===tab);
  $('#blurb').textContent=tab===CANAL?t('canalBlurb'):(c?bl(c):'');
}

function drawGrid(){
  const g=$('#grid');g.textContent='';
  $('#canal').hidden=tab!==CANAL;
  g.hidden=tab===CANAL;
  if(tab===CANAL)return;
  for(const c of cmds){
    if(c.mode!==tab)continue;
    const b=document.createElement('button');
    b.type='button';
    b.setAttribute('aria-pressed',c.i===st.index);
    b.title=nt(c)||cname(c);

    const sw=document.createElement('span');
    sw.className='sw';sw.style.background=c.color;

    const cap=document.createElement('span');
    cap.className='cap';
    const nm=document.createElement('span');
    nm.className='nm';nm.textContent=cname(c);
    const hx=document.createElement('span');
    hx.className='hx';hx.textContent=c.color;
    cap.append(nm,hx);

    b.append(sw,cap);
    b.onclick=()=>send(c.i);
    g.appendChild(b);
  }
}

// El dispositivo atiende una petición a la vez, así que tocar rápido encolaba
// un POST y un GET de estado por toque y el panel se quedaba atrás. Ahora la
// selección se pinta al instante y, mientras hay un envío en vuelo, los toques
// siguientes se resumen en el último: lo que importa es el color que acabas de
// tocar, no emitir los seis intermedios.
let sending=false,pending=null;

async function send(i){
  st.index=i;paintLive();drawGrid();

  if(sending){pending=i;return}
  sending=true;
  try{
    let idx=i;
    for(;;){
      await j('/api/send?index='+idx,{method:'POST',headers:H});
      if(pending===null)break;
      idx=pending;pending=null;
    }
    msg('');
  }catch(e){
    msg(t('falloIr'),1);
    await refresh().catch(()=>{});   // el estado local puede haberse ido
  }finally{sending=false;pending=null}
}

async function refresh(){
  st=await j('/api/state');
  drawVersion();
  paintLive();
  drawGrid();
}

// La versión sale de /api/state, así que es la que corre de verdad en la placa.
// Sirve para comprobar de un vistazo si una reinstalación entró.
function drawVersion(){
  const el=$('#ver');
  el.textContent=t('fw');
  const b=document.createElement('b');
  b.textContent=st.version?'v'+st.version:t('desconocida');
  el.appendChild(b);
}

// --- codificador PixMob -------------------------------------------------
// Implementado desde la documentación del protocolo (jamesw343/PixMob_IR, MIT),
// no copiado de su código. La trama se calcula aquí, en el navegador: el
// firmware sigue sin lógica de protocolo y solo emite lo que recibe en
// /api/raw. Validado reproduciendo los 67 presets del catálogo byte a byte.
const TBL=(()=>{const h='21325465a99a6d295692a1b4b284662a4c6aa6956251422435468aac8c6c2c4a'
  +'5986a4a2916455442231b1528596a5695a2d4d894534612536ad94aa8d499926';
  return Array.from({length:64},(_,k)=>parseInt(h.substr(k*2,2),16))})();

const CELL=26.5;      // celda T, en ciclos de portadora
const TAIL=0x076D;    // silencio final entre paquetes, ~50 ms

function toPronto(logical){
  // Solo se usan los 6 bits bajos de cada byte a partir del 0x02; el magic no
  // se sustituye y el checksum se calcula sobre los bytes ya sustituidos.
  const e=[0x80,0,...logical.map(b=>TBL[b&63])];
  let sum=0;for(let k=2;k<e.length;k++)sum=(sum+e[k])&255;
  e[1]=TBL[(sum>>2)&63];

  let bits='';
  for(const b of e)bits+=b.toString(2).padStart(8,'0').split('').reverse().join('');
  bits=bits.replace(/^0+|0+$/g,'');   // los ceros de guarda no son distinguibles

  const w=[];let n=1;
  for(let k=1;k<=bits.length;k++){
    if(bits[k]===bits[k-1]){n++;continue}
    w.push(Math.floor(n*CELL+0.5));n=1;
  }
  if(w.length%2)w.push(TAIL);         // cerrar el último par MARK/SPACE
  return [0,0x6D,w.length>>1,0,...w]
    .map(x=>x.toString(16).toUpperCase().padStart(4,'0')).join(' ');
}

// Ambos comandos de grupo comparten cuerpo: type=0b111 y onstrt=1 -> flags 0x0F.
// El byte 0x07 los separa: 1 cambia de ranura, 2 escribe un id en la ranura.
// El color va en RGB compacto de 12 bits y es lo que destella al recibir.
const RGB=0xFC00FC;

// El protocolo tiene ocho ranuras de grupo, pero exponerlas no aporta nada a
// quien solo quiere poner un badge en un canal: usamos siempre la 0.
const RANURA=0;

function grupoTrama(tipo,canal){
  const r=RGB>>16&255,g=RGB>>8&255,b=RGB&255;
  return toPronto([
    0x0F,
    (r>>4&3)<<4|(g>>4&15),
    (b>>4&15)<<2|(r>>6&3),
    RANURA,
    tipo===2?canal&31:0,
    tipo,
    0,                      // sin restringir: llega a todos los badges a la vista
  ]);
}

// El #msg del formulario de red está dentro de un <details> plegado y no se ve
// desde esta pestaña, así que el canal tiene su propio destino.
const cmsg=t=>{const m=$('#c-msg');m.textContent=t;m.hidden=!t};

// Escribir el id no basta: el badge mantiene en memoria el grupo anterior hasta
// que reinicia o recibe un cambio de ranura. Por eso van los dos seguidos, que
// es lo que convierte esto en un solo paso para quien lo usa.
let canalEnCurso=false;

async function ponerCanal(){
  if(canalEnCurso)return;
  const v=parseInt($('#c-num').value,10);
  const canal=Number.isFinite(v)?Math.min(31,Math.max(1,v)):1;
  $('#c-num').value=canal;

  const tramas=[grupoTrama(2,canal),grupoTrama(1,canal)];
  $('#c-frame').textContent=tramas.join('\n\n');
  $('#c-det').hidden=false;

  canalEnCurso=true;
  $('#c-go').disabled=true;
  cmsg(t('emitiendo'));
  try{
    // Más repeticiones que un color suelto: si el badge está dormido, aquí
    // perder la primera trama deja el cambio a medias, no solo sin destello.
    for(const t of tramas){
      await j('/api/raw',{method:'POST',
        headers:{...H,'Content-Type':'application/x-www-form-urlencoded'},
        body:new URLSearchParams({pronto:t,repeat:'4'})});
      await new Promise(r=>setTimeout(r,250));   // dejar que el badge procese
    }
    cmsg(t('listo')(canal));
  }catch(e){
    cmsg(t('falloIr'));
  }finally{canalEnCurso=false;$('#c-go').disabled=false}
}

$('#c-go').onclick=ponerCanal;

const level=r=>r>=-55?4:r>=-65?3:r>=-75?2:1;

function drawNets(nets){
  const box=$('#nets');box.textContent='';

  // Un mismo SSID puede aparecer varias veces (repetidores): nos quedamos con
  // la antena que mejor se oye. Las de nombre vacío son redes ocultas.
  const best=new Map();
  for(const n of nets){
    if(!n.ssid)continue;
    const prev=best.get(n.ssid);
    if(!prev||n.rssi>prev.rssi)best.set(n.ssid,n);
  }
  const list=[...best.values()].sort((a,b)=>b.rssi-a.rssi);

  for(const n of list){
    const b=document.createElement('button');
    b.type='button';

    const ss=document.createElement('span');
    ss.className='ss';
    ss.textContent=n.ssid;              // textContent: el SSID es texto ajeno

    const meta=document.createElement('span');
    meta.className='meta';
    if(!n.lock){const o=document.createElement('span');o.textContent=t('abierta');meta.appendChild(o)}

    const bars=document.createElement('span');
    bars.className='bars';
    const lv=level(n.rssi);
    for(let k=1;k<=4;k++){const i=document.createElement('i');if(k<=lv)i.className='on';bars.appendChild(i)}
    meta.appendChild(bars);

    b.append(ss,meta);
    b.onclick=()=>{$('#ssid').value=n.ssid;$('#pass').focus();msg(t('elegida')(n.ssid))};
    box.appendChild(b);
  }
  return list.length;
}

$('#scan').onclick=async()=>{
  msg(t('buscando'));
  try{
    const n=drawNets(await j('/api/scan'));
    msg(n?t('elige'):t('ninguna'));
  }catch(e){msg(t('falloScan'),1)}
};

$('#wf').onsubmit=async e=>{
  e.preventDefault();
  const body=new URLSearchParams({ssid:$('#ssid').value,pass:$('#pass').value});
  msg(t('guardando'));
  try{
    await fetch('/api/wifi',{method:'POST',headers:{...H,'Content-Type':'application/x-www-form-urlencoded'},body});
    msg(t('reiniciando'));
  }catch(e){msg(t('falloWifi'),1)}
};

$('#forget').onclick=async()=>{
  if(!confirm(t('olvidarConf')))return;
  await fetch('/api/forget',{method:'POST',headers:H});
  msg(t('olvidando'));
};

(async()=>{
  paintStatic();   // antes del primer fetch: la página no debe salir en blanco
  try{
    const d=await j('/api/commands');
    cats=d.categories;cmds=d.commands;
    tab=(cats[0]||{}).key||'';
    drawTabs();await refresh();
  }catch(e){$('#live-name').textContent=t('sinRespuesta');$('#live-note').textContent=t('recarga')}
})();
</script>
</body>
</html>
)HTML";
