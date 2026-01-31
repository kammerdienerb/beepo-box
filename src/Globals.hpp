#pragma once

#include <SDL3/SDL.h>
#include "Input_State.hpp"
#include "Texture_Manager.hpp"
#include "Editor.hpp"

extern SDL_Renderer    *renderer;
extern SDL_Window      *window;
extern Input_State      input_state;
extern Texture_Manager  texture_manager;
extern Editor           editor;
