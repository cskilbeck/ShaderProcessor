#pragma once

namespace XInput {

bool is_connected();
void cleanup();
DWORD WINAPI get_state(DWORD d, XINPUT_STATE *state);

}    // namespace XInput