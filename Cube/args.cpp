#include "stdafx.h"

params_t params;

LOG_Context("Args");

//////////////////////////////////////////////////////////////////////

bool window_size_from_string(std::string const &s, int &width, int &height)
{
    std::vector<std::string> parts;
    tokenize(s, parts, "x", tokenize_option::discard_empty);
    if(parts.size() != 2) {
        return false;
    }
    int w = atoi(parts[0].c_str());
    int h = atoi(parts[1].c_str());
    if(w < 160 || w > 3840) {
        LOG_Error("Width specified (%d) out of range, must be 160 < W < 3840", w);
        return false;
    }
    if(h < 120 || w > 2160) {
        LOG_Error("Height specified (%d) out of range, must be 120 < W < 2160", h);
        return false;
    }
    width = w;
    height = h;
    return true;
}

//////////////////////////////////////////////////////////////////////

std::map<std::string, uint32> swap_effect_names = { { "DISCARD", DXGI_SWAP_EFFECT_DISCARD },
                                                    { "SEQUENTIAL", DXGI_SWAP_EFFECT_SEQUENTIAL },
                                                    { "FLIP_SEQUENTIAL", DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL },
                                                    { "FLIP_DISCARD", DXGI_SWAP_EFFECT_FLIP_DISCARD } };

//////////////////////////////////////////////////////////////////////

std::map<char, uint32> button_names = {
    { 'A', XINPUT_GAMEPAD_A },
    { 'B', XINPUT_GAMEPAD_B },
    { 'X', XINPUT_GAMEPAD_X },
    { 'Y', XINPUT_GAMEPAD_Y },
};

//////////////////////////////////////////////////////////////////////

bool button_mask_from_string(std::string const &s, uint32 &result)
{
    std::string u = str_to_upper(s);
    uint32 mask = 0;
    for(char const c : u) {
        uint32 button;
        if(!map_find(button_names, c, button)) {
            LOG_Error("Unknown button '%c'", c);
            return false;
        }
        mask |= button;
    }
    if(mask == 0) {
        LOG_Error("Button mask is empty!");
        return false;
    }
    result = mask;
    return true;
}

//////////////////////////////////////////////////////////////////////

std::string string_from_button_mask(uint32 mask)
{
    std::string r("----");
    int d = 0;
    for(auto const &kvp : button_names) {
        if((mask & kvp.second) != 0) {
            r[d] = kvp.first;
        }
        d += 1;
    }
    return r;
}

//////////////////////////////////////////////////////////////////////
// nab the command line args
// it doesn't complain about unrecognized arguments
// it doesn't show a usage message

void process_arguments(LPSTR lpCmdLine)
{
    LOG_Context("Args");

    wchar const *env_var_name = L"dx11_blank_command_line";
    char const *env_var_nameA = "dx11_blank_command_line";

    std::wstring command_line(stringToWide(CP_ACP, lpCmdLine));

    // if config.ini exists and has a command line in it, get it from there...
    std::ifstream f("config.ini", std::ifstream::in);
    std::string line;

    while(std::getline(f, line)) {
        if(str_icompare(line, env_var_nameA)) {
            size_t o = line.find('=', 0);
            if(o != std::string::npos) {
                o += 1;
                size_t len = line.size() - o;
                if(line[o] == '"') {
                    o += 1;
                    len -= 1;
                }
                line = line.substr(o, len);
                command_line = stringToWide(CP_ACP, line.c_str());
                break;
            }
        }
    }

    // NOT THE ENV VAR THING, USE CONFIG.INI SO YOU
    // CAN CREATE TAILORED CLIENTS IN THE DASHBOARD

    // // maybe get it from the env var
    // std::vector<wchar> buffer;
    // uint32 len = GetEnvironmentVariableW(env_var_name, null, 0);
    // if(len != 0 && GetLastError() != ERROR_ENVVAR_NOT_FOUND) {
    //    buffer.resize(len);
    //    uint32 got = GetEnvironmentVariableW(env_var_name, buffer.data(), len);
    //    if(got == len - 1) {
    //        command_line = buffer.data();
    //    }
    //}

    int argc;
    LPWSTR *s = CommandLineToArgvW(command_line.c_str(), &argc);

    // convert args to utf8
    std::vector<std::string> utf8_arg_strings;
    std::vector<char const *> utf8_argv;
    for(int i = 0; i < argc; ++i) {
        utf8_arg_strings.push_back(wideToString(CP_UTF8, s[i]));
    }
    for(int i = 0; i < argc; ++i) {
        utf8_argv.push_back(utf8_arg_strings[i].c_str());
    }

    // parse into arguments
    argh::parser args({ "swap_effect", "vsync_frames", "flash_buttons", "blank_color", "flash_color", "window_size", "log_level", "num_cubes" });
    args.parse(argc, utf8_argv.data());

    // log level first!
    args("log_level") >> params.log_level;
    Log_SetLevel(params.log_level);

    if(params.log_level > 0) {
        Log_SetOutputFiles(L"log.txt");
    }

    LOG_Debug("Log level set to %d (ended up as %d)", params.log_level, Logging::log_level);

    // process the arguments

    params.debug = args["debug"];
    params.triple_buffered = args["triple_buffered"];
    params.fullscreen = args["fullscreen"];
    params.titlebar = args["titlebar"];

    auto swap_effect_arg = args("swap_effect");    // bool operator on a stream returns false if it's empty/bad
    auto vsync_arg = args("vsync");
    auto flash_buttons_arg = args("flash_buttons");
    auto blank_color_arg = args("blank_color");
    auto flash_color_arg = args("flash_color");
    auto window_size_arg = args("window_size");
    auto num_cubes_arg = args("num_cubes");

    if(swap_effect_arg) {
        if(!map_find(swap_effect_names, str_to_upper(swap_effect_arg.str()), params.swap_effect)) {
            auto s = map_reverse_find(swap_effect_names, params.swap_effect);
            LOG_Error("Bad swap effect \"%s\", defaulting to %s", swap_effect_arg.str().c_str(), s.c_str());
        }
    }

    if(vsync_arg) {
        vsync_arg >> params.vsync_frames;    // shifting an istringstream into an int does an atoi I guess
        if(params.vsync_frames < 0 || params.vsync_frames > 60) {
            LOG_Warn("Warning, %d is not a good value for vsync_frames, defaulting to 1", params.vsync_frames);
            params.vsync_frames = 1;
        }
    }

    if(flash_buttons_arg) {
        if(!button_mask_from_string(flash_buttons_arg.str(), params.flash_buttons)) {
            LOG_Error("Bad button mask \"%s\", defaulting to ABXY", flash_buttons_arg.str().c_str());
        }
    }

    if(blank_color_arg) {
        if(!rgb24_from_string(blank_color_arg.str(), params.blank_color)) {
            LOG_Warn("Bad color for %s: \"%s\", defaulting to %06x", "blank_color", blank_color_arg.str().c_str(), params.blank_color);
        }
    }

    if(flash_color_arg) {
        if(!rgb24_from_string(flash_color_arg.str(), params.flash_color)) {
            LOG_Warn("Bad color for %s: \"%s\", defaulting to %06x", "flash_color", flash_color_arg.str().c_str(), params.flash_color);
        }
    }

    if(window_size_arg) {
        if(!window_size_from_string(window_size_arg.str(), params.window_width, params.window_height)) {
            LOG_Warn("Bad argument for %s: \"%s\", defaulting to %dx%d", "window_size", window_size_arg.str().c_str(), params.window_width, params.window_height);
        }
    }

    if(num_cubes_arg) {
        int cubes;
        num_cubes_arg >> cubes;
        if(cubes < 1 || cubes > 10000) {
            LOG_Warn("Warning, num_cubes (%d) out of range (1..10000), defaulting to %d", cubes, params.num_cubes);
        } else {
            params.num_cubes = cubes;
        }
    }

    string swap_effect_name = map_reverse_find(swap_effect_names, params.swap_effect);
    string buttons_string = string_from_button_mask(params.flash_buttons);

    LOG_Verbose("Fullscreen = %s", params.fullscreen ? "true" : "false");
    LOG_Verbose("TripleBuffered = %s", params.triple_buffered ? "true" : "false");
    LOG_Verbose("DebugMode = %s", params.debug ? "true" : "false");
    LOG_Verbose("Swap Effect is %s", swap_effect_name.c_str());
    LOG_Verbose("VSync = %d", params.vsync_frames);
    LOG_Verbose("FlashButtons = %s", buttons_string.c_str());
    LOG_Verbose("BlankColor = %06x", params.blank_color);
    LOG_Verbose("FlashColor = %06x", params.flash_color);
    LOG_Verbose("Window Size = %dx%d", params.window_width, params.window_height);
}
