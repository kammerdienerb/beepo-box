#pragma once

#include <SDL3/SDL.h>

struct Input_State {
    int  scroll_wheel_dir  = 0;
    bool left_shift_down   = false;
    bool left_down         = false;
    bool left_clicked      = false;
    bool left_dragging     = false;
    bool right_down        = false;
    bool right_clicked     = false;
    bool right_dragging    = false;
    bool middle_down       = false;
    bool middle_clicked    = false;
    bool middle_dragging   = false;
    int  mouse_x           = 0;
    int  mouse_y           = 0;
    int  mouse_drag_xstart = 0;
    int  mouse_drag_ystart = 0;
    int  mouse_drag_xrel   = 0;
    int  mouse_drag_yrel   = 0;

    void new_frame_reset();
    void process_SDL_event(const SDL_Event &event);
};
