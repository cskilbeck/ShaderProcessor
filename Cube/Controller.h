#pragma once

namespace XInput {

bool is_ready();
void cleanup();
DWORD WINAPI get_state(DWORD d, XINPUT_STATE *state);

}    // namespace XInput