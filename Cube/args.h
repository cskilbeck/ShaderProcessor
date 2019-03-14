#pragma once

struct params_t
{
    int log_level = 0;
    bool triple_buffered = false;
    bool fullscreen = false;
    bool debug = false;
    bool titlebar = false;
    DXGI_SWAP_EFFECT swap_effect = DXGI_SWAP_EFFECT_DISCARD;
    int vsync_frames = 1;
    uint32 flash_buttons = XINPUT_GAMEPAD_A;
    uint32 blank_color = 0;
    uint32 flash_color = 0xffffff;
    int window_width = 1280;
    int window_height = 720;
    int num_cubes = 128;
};

extern params_t params;

void process_arguments(LPSTR lpCmdLine);