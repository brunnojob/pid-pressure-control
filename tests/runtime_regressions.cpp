#include "controller.hpp"
#include <cassert>
#include <limits>
int main() {
 PidConfig pid{1,1,0,0,100,0.1,100};
 bool rejected=false;
 try {PressureLoop loop({0,4095,0,250},pid,std::numeric_limits<double>::quiet_NaN(),1000);} catch(...) {rejected=true;}
 assert(rejected);
 PressureLoop loop({0,4095,0,250},pid,100,1000);
 auto fault=loop.sample(10,10,2000,0.1); assert(fault.valvePercent==0);
 assert(loop.sample(10,2001,2001,0.1).status==LoopStatus::SensorFault);
 loop.reset(); assert(loop.sample(10,2002,2002,0.1).status==LoopStatus::Ready);
}
