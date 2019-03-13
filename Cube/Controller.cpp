#include "stdafx.h"

LOG_Context("XInput");

namespace {

typedef DWORD(WINAPI *XInputGetState_t)(DWORD, XINPUT_STATE *);

bool opened = false;
HMODULE XInputDll = null;
XInputGetState_t XInputGetStatePtr = null;

XInputGetState_t XInputGetState_fn()
{
    if(!opened) {
        LOG_Debug("Opening xinput1_3.dll");
        XInputDll = LoadLibrary(TEXT("xinput1_3.DLL"));
        LOG_Debug("Loaded xinput1_3.dll: %p", XInputDll);
        XInputGetStatePtr = reinterpret_cast<XInputGetState_t>(GetProcAddress(XInputDll, "XInputGetState"));
        opened = true;
        if(XInputGetStatePtr == null) {
            LOG_Warn("XInput open failed: DLL: %p, XInputGetState: %p", XInputDll, XInputGetStatePtr);
        } else {
            LOG_Debug("xinput1_3.dll opened OK, XInputGetState: %p", XInputGetStatePtr);
        }
    }
    return XInputGetStatePtr;
}

}    // namespace

namespace XInput {

bool is_connected()
{
    return XInputGetStatePtr != null;
}

void cleanup()
{
    XInputGetStatePtr = null;
    if(XInputDll != null) {
        FreeLibrary(XInputDll);
        LOG_Info("Closing XInput DLL");
    }
    opened = false;
}

DWORD WINAPI get_state(DWORD d, XINPUT_STATE *state)
{
    auto p = XInputGetState_fn();
    if(p != null) {
        return p(d, state);
    }
    return E_NOT_VALID_STATE;
}

};    // namespace XInput
