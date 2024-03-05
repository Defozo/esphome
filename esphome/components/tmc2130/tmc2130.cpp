#include "tmc2130.h"
#include "esphome/core/log.h"
#include <TMCStepper.h>

namespace esphome {
namespace tmc2130 {

static const char *const TAG = "tmc2130";

TMC2130Component::TMC2130Component(uint8_t cs_pin, float r_sense)
  : driver_(cs_pin, r_sense) {}

void TMC2130Component::setup() {
  ESP_LOGD(TAG, "Setting up TMC2130...");
  pinMode(EN_PIN, OUTPUT);
  pinMode(STEP_PIN, OUTPUT);
  pinMode(DIR_PIN, OUTPUT);
  digitalWrite(EN_PIN, LOW); // Enable driver by default

   this->driver_.begin(); // Initiate SPI
  // Configuration values are now set from the YAML file
  this->driver_.toff(this->toff_);
  this->driver_.blank_time(this->blank_time_);
  this->driver_.rms_current(this->rms_current_); // Set motor RMS current
  this->driver_.microsteps(this->microsteps_);
  this->driver_.TCOOLTHRS(this->tcoolthrs_);
  this->driver_.THIGH(this->thigh_);
  this->driver_.semin(this->semin_);
  this->driver_.semax(this->semax_);
  this->driver_.sedn(this->sedn_);
  this->driver_.sgt(this->sgt_);

  // Setup timer
  this->timer_ = timerBegin(0, 80, true); // Use the first timer
  timerAttachInterrupt(this->timer_, &onTimer, true); // Attach the interrupt function
  timerAlarmWrite(this->timer_, 1000, true); // Set the alarm
  timerAlarmEnable(this->timer_); // Enable the alarm
}

void TMC2130Component::set_toff(uint8_t toff) { this->toff_ = toff; }
void TMC2130Component::set_blank_time(uint8_t blank_time) { this->blank_time_ = blank_time; }
void TMC2130Component::set_rms_current(uint16_t rms_current) { this->rms_current_ = rms_current; }
void TMC2130Component::set_microsteps(int microsteps) { this->microsteps_ = microsteps; }
void TMC2130Component::set_tcoolthrs(uint32_t tcoolthrs) { this->tcoolthrs_ = tcoolthrs; }
void TMC2130Component::set_thigh(uint32_t thigh) { this->thigh_ = thigh; }
void TMC2130Component::set_semin(uint8_t semin) { this->semin_ = semin; }
void TMC2130Component::set_semax(uint8_t semax) { this->semax_ = semax; }
void TMC2130Component::set_sedn(uint8_t sedn) { this->sedn_ = sedn; }
void TMC2130Component::set_sgt(int8_t sgt) { this->sgt_ = sgt; }


void TMC2130Component::loop() {
  // This method can be used to update the component state in each loop iteration
  // For example, handling dynamic speed adjustments or other runtime tasks
}

void TMC2130Component::dump_config() {
  ESP_LOGCONFIG(TAG, "TMC2130 Component Configuration:");
  ESP_LOGCONFIG(TAG, "  - Toff: %u", this->toff_);
  ESP_LOGCONFIG(TAG, "  - Blank Time: %u", this->blank_time_);
  ESP_LOGCONFIG(TAG, "  - RMS Current: %u", this->rms_current_);
  ESP_LOGCONFIG(TAG, "  - Microsteps: %d", this->microsteps_);
  ESP_LOGCONFIG(TAG, "  - TCOOLTHRS: %u", this->tcoolthrs_);
  ESP_LOGCONFIG(TAG, "  - THIGH: %u", this->thigh_);
  ESP_LOGCONFIG(TAG, "  - SEMIN: %u", this->semin_);
  ESP_LOGCONFIG(TAG, "  - SEMAX: %u", this->semax_);
  ESP_LOGCONFIG(TAG, "  - SEDN: %u", this->sedn_);
  ESP_LOGCONFIG(TAG, "  - SGT: %d", this->sgt_);
}

void TMC2130Component::set_speed(int speed) {
  // Adjust timer alarm based on speed
  if (speed > MAX_SPEED && speed < MIN_SPEED) {
    uint32_t timer_speed = map(speed, 0, 100, MIN_SPEED, MAX_SPEED); // Map speed to timer range
    timerAlarmWrite(this->timer_, timer_speed, true);
  }
}

void TMC2130Component::set_direction_forward(bool forward) {
  motor_direction_ = forward;
  digitalWrite(DIR_PIN, motor_direction_ ? HIGH : LOW); // Set direction
}

void TMC2130Component::enable_motor(bool enable) {
  // Enable or disable motor by controlling EN_PIN
  digitalWrite(EN_PIN, !enable); // LOW to enable, HIGH to disable
}

static void IRAM_ATTR onTimer() {
  digitalWrite(STEP_PIN, !digitalRead(STEP_PIN)); // Toggle the step pin
}

}  // namespace tmc2130
}  // namespace esphome