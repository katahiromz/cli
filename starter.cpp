// starter.cpp --- C++/Win32 command line starter
// Author: katahiromz
// License: MIT

#include "MProcessMaker.hpp"

int main(void)
{
    MProcessMaker pmaker;
    //pmaker.SetCreationFlags(CREATE_NO_WINDOW);
    //pmaker.SetCreationFlags(CREATE_NEW_CONSOLE);
    if (!pmaker.CreateProcessDx("cli.exe")) {
        printf("Cannot start up\n");
        return 1;
    }
    pmaker.WaitForSingleObject();
    int ret = (int)pmaker.GetExitCode();
    printf("ret: %d\n", ret);
    return ret;
}
