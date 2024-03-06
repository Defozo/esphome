#include "tmc2130.h"
#include "esphome/core/log.h"
#include <TMCStepper.h>

namespace esphome {
namespace tmc2130 {

static const char *const TAG = "tmc2130";

TMC2130Component::TMC2130Component(uint8_t cs_pin, float r_sense)
  : driver_(cs_pin, r_sense) {}

static void IRAM_ATTR on_timer(void *arg) {
  auto step_pin = reinterpret_cast<uint8_t *>(arg);
  digitalWrite(*step_pin, !digitalRead(*step_pin)); // Toggle the step pin
}

void TMC2130Component::setup() {
  ESP_LOGD(TAG, "Setting up TMC2130...");
  pinMode(en_pin_, OUTPUT);
  pinMode(step_pin_, OUTPUT);
  pinMode(dir_pin_, OUTPUT);
  digitalWrite(en_pin_, LOW); // Enable driver by default

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
  timerAttachInterrupt(this->timer_, &on_timer, reinterpret_cast<void *>(&this->step_pin_));
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

void TMC2130Component::set_step_pin(uint8_t step_pin) {
  this->step_pin_ = step_pin;
}

void TMC2130Component::set_dir_pin(uint8_t dir_pin) {
  this->dir_pin_ = dir_pin;
}

void TMC2130Component::set_speed(int speed) {
  // Adjust timer alarm based on speed
  uint32_t timer_speed = map(speed, 0, 100, 0, 100); // Directly map speed to timer range without using max_speed_ and min_speed_
  timerAlarmWrite(this->timer_, timer_speed, true);
}

void TMC2130Component::set_direction_forward(bool forward) {
  motor_direction_ = forward;
  digitalWrite(dir_pin_, motor_direction_ ? HIGH : LOW); // Set direction
}

void TMC2130Component::enable_motor(bool enable) {
  // Enable or disable motor by controlling en_pin_
  digitalWrite(en_pin_, !enable); // LOW to enable, HIGH to disable
}

}  // namespace tmc2130
}  // namespace esphome