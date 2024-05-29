#pragma once

#include <cstdint>
#include <cstddef>
#include "ryzenadj.h"

namespace cpu_utils {


struct RyzenState {


  RyzenState();

  ~RyzenState();

  void tick();

  void setTdp(int tdp);
  void toggleMaxPerf();

  const char * getFamilyName() const;

  int stapm_limit;
  int stapm_fast_limit;
  int stapm_slow_limit;
  int apu_slow_limit;
  float stapm_value;
  float stapm_fast_value;
  float stapm_slow_value;
  float apu_slow_value;
  float stapm_time;
  float stapm_slow_time;
  float vrm_limit;
  float vrm_value;
  float vrm_soc_limit;
  float vrm_soc_value;
  float vrm_max_limit;
  float vrm_max_value;
  float vrm_soc_max_limit;
  float vrm_soc_max_value;
  float core_temp_limit;
  float core_temp_value;
  float apu_skin_temp_limit;
  float apu_skin_temp_value;
  float dgpu_skin_temp_limit;
  float dgpu_skin_temp_value;
  float cclk_setpoint;
  float cclk_busy_value;

  bool on_max_perf;

private:
  ryzen_access _ryzen;
};
}
