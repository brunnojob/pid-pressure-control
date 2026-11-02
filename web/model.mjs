export const defaults = Object.freeze({kp:1.8,ki:0.5,kd:0.1,derivativeTau:0.08,integralLimit:40,outputMin:0,outputMax:100});
const clamp=(x,a,b)=>Math.min(b,Math.max(a,x));
export class PID {
  constructor(config={}) {this.config={...defaults,...config};this.reset();}
  reset(){this.integral=0;this.filteredDerivative=0;this.previousMeasurement=0;this.initialized=false;}
  update(setpoint,measurement,dt){
    if(![setpoint,measurement,dt].every(Number.isFinite)||dt<=0||dt>2)throw new Error('invalid_pid_sample');
    const c=this.config,error=setpoint-measurement;
    const derivative=this.initialized?-(measurement-this.previousMeasurement)/dt:0;
    const alpha=c.derivativeTau===0?1:dt/(c.derivativeTau+dt);
    this.filteredDerivative+=alpha*(derivative-this.filteredDerivative);
    const integral=clamp(this.integral+error*dt,-c.integralLimit,c.integralLimit);
    const unconstrained=c.kp*error+c.ki*integral+c.kd*this.filteredDerivative;
    const output=clamp(unconstrained,c.outputMin,c.outputMax);
    if(!(unconstrained>c.outputMax&&error>0)&&!(unconstrained<c.outputMin&&error<0))this.integral=integral;
    this.previousMeasurement=measurement;this.initialized=true;return output;
  }
}
export class Simulation {
  constructor(){this.pid=new PID();this.reset();}
  reset(){this.pid.reset();this.time=0;this.pressure=40;this.valve=0;this.setpoint=120;this.load=0;this.fault=null;this.history=[];this.record();}
  trip(kind){this.fault=kind;this.valve=0;}
  clearFault(){this.fault=null;this.pid.reset();}
  step(dt=0.1){
    this.valve=this.fault?0:this.pid.update(this.setpoint,this.pressure,dt);
    // Illustrative first-order plant, not an identified physical installation.
    const equilibrium=2.5*this.valve-this.load;
    this.pressure=clamp(equilibrium+(this.pressure-equilibrium)*Math.exp(-dt/4),0,250);
    this.time+=dt;this.record();
  }
  record(){this.history.push({time:this.time,pressure:this.pressure,setpoint:this.setpoint,valve:this.valve});if(this.history.length>600)this.history.shift();}
}
