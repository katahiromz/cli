// canceler.cpp --- CLI canceler
// Author: katahiromz
// License: MIT

#include <windows.h>
#include <stdio.h>
#include "common.h"

int main(void)
{
    HANDLE cancel_event = OpenEventW(EVENT_MODIFY_STATE, FALSE, CLI_CANCEL_EVENT);
    if (!cancel_event) {
        fprintf(stderr, "canceler: Failed to open cancel_event\n");
        return 1;
    }
    AutoCloseHandle auto_close_0(cancel_event);

    SharedLong shared;
    SharedLong::value_type value;
    if (!shared.Open()) {
        fwprintf(stderr, L"canceler: SharedLong::Open failed\n");
        return 1;
    }

    for (int i = 0; i < 5; ++i) {
        shared.Read(&value);
        printf("value: %ld\n", value);
        Sleep(1000);
    }

    SetEvent(cancel_event);
    fprintf(stderr, "canceler: Canceled.\n");
    return 0;
}
