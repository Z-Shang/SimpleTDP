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

#include "cpu_utils.h"

#include <iostream>

namespace cpu_utils {

namespace {

static const char *family_name(ryzen_family fam)
{
  switch (fam)
  {
    case FAM_RAVEN: return "Raven";
    case FAM_PICASSO: return "Picasso";
    case FAM_RENOIR: return "Renoir";
    case FAM_CEZANNE: return "Cezanne";
    case FAM_DALI: return "Dali";
    case FAM_LUCIENNE: return "Lucienne";
    case FAM_VANGOGH: return "Vangogh";
    case FAM_REMBRANDT: return "Rembrandt";
    case FAM_PHOENIX: return "Phoenix Point";
    case FAM_HAWKPOINT: return "Hawk Point";
    case FAM_STRIXPOINT: return "Strix Point";
    default:
      break;
  }

  return "Unknown";
}

}

RyzenState::RyzenState() {
  _ryzen = init_ryzenadj();
  if(!_ryzen){
    throw "Unable to init ryzenadj";
  }
  init_table(_ryzen);
}

RyzenState::~RyzenState() {
  cleanup_ryzenadj(_ryzen);
}

void RyzenState::tick() {
  refresh_table(_ryzen);
  stapm_limit = get_stapm_limit(_ryzen);
  stapm_fast_limit = get_fast_limit(_ryzen);
  stapm_slow_limit = get_slow_limit(_ryzen);
  apu_slow_limit = get_apu_slow_limit(_ryzen);
  stapm_value = get_stapm_value(_ryzen);
  stapm_fast_value = get_fast_value(_ryzen);
  stapm_slow_value = get_slow_value(_ryzen);
  apu_slow_value = get_apu_slow_value(_ryzen);
  stapm_time = get_stapm_time(_ryzen);
  stapm_slow_time = get_slow_time(_ryzen);
  vrm_limit = get_vrm_current(_ryzen);
  vrm_value = get_vrm_current_value(_ryzen);
  vrm_soc_limit = get_vrmsoc_current(_ryzen);
  vrm_soc_value = get_vrmsoc_current_value(_ryzen);
  vrm_max_limit = get_vrmmax_current(_ryzen);
  vrm_max_value = get_vrmmax_current_value(_ryzen);
  vrm_soc_max_limit = get_vrmsocmax_current(_ryzen);
  vrm_soc_max_value = get_vrmsocmax_current_value(_ryzen);
  core_temp_limit = get_tctl_temp(_ryzen);
  core_temp_value = get_tctl_temp_value(_ryzen);
  apu_skin_temp_limit = get_apu_skin_temp_limit(_ryzen);
  apu_skin_temp_value = get_apu_skin_temp_value(_ryzen);
  dgpu_skin_temp_limit = get_dgpu_skin_temp_limit(_ryzen);
  dgpu_skin_temp_value = get_dgpu_skin_temp_value(_ryzen);
  cclk_setpoint = get_cclk_setpoint(_ryzen);
  cclk_busy_value = get_cclk_busy_value(_ryzen);
}

void RyzenState::setTdp(int tdp) {
    int fast = tdp + 2;
    set_stapm_limit(_ryzen, tdp * 1000);
    set_fast_limit(_ryzen, fast * 1000);
    set_slow_limit(_ryzen, tdp * 1000);
    set_apu_slow_limit(_ryzen, tdp * 1000);
}

const char * RyzenState::getFamilyName() const {
    return family_name(get_cpu_family(_ryzen));
}

void RyzenState::toggleMaxPerf() {
  on_max_perf = !on_max_perf;
  if (on_max_perf) {
    set_max_performance(_ryzen);
  } else {
    set_power_saving(_ryzen);
  }
}

}
