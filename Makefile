CC=gcc
CXX=g++

EXE=simpletdp

IMGUI_PATH=./imgui
RYZENADJ_PATH=./RyzenAdj/lib

SOURCES = main.cpp cpu_utils.cpp
SOURCES += $(IMGUI_PATH)/imgui.cpp $(IMGUI_PATH)/imgui_demo.cpp $(IMGUI_PATH)/imgui_draw.cpp $(IMGUI_PATH)/imgui_tables.cpp $(IMGUI_PATH)/imgui_widgets.cpp
SOURCES += $(IMGUI_PATH)/backends/imgui_impl_sdl2.cpp $(IMGUI_PATH)/backends/imgui_impl_opengl3.cpp
SOURCES += $(RYZENADJ_PATH)/osdep_linux.c $(RYZENADJ_PATH)/nb_smu_ops.c $(RYZENADJ_PATH)/api.c $(RYZENADJ_PATH)/cpuid.c
OBJS = $(addsuffix .o, $(basename $(notdir $(SOURCES))))

RYZENADJ_DEFS = -D_LIBRYZENADJ_INTERNAL -Dlibryzenadj_EXPORTS

CFLAGS=-I$(IMGUI_PATH) -I$(IMGUI_PATH)/backends -I$(RYZENADJ_PATH) `sdl2-config --cflags` -fPIC -fpermissive
CXXFLAGS=-std=c++20 $(CFLAGS)
LIBS=-lGL -ldl -lpci `sdl2-config --libs`

%.o:%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

%.o:$(IMGUI_PATH)/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

%.o:$(IMGUI_PATH)/backends/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

%.o:$(RYZENADJ_PATH)/%.c
	$(CC) $(CFLAGS) $(RYZENADJ_DEFS) -c -o $@ $<

$(EXE): $(OBJS)
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LIBS)

all: $(EXE)
	@echo "Built"

clean:
	rm -f $(OBJS) simpletdp
