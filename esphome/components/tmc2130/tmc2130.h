#include "esphome/core/component.h"
#include "esphome/core/hal.h"
#include <TMCStepper.h>

namespace esphome {
namespace tmc2130 {

class TMC2130Component : public Component {
 public:
  void setup() override;
  void dump_config() override;
  void set_toff(uint8_t toff);
  void set_blank_time(uint8_t blank_time);
  void set_rms_current(uint16_t rms_current);
  void set_microsteps(int microsteps);
  void set_tcoolthrs(uint32_t tcoolthrs);
  void set_thigh(uint32_t thigh);
  void set_semin(uint8_t semin);
  void set_semax(uint8_t semax);
  void set_sedn(uint8_t sedn);
  void set_sgt(int8_t sgt);
  void set_direction_forward(bool forward);

 protected:
  TMC2130Stepper driver_;
  uint8_t toff_;
  uint8_t blank_time_;
  uint16_t rms_current_;
  int microsteps_;
  uint32_t tcoolthrs_;
  uint32_t thigh_;
  uint8_t semin_;
  uint8_t semax_;
  uint8_t sedn_;
  int8_t sgt_;
  bool motor_direction_{true};
};

}  // namespace tmc2130
}  // namespace esphome