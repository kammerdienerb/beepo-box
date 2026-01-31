#pragma once

#include <SDL3/SDL.h>

#include <map>
#include <string>

struct Texture_Manager {
    std::map<std::string, SDL_Texture*> textures;

    Texture_Manager();
    Texture_Manager(const char *assets_path);
    void add_texture(const std::string &name, SDL_Texture *texture);
    SDL_Texture *get_texture(std::string name);

    decltype(Texture_Manager::textures)::iterator begin();
    decltype(Texture_Manager::textures)::iterator end();
};
