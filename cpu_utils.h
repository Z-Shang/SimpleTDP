/*
* SimpleTDP
* Copyright (C) 2024 Z-Shang
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

#include <filesystem>
#include <vector>
#include <tuple>
#include <string>
#include <cstdint>
#include <cstddef>

#include "ryzenadj.h"

namespace cpu_utils {

struct CPUState {

  void init();
  void setScalingGovernor(const std::string &);
  void setEPP(const std::string &);

  std::vector<std::tuple<std::filesystem::path, bool>> cpus;
  std::string scaling_governor;
  std::vector<std::string> scaling_available_governors;
  std::string epp;
  std::vector<std::string> epp_available_options;
  
};

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
