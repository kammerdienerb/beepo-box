#pragma once

#include "Level.hpp"

#include <SDL3/SDL.h>

#include <string>
#include <vector>

struct Editor {
    Level                    level;
    std::string              level_path                     = "";
    int                      view_x_off                     = 0;
    int                      view_y_off                     = 19;
    float                    level_zoom                     = 4.0;
    float                    picker_zoom                    = 2.0;
    bool                     show_level_chooser_modal       = false;
    std::vector<std::string> level_paths                    = {};
    int                      level_path_selected_idx        = 0;
    bool                     show_level_save_modal          = false;
    std::string              level_save_path                = "";
    std::string              level_save_error               = "";
    bool                     show_tiles_window              = true;
    bool                     hide_layer[Level::LAYER_COUNT] = {};
    bool                     show_grid                      = true;
    bool                     show_empty                     = true;
    int                      active_layer                   = Level::LAYER_GROUND;
    Tile_Data                active_tile                    = {};
    std::string              cur_tileset                    = "";
    bool                     window_hovered                 = false;
    bool                     bulk_edit                      = false;
    int                      bulk_t                         = 0;
    int                      bulk_l                         = 0;
    int                      bulk_h                         = 0;
    int                      bulk_w                         = 0;

    void frame();
    void frame_gui_menu_bar();
    void frame_gui_level_chooser_modal();
    void frame_gui_level_save_modal();
    void frame_gui_tile_editor();
    void handle_inputs();
    void render();

    void increase_level_zoom();
    void decrease_level_zoom();
    void increase_picker_zoom();
    void decrease_picker_zoom();
    void move_view(int x, int y);

    void xy_to_level_coords(int x, int y, int &row, int &col);
};
