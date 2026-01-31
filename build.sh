#!/usr/bin/env bash

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"

cd ${DIR}

rm -rf build
mkdir -p build/obj

CXX=g++

SRC=src/*.cpp

SRC+=" imgui/imgui.cpp"
SRC+=" imgui/imgui_draw.cpp"
SRC+=" imgui/imgui_tables.cpp"
SRC+=" imgui/imgui_widgets.cpp"
SRC+=" imgui/backends/imgui_impl_sdl3.cpp"
SRC+=" imgui/backends/imgui_impl_sdlrenderer3.cpp"
SRC+=" imgui/misc/cpp/imgui_stdlib.cpp"

INC="-Iimgui -Iimgui/backends -Iimgui/misc/cpp"
INC+=" $(pkg-config sdl3 --cflags)"
INC+=" $(pkg-config sdl3-image --cflags)"

OPT="-O3"
OPT="-g -O0"

CPP_FLAGS="-std=c++20 -Wall -Werror -fno-omit-frame-pointer -mno-omit-leaf-frame-pointer ${OPT} ${INC}"

pids=()
for f in ${SRC}; do
    ${CXX} -c -o build/obj/$(basename ${f}).o ${f} ${CPP_FLAGS} &
    pids+=($!)
done

for pid in "${pids[@]}"; do
    wait ${pid} || exit $?
done

LD_FLAGS="$(pkg-config sdl3 --libs)"
LD_FLAGS+=" $(pkg-config sdl3-image --libs)"

${CXX} -o build/beepo-box ${CPP_FLAGS} build/obj/* ${LD_FLAGS} || exit $?
