#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_opengl3.h"
#include <SDL.h>
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <SDL_opengles2.h>
#else
#include <SDL_opengl.h>
#endif
#include "ryzenadj.h"
#include "cpu_utils.h"

#include <deque>
#include <iostream>

#define REC_COUNT 120
#define BOOST_PATH "/sys/devices/system/cpu/cpufreq/boost"
#define PSTATE_BOOST_PATH "/sys/devices/system/cpu/amd_pstate/cpb_boost"
#define AMD_PSTATE_PATH "/sys/devices/system/cpu/amd_pstate/status"
#define AMD_SMT_PATH "/sys/devices/system/cpu/smt/control"

int main(){
  // Setup SDL
  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0)
  {
    printf("Error: %s\n", SDL_GetError());
    return -1;
  }
  const char* glsl_version = "#version 130";
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
  // Create window with graphics context
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
  SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
  SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
  SDL_Window* window = SDL_CreateWindow("SimpleTDP", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 384, 720, window_flags);
  if (window == nullptr)
  {
    printf("Error: SDL_CreateWindow(): %s\n", SDL_GetError());
    return -1;
  }

  SDL_GLContext gl_context = SDL_GL_CreateContext(window);
  SDL_GL_MakeCurrent(window, gl_context);
  SDL_GL_SetSwapInterval(1); // Enable vsync


  // Setup Dear ImGui context
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO();
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

  // Setup Platform/Renderer backends
  ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
  ImGui_ImplOpenGL3_Init(glsl_version);

  bool done = false;
  bool show_demo_window = false;

  cpu_utils::RyzenState rs{};

  std::deque<float> stapm_rec;
  std::deque<float> stapm_fast_rec;
  std::deque<float> stapm_slow_rec;
  std::deque<float> apu_slow_rec;

  auto getter = [](void * data, int idx) -> float {
    auto dp = reinterpret_cast<std::deque<float>*> (data);
    if (idx < dp->size()){
      return (*dp)[idx];
    }
    return 0;
  };

  bool smtEnabled = true;
  bool boostEnabled = true;

  bool showDetailOverview = false;

  while(!done){

    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
      ImGui_ImplSDL2_ProcessEvent(&event);
      if (event.type == SDL_QUIT)
        done = true;
      if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(window))
        done = true;
    }

    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();

    static ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse;

    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->Pos);
    ImGui::SetNextWindowSize(viewport->Size);

    rs.tick();
    if (stapm_rec.size() > REC_COUNT) {
      stapm_rec.pop_front();
    }
    if (stapm_fast_rec.size() > REC_COUNT) {
      stapm_fast_rec.pop_front();
    }
    if (stapm_slow_rec.size() > REC_COUNT) {
      stapm_slow_rec.pop_front();
    }
    if (apu_slow_rec.size() > REC_COUNT) {
      apu_slow_rec.pop_front();
    }
    stapm_rec.push_back(rs.stapm_value);
    stapm_fast_rec.push_back(rs.stapm_fast_value);
    stapm_slow_rec.push_back(rs.stapm_slow_value);
    apu_slow_rec.push_back(rs.apu_slow_value);

    ImGui::Begin("SimpleTDP", &done, flags);

    ImGui::SeparatorText("Overview");

    if (!showDetailOverview){
      ImGui::Text("CPU Family: %s", rs.getFamilyName());

      ImGui::Text("STAPM Limit: %d W", rs.stapm_limit);
      ImGui::PlotLines("STAPM", getter, static_cast<void *>(&stapm_rec), 120);
      ImGui::Text("STAPM FAST Limit: %d W", rs.stapm_fast_limit);
      ImGui::PlotLines("STAPM FAST", getter, static_cast<void *>(&stapm_fast_rec), 120);
      ImGui::Text("STAPM SLOW Limit: %d W", rs.stapm_slow_limit);
      ImGui::PlotLines("STAPM SLOW", getter, static_cast<void *>(&stapm_slow_rec), 120);
    }else{
      if(ImGui::BeginTable("Detail Overview", 2, ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders)){
        ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthFixed);
        ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthFixed);
        ImGui::TableHeadersRow();
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("STAPM LIMIT");
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("%d", rs.stapm_limit);
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("STAPM VALUE");
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("%.2f", rs.stapm_value);
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("PPT LIMIT FAST");
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("%d", rs.stapm_fast_limit);
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("PPT VALUE FAST");
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("%.2f", rs.stapm_fast_value);
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("PPT LIMIT SLOW");
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("%d", rs.stapm_slow_limit);
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("PPT VALUE SLOW");
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("%.2f", rs.apu_slow_value);
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("StapmTimeConst");
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("%.2f", rs.stapm_time);
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("SlowPPTTimeConst");
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("%.2f", rs.stapm_slow_time);
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("PPT LIMIT APU");
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("%d", rs.apu_slow_limit);
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("PPT VALUE APU");
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("%.2f", rs.apu_slow_value);
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("TDC LIMIT VDD");
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("%.2f", rs.vrm_limit);
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("TDC VALUE VDD");
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("%.2f", rs.vrm_value);
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("TDC LIMIT SOC");
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("%.2f", rs.vrm_soc_limit);
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("TDC VALUE SOC");
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("%.2f", rs.vrm_soc_value);
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("EDC LIMIT VDD");
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("%.2f", rs.vrm_max_limit);
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("EDC VALUE VDD");
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("%.2f", rs.vrm_max_value);
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("EDC LIMIT SOC");
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("%.2f", rs.vrm_soc_max_limit);
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("EDC VALUE SOC");
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("%.2f", rs.vrm_soc_max_value);
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("THM LIMIT CORE");
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("%.2f", rs.core_temp_limit);
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("THM VALUE CORE");
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("%.2f", rs.core_temp_value);
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("STT LIMIT APU");
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("%.2f", rs.apu_skin_temp_limit);
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("STT VALUE APU");
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("%.2f", rs.apu_skin_temp_value);
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("STT LIMIT dGPU");
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("%.2f", rs.dgpu_skin_temp_limit);
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("STT VALUE dGPU");
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("%.2f", rs.dgpu_skin_temp_value);
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("CCLK Boost SETPOINT");
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("%.2f", rs.cclk_setpoint);
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("CCLK BUSY VALUE");
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("%.2f", rs.cclk_busy_value);
        ImGui::EndTable();
      }

    }

    ImGui::Checkbox("Show Details", &showDetailOverview);

    static int tdp = rs.stapm_limit;
    static int minTdp = 4;
    static int maxTdp = 20;
    if (minTdp > tdp) {
      tdp = minTdp;
    }
    if (maxTdp < tdp) {
      tdp = maxTdp;
    }
    ImGui::SeparatorText("TDP Controls");

    // ImGui::Checkbox("Demo Window", &show_demo_window);

    ImGui::SliderInt("TDP (Watt)", &tdp, minTdp, maxTdp);
    ImGui::SliderInt("Min TDP (Watt)", &minTdp, 4, 10);
    ImGui::SliderInt("Max TDP (Watt)", &maxTdp, 15, 60);

    static bool smt = true;
    static bool boost = true;
    ImGui::SeparatorText("CPU Options");
    ImGui::Checkbox("Enable SMT", &smt);
    ImGui::Checkbox("Enable Boost", &boost);

    ImGui::SeparatorText("Power Options");
    ImGui::Text("todo");

    ImGui::SeparatorText("GPU Options");
    ImGui::Text("todo");

    ImGui::End();
    ImGui::Render();

    // Update states
    if(tdp != rs.stapm_limit) {
      rs.setTdp(tdp);
    }
    if(smt != smtEnabled){
      // TODO: smt
      printf("toggling smt\n");
      smtEnabled = smt;
    }
    if(boost != boostEnabled){
      // TODO: boost
      printf("toggling boost\n");
      boostEnabled = boost;
    }

    // glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
    // glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    SDL_GL_SwapWindow(window);
  }
  // Cleanup
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplSDL2_Shutdown();
  ImGui::DestroyContext();

  SDL_GL_DeleteContext(gl_context);
  SDL_DestroyWindow(window);
  SDL_Quit();


  return 0;
}
