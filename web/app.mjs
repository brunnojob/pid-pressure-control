import {Simulation} from './model.mjs';
import {messages} from './i18n.mjs';
const $=id=>document.getElementById(id), sim=new Simulation();
let lang=new URLSearchParams(location.search).get('lang');
if(!Object.hasOwn(messages,lang)) {try{lang=sessionStorage.getItem('pressure-language');}catch{} }
if(!Object.hasOwn(messages,lang))lang='pt';
let running=false,apiState='apiLoading',repoData=null;
const locale=()=>lang==='pt'?'pt-BR':'en-US',t=key=>messages[lang][key];
const fmt=(x,d=1)=>x.toLocaleString(locale(),{minimumFractionDigits:d,maximumFractionDigits:d});
function svgElement(tag,attrs,text){const el=document.createElementNS('http://www.w3.org/2000/svg',tag);for(const [key,value]of Object.entries(attrs))el.setAttribute(key,value);if(text!==undefined)el.textContent=text;return el;}
function renderChart(){
  const history=sim.history,end=Math.max(60,sim.time),start=end-60;
  const x=time=>45+((time-start)/60)*740,y=pressure=>260-pressure/250*240;
  const grid=$('grid');grid.replaceChildren();
  for(let p=0;p<=250;p+=50){grid.append(svgElement('line',{x1:45,x2:785,y1:y(p),y2:y(p)}),svgElement('text',{x:35,y:y(p)+3,'text-anchor':'end'},String(p)));}
  for(let i=0;i<=6;i++){const time=start+i*10;grid.append(svgElement('text',{x:x(time),y:284,'text-anchor':'middle'},fmt(time,0)));}
  const path=field=>history.map((p,i)=>(i?'L':'M')+x(p.time).toFixed(2)+','+y(p[field]).toFixed(2)).join(' ');
  $('pressurePath').setAttribute('d',path('pressure'));$('targetPath').setAttribute('d',path('setpoint'));
}
function renderRepo(){
  $('apiStatus').textContent=t(apiState);
  $('version').textContent=repoData?.sha?.slice(0,7)||t('unavailable');
  $('updated').textContent=repoData?.date?new Date(repoData.date).toLocaleDateString(locale()):t('unavailable');
  const run=repoData?.run;
  $('checks').textContent=!run?t('unavailable'):t(run.status!=='completed'?'ciProgress':run.conclusion==='success'?'ciSuccess':run.conclusion==='failure'?'ciFailure':run.conclusion==='cancelled'?'ciCancelled':'ciOther');
}
function render(){
  $('pressure').textContent=fmt(sim.pressure);$('targetValue').textContent=fmt(sim.setpoint,0);$('valve').textContent=fmt(sim.valve);$('error').textContent=fmt(sim.setpoint-sim.pressure);
  $('clock').textContent=fmt(sim.time)+' s';$('setpointOutput').textContent=fmt(sim.setpoint,0)+' bar';
  $('valveNode').textContent=fmt(sim.valve,0)+'%';$('vesselPressure').textContent=fmt(sim.pressure,0)+' bar';
  $('liquid').style.height=(sim.pressure/250*100)+'%';$('flow').style.opacity=String(sim.valve/100);
  $('sensorState').textContent=t(sim.fault?'sensorOff':'sensorOk');
  $('status').textContent=t(sim.fault==='stale'?'staleFault':sim.fault?'fault':running?'ready':'paused');
  $('statusDetail').textContent=t(sim.fault?'faultDetail':running?'readyDetail':'pausedDetail');
  $('statusDot').style.background=sim.fault?'#ffc394':'var(--accent)';
  $('play').textContent=t(running?'pause':'play');$('load').textContent=t(sim.load?'removeLoad':'addLoad');renderChart();
}
function setLanguage(value){
  lang=value;document.documentElement.lang=lang==='pt'?'pt-BR':'en';$('language').value=lang;
  document.querySelectorAll('[data-i18n]').forEach(el=>el.textContent=t(el.dataset.i18n));
  $('chartTitle').textContent=t('chartTitle');$('chartDesc').textContent=t('chartDesc');document.title=lang==='pt'?'Pressure Lab · Controle de pressão PID':'Pressure Lab · PID pressure control';
  document.querySelector('meta[name=description]').content=t('intro');
  try{sessionStorage.setItem('pressure-language',lang);}catch{}
  const url=new URL(location.href);url.searchParams.set('lang',lang);history.replaceState(null,'',url);
  render();renderRepo();
}
$('language').addEventListener('change',e=>setLanguage(e.target.value));
$('play').addEventListener('click',()=>{running=!running;render();});
$('restart').addEventListener('click',()=>{sim.reset();$('setpoint').value=120;$('preset').value='balanced';applyPreset('balanced');render();});
$('setpoint').addEventListener('input',e=>{sim.setpoint=Number(e.target.value);render();});
function applyPreset(name){const values={balanced:[1.8,.5,.1],gentle:[.6,.3,.05],aggressive:[6,.5,.1]}[name];['kp','ki','kd'].forEach((key,i)=>{sim.pid.config[key]=values[i];$(key).value=values[i];});sim.pid.reset();}
$('preset').addEventListener('change',e=>{applyPreset(e.target.value);render();});
for(const key of ['kp','ki','kd'])$(key).addEventListener('change',e=>{const input=e.target,value=input.valueAsNumber;if(!Number.isFinite(value)||value<0||value>Number(input.max)){input.value=sim.pid.config[key];return;}sim.pid.config[key]=value;sim.pid.reset();});
$('load').addEventListener('click',()=>{sim.load=sim.load?0:30;render();});
$('disconnect').addEventListener('click',()=>{sim.trip('sensor');render();});$('stale').addEventListener('click',()=>{sim.trip('stale');render();});$('clearFault').addEventListener('click',()=>{sim.clearFault();render();});
$('export').addEventListener('click',()=>{const headers=['csvTime','csvPressure','csvTarget','csvValve'].map(t);const csv=[headers.join(','),...sim.history.map(p=>[p.time,p.pressure,p.setpoint,p.valve].map(n=>n.toFixed(4)).join(','))].join('\n');const url=URL.createObjectURL(new Blob(['\uFEFF'+csv],{type:'text/csv;charset=utf-8'}));const link=document.createElement('a');link.href=url;link.download='pressure-lab-'+lang+'.csv';link.click();setTimeout(()=>URL.revokeObjectURL(url),1000);});
setInterval(()=>{if(running&&!document.hidden){sim.step();render();}},100);
setLanguage(lang);
const base='https://api.github.com/repos/brunnojob/pid-pressure-control';
async function get(path){const response=await fetch(base+path,{headers:{Accept:'application/vnd.github+json'},signal:AbortSignal.timeout(10000)});if(!response.ok)throw new Error(String(response.status));return response.json();}
Promise.allSettled([get('/commits?per_page=1'),get('/actions/runs?branch=main&per_page=1')]).then(results=>{const commit=results[0].status==='fulfilled'?results[0].value[0]:null;const run=results[1].status==='fulfilled'?results[1].value.workflow_runs?.[0]:null;repoData={sha:commit?.sha,date:commit?.commit?.committer?.date,run};apiState=results.every(r=>r.status==='fulfilled')?'apiOk':'apiError';renderRepo();});
