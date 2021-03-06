/*
  temperature.h - temperature controller
  Part of Marlin

  Copyright (c) 2011 Erik van der Zalm

  Grbl is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Grbl is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with Grbl.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef temperature_h
#define temperature_h

#include "Marlin.h"
#include "planner.h"
#ifdef PID_ADD_EXTRUSION_RATE
  #include "stepper.h"
#endif

#define constrainmax(amt,high) ((amt)>(high)?(high):(amt))

// public functions
void tp_init();  //initialize the heating
void manage_heater(); //it is critical that this is called periodically.

#define EXTRUDER_PREHEAT     1
#define EXTRUDER_STANDBY     4
#define EXTRUDER_AUTOSTANDBY 16
extern uint8_t temperature_state;

// low level conversion routines
// do not use these routines and variables outside of temperature.cpp
extern uint16_t target_temperature[EXTRUDERS];
extern int8_t target_temperature_diff[EXTRUDERS];
extern float current_temperature[EXTRUDERS];
extern unsigned long extruder_lastused[EXTRUDERS];
#if TEMP_SENSOR_BED != 0
extern uint16_t target_temperature_bed;
extern int8_t target_temperature_bed_diff;
extern float current_temperature_bed;
#endif
#ifdef TEMP_SENSOR_1_AS_REDUNDANT
  extern float redundant_temperature;
#endif

#ifdef PIDTEMP
  extern float Kp,Ki,Kd;
//  #ifdef PID_ADD_EXTRUSION_RATE
//  extern float Kc;
//  #endif // PID_ADD_EXTRUSION_RATE
  float scalePID_i(float i);
  float scalePID_d(float d);
  float unscalePID_i(float i);
  float unscalePID_d(float d);

#endif
#if defined(PIDTEMPBED) && (TEMP_SENSOR_BED != 0)
  extern float bedKp,bedKi,bedKd;
#endif

#if defined(BABYSTEPPING)
  extern volatile int babystepsTodo[3];
#endif

//high level conversion routines, for use outside of temperature.cpp
//inline so that there is no performance decrease.
//deg=degreeCelsius

FORCE_INLINE float degHotend(uint8_t extruder) {
  return current_temperature[extruder];
}

#if TEMP_SENSOR_BED != 0
FORCE_INLINE float degBed() {
  return current_temperature_bed;
}

FORCE_INLINE int degTargetBed() {
  return target_temperature_bed + target_temperature_bed_diff;
}

FORCE_INLINE bool isHeatingBed() {
  return target_temperature_bed + target_temperature_bed_diff > current_temperature_bed;
}

FORCE_INLINE bool isCoolingBed() {
  return target_temperature_bed + target_temperature_bed_diff < current_temperature_bed;
}
#endif // TEMP_SENSOR_BED

FORCE_INLINE int degTargetHotend(uint8_t extruder) {
  return target_temperature[extruder]+target_temperature_diff[extruder];
}

FORCE_INLINE void setTargetHotend(const uint16_t &celsius, uint8_t extruder) {
  target_temperature[extruder] = constrainmax(celsius, HEATER_0_MAXTEMP - 15);
}

FORCE_INLINE void setTargetBed(const uint16_t &celsius)
{
#ifdef BED_MAXTEMP
  target_temperature_bed = constrainmax(celsius, BED_MAXTEMP - 15);
#else
  target_temperature_bed = celsius;
#endif
}

FORCE_INLINE bool isHeatingHotend(uint8_t extruder){
  return degTargetHotend(extruder) > current_temperature[extruder];
}

FORCE_INLINE bool isCoolingHotend(uint8_t extruder) {
  return degTargetHotend(extruder) < current_temperature[extruder];
}

#define degHotend0() degHotend(0)
#define degTargetHotend0() degTargetHotend(0)
#define setTargetHotend0(_celsius) setTargetHotend((_celsius), 0)
#define isHeatingHotend0() isHeatingHotend(0)
#define isCoolingHotend0() isCoolingHotend(0)
#if EXTRUDERS > 1
#define degHotend1() degHotend(1)
#define degTargetHotend1() degTargetHotend(1)
#define setTargetHotend1(_celsius) setTargetHotend((_celsius), 1)
#define isHeatingHotend1() isHeatingHotend(1)
#define isCoolingHotend1() isCoolingHotend(1)
#else
#define setTargetHotend1(_celsius) do{}while(0)
#endif
#if EXTRUDERS > 2
#define degHotend2() degHotend(2)
#define degTargetHotend2() degTargetHotend(2)
#define setTargetHotend2(_celsius) setTargetHotend((_celsius), 2)
#define isHeatingHotend2() isHeatingHotend(2)
#define isCoolingHotend2() isCoolingHotend(2)
#else
#define setTargetHotend2(_celsius) do{}while(0)
#endif
#if EXTRUDERS > 3
#error Invalid number of extruders
#endif

#define AUTOTUNE_OK            0x01
#define AUTOTUNE_BAD_EXTRUDER  0x02
#define AUTOTUNE_TEMP_HIGH     0x04
#define AUTOTUNE_TIMEOUT       0x08
#define AUTOTUNE_ABORT         0x10

typedef bool (*autotuneFunc_t) (uint8_t state, uint8_t cycle, float kp, float ki, float kd);

int getHeaterPower(int heater);
void disable_heater();
void setWatch();
void updatePID();

FORCE_INLINE void autotempShutdown(){
 #ifdef AUTOTEMP
 if(autotemp_enabled)
 {
  autotemp_enabled=false;
  if(degTargetHotend(active_extruder)>autotemp_min)
    setTargetHotend(0,active_extruder);
 }
 #endif
}

void PID_autotune(float temp, int extruder, int ncycles, autotuneFunc_t pCallback = NULL);

void set_maxtemp(uint8_t e, int maxTemp);
int get_maxtemp(uint8_t e);

#endif

