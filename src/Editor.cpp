#include "Editor.hpp"
#include "Globals.hpp"

#include "imgui.h"
#include "imgui_stdlib.h"

#include <cmath>
#include <algorithm>
#include <filesystem>

void Editor::frame() {
    ImGuiIO& io = ImGui::GetIO();

    this->window_hovered = false;

    constexpr ImGuiWindowFlags titlebar_flags =   ImGuiWindowFlags_NoTitleBar
                                                | ImGuiWindowFlags_NoBackground
                                                | ImGuiWindowFlags_NoCollapse
                                                | ImGuiWindowFlags_NoResize
                                                | ImGuiWindowFlags_NoMove
                                                | ImGuiWindowFlags_NoScrollbar
                                                | ImGuiWindowFlags_NoBringToFrontOnFocus
                                                | ImGuiWindowFlags_NoSavedSettings
                                                | ImGuiWindowFlags_NoScrollWithMouse
                                                | ImGuiWindowFlags_MenuBar;

    ImGui::SetNextWindowPos({ 0, 0 });
    ImGui::SetNextWindowSize({ io.DisplaySize.x, 0 });
    ImGui::SetNextWindowBgAlpha(0.0);
    ImGui::Begin("Editor", NULL, titlebar_flags);

    if (ImGui::BeginMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            this->window_hovered = true;
            if (ImGui::MenuItem("Load Level...", "Ctrl+O")) {
                this->show_level_chooser_modal = true;
            }
            if (ImGui::MenuItem("Save Level...", "Ctrl+S")) {
                if (this->level_path.size() > 0) {
                    this->level.save(this->level_path);
                } else {
                    this->show_level_save_modal = true;
                    this->level_save_path = "./levels/";
                    this->level_save_error = "";
                }
            }
            if (ImGui::MenuItem("Save Level As...", "Ctrl+Shift+S")) {
                this->show_level_save_modal = true;
                this->level_save_path = this->level_path;
                this->level_save_error = "";
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("View")) {
            this->window_hovered = true;
            if (ImGui::MenuItem("Tile Editor", "Ctrl+T")) {
                this->show_tiles_window = true;
            }
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }

    if (this->show_level_chooser_modal) {
        this->window_hovered = true;

        if (!ImGui::IsPopupOpen("Choose level")) {
            ImGui::OpenPopup("Choose level");

            this->level_paths.clear();

            std::filesystem::path dir_path("./levels");

            if (std::filesystem::exists(dir_path) && std::filesystem::is_directory(dir_path)) {
                for (const auto& entry : std::filesystem::recursive_directory_iterator(dir_path)) {
                    if (entry.is_regular_file()) {
                        std::string name = entry.path().string();
                        this->level_paths.emplace_back(std::move(name));
                    }
                }
            }
            this->level_path_selected_idx = 0;
        }

        ImGui::SetNextWindowPos({ io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f }, ImGuiCond_Always, { 0.5f,0.5f });

        if (ImGui::BeginPopupModal("Choose level", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
            bool level_chosen = false;

            if (ImGui::BeginListBox("##level-list", { 500, 0 })) {
                for (int i = 0; i < (int)this->level_paths.size(); i += 1) {
                    bool is_selected = (i == this->level_path_selected_idx);
                    if (ImGui::Selectable(this->level_paths[i].c_str(), is_selected)) {
                        this->level_path_selected_idx = i;
                    }
                    if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0)) {
                        level_chosen = true;
                    }

                    if (is_selected) { ImGui::SetItemDefaultFocus(); }
                }
                ImGui::EndListBox();
            }

            if (ImGui::Button("Load") && this->level_paths.size() > 0) {
                level_chosen = true;
            }

            ImGui::SameLine();

            if (ImGui::Button("Cancel") || level_chosen) {
                if (level_chosen) {
                    this->level_path = this->level_paths[this->level_path_selected_idx];
                    this->level.load(this->level_path);
                }

                ImGui::CloseCurrentPopup();
                this->show_level_chooser_modal = false;
            }

            ImGui::EndPopup();
        }
    }

    if (this->show_level_save_modal) {
        this->window_hovered = true;

        if (!ImGui::IsPopupOpen("Save level")) {
            ImGui::OpenPopup("Save level");
        }

        ImGui::SetNextWindowPos({ io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f }, ImGuiCond_Always, { 0.5f,0.5f });

        if (ImGui::BeginPopupModal("Save level", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
            ImGui::Text("Save level to:");
            ImGui::SameLine();

            bool commit = false;
            ImGui::InputText("##save-to", &this->level_save_path);

            if (ImGui::IsItemDeactivatedAfterEdit()) {
                commit = true;
            }

            if (this->level_save_error.size()) {
                ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255,0,0,255));
                ImGui::TextWrapped("%s", this->level_save_error.c_str());
                ImGui::PopStyleColor();
            }

            if (ImGui::Button("Save")) {
                commit = true;
            }

            ImGui::SameLine();

            if (ImGui::Button("Cancel")) {
                ImGui::CloseCurrentPopup();
                this->show_level_save_modal = false;
            }

            if (commit) {
                std::filesystem::path path(this->level_save_path);

                if (std::filesystem::exists(path) && std::filesystem::is_directory(path)) {
                    this->level_save_error = "'" + this->level_save_path + "' is a directory!";
                } else {
                    this->level.save(this->level_save_path);
                    this->level_path = this->level_save_path;

                    ImGui::CloseCurrentPopup();
                    this->show_level_save_modal = false;
                }
            }

            ImGui::EndPopup();
        }
    }

    if (this->show_tiles_window && ImGui::Begin("Tile Editor", &this->show_tiles_window)) {
        if (ImGui::IsWindowHovered()) {
            this->window_hovered = true;
        }

        if (ImGui::BeginTable("MyTable", 1 + Level::LAYER_COUNT)) {
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::Text("Select layer:");
            ImGui::TableSetColumnIndex(1);
            if (ImGui::RadioButton("Ground", this->active_layer == Level::Layer::GROUND)) { this->active_layer = Level::Layer::GROUND;       }
            ImGui::TableSetColumnIndex(2);
            if (ImGui::RadioButton("Cover", this->active_layer == Level::Layer::GROUND_COVER)) { this->active_layer = Level::Layer::GROUND_COVER; }
            ImGui::TableSetColumnIndex(3);
            if (ImGui::RadioButton("Solid", this->active_layer == Level::Layer::SOLID)) { this->active_layer = Level::Layer::SOLID;        }
            ImGui::TableSetColumnIndex(4);
            if (ImGui::RadioButton("Top", this->active_layer == Level::Layer::TOP)) { this->active_layer = Level::Layer::TOP;          }
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::Text("Hide:");
            ImGui::TableSetColumnIndex(1);
            ImGui::Checkbox("##ground-hide", &this->hide_layer[(int)Level::Layer::GROUND]);
            ImGui::TableSetColumnIndex(2);
            ImGui::Checkbox("##ground-cover-hide", &this->hide_layer[(int)Level::Layer::GROUND_COVER]);
            ImGui::TableSetColumnIndex(3);
            ImGui::Checkbox("##solid-hide", &this->hide_layer[(int)Level::Layer::SOLID]);
            ImGui::TableSetColumnIndex(4);
            ImGui::Checkbox("##top-hide", &this->hide_layer[(int)Level::Layer::TOP]);

            ImGui::EndTable();
        }

        ImGui::Text("Grid:");
        ImGui::SameLine();
        ImGui::Checkbox("##show-grid", &this->show_grid);
        ImGui::SameLine();
        ImGui::Text("Empty Space:");
        ImGui::SameLine();
        ImGui::Checkbox("##show-empty", &this->show_empty);


        ImGui::Text("Tileset:");
        ImGui::SameLine();
        if (ImGui::BeginCombo("##tileset-selector", this->cur_tileset.size() ? this->cur_tileset.c_str() : "Choose tileset...")) {
            this->window_hovered = true;
            for (auto it : texture_manager) {
                bool is_selected = (it.first == this->cur_tileset);

                if (ImGui::Selectable(it.first.c_str(), is_selected)) {
                    this->cur_tileset = it.first;
                }

                if (is_selected) {
                    ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::EndCombo();
        }

        SDL_Texture *tileset_texture = nullptr;
        if (this->cur_tileset.size()
        &&  (tileset_texture = texture_manager.get_texture(this->cur_tileset)) != nullptr) {

            float content_height = std::max(ImGui::GetContentRegionAvail().y, 50.0f);
            float top_height     = std::clamp(content_height, 25.0f, content_height - 25.0f);

            ImGui::BeginChild("tileset", { -FLT_MIN, top_height }, ImGuiChildFlags_Borders, ImGuiWindowFlags_HorizontalScrollbar);

            if (ImGui::IsWindowHovered()) {
                this->window_hovered = true;
            }

            float top  = ImGui::GetCursorPosY();
            float left = ImGui::GetCursorPosX();

            float tileset_texture_w;
            float tileset_texture_h;
            SDL_GetTextureSize(tileset_texture, &tileset_texture_w, &tileset_texture_h);
            ImGui::Image((ImTextureID)(intptr_t)tileset_texture, ImVec2(tileset_texture_w * picker_zoom, tileset_texture_h * picker_zoom));

            ImGui::SetCursorPos(ImVec2(left, top));

            for (int x = 0; x < tileset_texture_w / Level::TILE_SIZE; x += 1) {
                for (int y = 0; y < tileset_texture_h / Level::TILE_SIZE; y += 1) {
                    ImGui::SetCursorPosY(top + (y * Level::TILE_SIZE * picker_zoom));
                    ImGui::SetCursorPosX(left + (x * Level::TILE_SIZE * picker_zoom));

                    ImGui::Dummy({ Level::TILE_SIZE * picker_zoom, Level::TILE_SIZE * picker_zoom });

                    ImVec2 p0 = ImGui::GetItemRectMin();
                    ImVec2 p1 = ImGui::GetItemRectMax();

                    ImU32 col = ImGui::IsItemHovered() ? IM_COL32(255, 0, 255, 128) : IM_COL32(255, 255, 255, 0);

                    ImDrawList* draw_list = ImGui::GetWindowDrawList();
                    draw_list->AddRectFilled(p0, p1, col);

                    if (ImGui::IsItemClicked()) {
                        active_tile.texture = tileset_texture;
                        active_tile.x       = x;
                        active_tile.y       = y;
                    }
                }
            }

            ImGui::EndChild();

            ImGui::Text("Zoom");
            ImGui::SameLine();
            if (ImGui::Button("-")) {
                this->decrease_picker_zoom();
            }
            ImGui::SameLine();
            if (ImGui::Button("+")) {
                this->increase_picker_zoom();
            }
        }

        ImGui::End();
    }
    ImGui::End();

    if (!this->window_hovered && !ImGui::IsAnyItemActive()) {
        ImGui::SetWindowFocus(NULL);
    }

    if (!io.WantCaptureKeyboard && !io.WantCaptureMouse) {
        if (input_state.scroll_wheel_dir < 0) {
            this->decrease_level_zoom();
        } else if (input_state.scroll_wheel_dir > 0) {
            this->increase_level_zoom();
        }

        if (input_state.right_dragging) {
            this->move_view(input_state.mouse_drag_xrel, input_state.mouse_drag_yrel);
        }

        if (this->show_tiles_window) {
            int tile_col = 0;
            int tile_row = 0;

            if (this->bulk_edit && !input_state.left_dragging) {
                int initial_h = this->level.get_height();
                int initial_w = this->level.get_width();
                for (int x = 0; x < this->bulk_w; x += 1) {
                    for (int y = 0; y < this->bulk_h; y += 1) {
                        if (input_state.left_shift_down) {
                            this->level.set_tile(this->active_layer, this->bulk_t + y, this->bulk_l + x, {});
                        } else if (active_tile.texture != nullptr) {
                            this->level.set_tile(this->active_layer, this->bulk_t + y, this->bulk_l + x, active_tile);
                        }

                        /* If setting the topmost or leftmost tile resizes the level, adjust the rect so that
                         * subsequent set_tile calls are based on the new origin. */
                        if (x == 0 && this->bulk_l < 0 && this->level.get_width() > initial_w) {
                            this->view_x_off += (Level::TILE_SIZE * this->level_zoom * this->bulk_l);
                            this->bulk_l = 0;
                        }
                        if (y == 0 && this->bulk_t < 0 && this->level.get_height() > initial_h) {
                            this->view_y_off += (Level::TILE_SIZE * this->level_zoom * this->bulk_t);
                            this->bulk_t = 0;
                        }
                    }
                }
            } else if (input_state.left_clicked) {
                this->xy_to_level_coords(input_state.mouse_x, input_state.mouse_y, tile_row, tile_col);

                if (input_state.left_shift_down) {
                    this->level.set_tile(this->active_layer, tile_row, tile_col, {});
                } else if (active_tile.texture != nullptr) {
                    this->level.set_tile(this->active_layer, tile_row, tile_col, active_tile);

                    if (tile_col < 0) {
                        this->view_x_off += (Level::TILE_SIZE * this->level_zoom * tile_col);
                    }
                    if (tile_row < 0) {
                        this->view_y_off += (Level::TILE_SIZE * this->level_zoom * tile_row);
                    }
                }

            } else if (input_state.left_dragging) {
                this->xy_to_level_coords(input_state.mouse_x, input_state.mouse_y, tile_row, tile_col);

                int anchor_tile_row = 0;
                int anchor_tile_col = 0;
                this->xy_to_level_coords(input_state.mouse_drag_xstart, input_state.mouse_drag_ystart, anchor_tile_row, anchor_tile_col);

                if (anchor_tile_row <= tile_row) {
                    this->bulk_t = anchor_tile_row;
                    this->bulk_h = tile_row - anchor_tile_row + 1;
                } else {
                    this->bulk_t = tile_row;
                    this->bulk_h = anchor_tile_row - tile_row + 1;
                }

                if (anchor_tile_col <= tile_col) {
                    this->bulk_l = anchor_tile_col;
                    this->bulk_w = tile_col - anchor_tile_col + 1;
                } else {
                    this->bulk_l = tile_col;
                    this->bulk_w = anchor_tile_col - tile_col + 1;
                }

                if (this->bulk_h > 1 || this->bulk_w > 1) {
                    this->bulk_edit = true;
                }
            }

            if (!input_state.left_dragging) {
                this->bulk_edit = false;
            }
        }
    }
}

void Editor::render() {
    if (this->show_empty) {
        SDL_SetRenderDrawColor(renderer, 255, 0, 255, 255);
        SDL_FRect bg_rect = { (float)view_x_off,
                        (float)view_y_off,
                        Level::TILE_SIZE * this->level.get_width() * this->level_zoom,
                        Level::TILE_SIZE * this->level.get_height() * this->level_zoom
        };
        SDL_RenderFillRect(renderer, &bg_rect);
    }

    if (!this->hide_layer[(int)Level::Layer::GROUND]) {
        this->level.render_layer(Level::Layer::GROUND,
                                 this->view_x_off, this->view_y_off,
                                 this->level_zoom);
    }
    if (!this->hide_layer[(int)Level::Layer::GROUND_COVER]) {
        this->level.render_layer(Level::Layer::GROUND_COVER,
                                 this->view_x_off, this->view_y_off,
                                 this->level_zoom);
    }
    if (!this->hide_layer[(int)Level::Layer::SOLID]) {
        this->level.render_layer(Level::Layer::SOLID,
                                 this->view_x_off, this->view_y_off,
                                 this->level_zoom);
    }
    if (!this->hide_layer[(int)Level::Layer::TOP]) {
        this->level.render_layer(Level::Layer::TOP,
                                 this->view_x_off, this->view_y_off,
                                 this->level_zoom);
    }

    if (this->show_grid) {
        int ww;
        int wh;
        SDL_GetWindowSize(window, &ww, &wh);

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 128);

        for (int x = this->view_x_off % (int)(Level::TILE_SIZE * this->level_zoom); x < 1 + ww; x += Level::TILE_SIZE * this->level_zoom) {
            SDL_RenderLine(renderer, x, 0, x, wh);
        }

        for (int y = this->view_y_off % (int)(Level::TILE_SIZE * this->level_zoom); y < 1 + wh; y += Level::TILE_SIZE * this->level_zoom) {
            SDL_RenderLine(renderer, 0, y, ww, y);
        }
    }

    if (this->show_tiles_window) {
        if (input_state.left_shift_down) {
            constexpr float scale_up = 4.0;
            float scale_x;
            float scale_y;
            SDL_GetRenderScale(renderer, &scale_x, &scale_y);
            SDL_SetRenderScale(renderer, scale_x * scale_up, scale_y * scale_up);
            SDL_SetRenderDrawColor(renderer, 255, 0, 0, 128);

            auto draw_x_on_tile = [&](int x, int y) {
                float x1 = (float)(x - ((x - (this->view_x_off % (int)(Level::TILE_SIZE * this->level_zoom))) % (int)(Level::TILE_SIZE * this->level_zoom)));
                float y1 = (float)(y - ((y - (this->view_y_off % (int)(Level::TILE_SIZE * this->level_zoom))) % (int)(Level::TILE_SIZE * this->level_zoom)));
                float x2 = x1 + ((float)Level::TILE_SIZE * this->level_zoom * 0.95);
                float y2 = y1 + ((float)Level::TILE_SIZE * this->level_zoom * 0.95);

                x1 /= scale_up;
                y1 /= scale_up;
                x2 /= scale_up;
                y2 /= scale_up;

                SDL_RenderLine(renderer, x1, y1, x2, y2);
                float tmp = y1; y1 = y2; y2 = tmp;
                SDL_RenderLine(renderer, x1, y1, x2, y2);
            };

            if (this->bulk_edit) {
                for (int x = 0; x < this->bulk_w; x += 1) {
                    for (int y = 0; y < this->bulk_h; y += 1) {
                        draw_x_on_tile(this->view_x_off + ((this->bulk_l + x) * Level::TILE_SIZE * this->level_zoom),
                                       this->view_y_off + ((this->bulk_t + y) * Level::TILE_SIZE * this->level_zoom));
                    }
                }
            } else {
                draw_x_on_tile(input_state.mouse_x, input_state.mouse_y);
            }

            SDL_SetRenderScale(renderer, scale_x, scale_y);
        } else {
            if (this->active_tile.texture != nullptr) {
                if (this->bulk_edit) {
                    SDL_FRect src = { (float)Level::TILE_SIZE * this->active_tile.x, (float)Level::TILE_SIZE * this->active_tile.y, (float)Level::TILE_SIZE, (float)Level::TILE_SIZE };

                    for (int x = 0; x < this->bulk_w; x += 1) {
                        for (int y = 0; y < this->bulk_h; y += 1) {
                            SDL_FRect dst = {
                                (float)(this->view_x_off + ((this->bulk_l + x) * Level::TILE_SIZE * this->level_zoom)),
                                (float)(this->view_y_off + ((this->bulk_t + y) * Level::TILE_SIZE * this->level_zoom)),
                                (float)Level::TILE_SIZE * this->level_zoom,
                                (float)Level::TILE_SIZE * this->level_zoom
                            };
                            SDL_RenderTexture(renderer, this->active_tile.texture, &src, &dst);
                        }
                    }
                } else if (!this->window_hovered) {
                    SDL_FRect src = { (float)Level::TILE_SIZE * this->active_tile.x, (float)Level::TILE_SIZE * this->active_tile.y, (float)Level::TILE_SIZE, (float)Level::TILE_SIZE };
                    SDL_FRect dst = {
                        (float)(input_state.mouse_x - ((input_state.mouse_x - (this->view_x_off % (int)(Level::TILE_SIZE * this->level_zoom))) % (int)(Level::TILE_SIZE * this->level_zoom))),
                        (float)(input_state.mouse_y - ((input_state.mouse_y - (this->view_y_off % (int)(Level::TILE_SIZE * this->level_zoom))) % (int)(Level::TILE_SIZE * this->level_zoom))),
                        (float)Level::TILE_SIZE * this->level_zoom,
                        (float)Level::TILE_SIZE * this->level_zoom
                    };
                    SDL_RenderTexture(renderer, this->active_tile.texture, &src, &dst);
                }
            }
        }
    }
}

void Editor::increase_level_zoom() {
    if (this->level_zoom < 16) {
        this->level_zoom *= 2;
    }
}
void Editor::decrease_level_zoom() {
    if (this->level_zoom > 0.25) {
        this->level_zoom /= 2;
    }
}
void Editor::increase_picker_zoom() {
    if (this->picker_zoom < 16) {
        this->picker_zoom *= 2;
    }
}
void Editor::decrease_picker_zoom() {
    if (this->picker_zoom > 0.25) {
        this->picker_zoom /= 2;
    }
}

void Editor::move_view(int x, int y) {
    this->view_x_off += x;
    this->view_y_off += y;
}

void Editor::xy_to_level_coords(int x, int y, int &row, int &col) {
    float col_f = (x - this->view_x_off) / (Level::TILE_SIZE * this->level_zoom);
    if (col_f < 0) {
        col_f = floor(col_f);
    }
    col = (int)col_f;

    float row_f = (y - this->view_y_off) / (Level::TILE_SIZE * this->level_zoom);
    if (row_f < 0) {
        row_f = floor(row_f);
    }
    row = (int)row_f;
}
