#include "Input_State.hpp"

#include <cstdio>

void Input_State::new_frame_reset() {
    this->left_clicked     = false;
    this->right_clicked    = false;
    this->middle_clicked   = false;
    this->scroll_wheel_dir = 0;
    this->mouse_drag_xrel  = 0;
    this->mouse_drag_yrel  = 0;
}

void Input_State::process_SDL_event(const SDL_Event &event) {
    switch (event.type) {
        case SDL_EVENT_KEY_DOWN:
            switch (event.key.key) {
                case SDLK_LSHIFT:
                    this->left_shift_down = true;
                    break;
            }
            break;

        case SDL_EVENT_KEY_UP:
            switch (event.key.key) {
                case SDLK_LSHIFT:
                    this->left_shift_down = false;
                    break;
            }
            break;

        case SDL_EVENT_MOUSE_BUTTON_DOWN:
            this->mouse_x           = event.button.x;
            this->mouse_y           = event.button.y;
            this->mouse_drag_xstart = event.button.x;
            this->mouse_drag_ystart = event.button.y;

            if (event.button.button == SDL_BUTTON_LEFT) {
                this->left_down     = true;
                this->left_clicked  = false;
                this->left_dragging = true;
            } else if (event.button.button == SDL_BUTTON_RIGHT) {
                this->right_down     = true;
                this->right_clicked  = false;
                this->right_dragging = true;
            } else if (event.button.button == SDL_BUTTON_MIDDLE) {
                this->middle_down     = true;
                this->middle_clicked  = false;
                this->middle_dragging = true;
            }
            break;

        case SDL_EVENT_MOUSE_BUTTON_UP:
            if (event.button.button == SDL_BUTTON_LEFT) {
                this->left_clicked  = this->left_down;
                this->left_down     = false;
                this->left_dragging = false;
            } else if (event.button.button == SDL_BUTTON_RIGHT) {
                this->right_clicked  = this->right_down;
                this->right_down     = false;
                this->right_dragging = false;
            } else if (event.button.button == SDL_BUTTON_MIDDLE) {
                this->middle_clicked  = this->middle_down;
                this->middle_down     = false;
                this->middle_dragging = false;
            }
            break;

        case SDL_EVENT_MOUSE_MOTION:
            this->mouse_x = event.motion.x;
            this->mouse_y = event.motion.y;

            if (this->left_dragging || this->right_dragging || this->middle_dragging) {
                this->mouse_drag_xrel += (int)event.motion.xrel;
                this->mouse_drag_yrel += (int)event.motion.yrel;
            }
            break;

        case SDL_EVENT_MOUSE_WHEEL:
            if      (event.wheel.y > 0) { this->scroll_wheel_dir = -1; }
            else if (event.wheel.y < 0) { this->scroll_wheel_dir =  1; }
            break;

        default:
            break;
    }
}
