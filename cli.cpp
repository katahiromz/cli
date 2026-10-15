// cli.cpp --- Cancelable command line interface
// Author: katahiromz
// License: MIT

#include <windows.h>
#include <stdio.h>
#include <string>
#include "common.h"

void version(void)
{
    puts("cli Version 0.0 by katahiromz\n");
}

void usage(void)
{
    puts(
        "Usage: cli [OPTIONS] [input_file] [output_file]\n"
        "  Options:\n"
        "    --help (or -h)                     Display this message.\n"
        "    --version (or -v)                  Display version info.\n"
        "    --input  INPUT  (or -i INPUT)      Specify the input file.\n"
        "    --output OUTPUT (or -o OUTPUT)     Specify the output file.\n"
    );
}

HANDLE g_cancel_event = NULL;
bool g_help = false;
bool g_version = false;
bool g_canceled = false;
std::wstring g_input_file;
std::wstring g_output_file;

bool do_it(void)
{
    g_cancel_event = CreateEventW(NULL, TRUE, FALSE, CLI_CANCEL_EVENT);
    if (!g_cancel_event) {
        fwprintf(stderr, L"cli: Cannot create g_cancel_event\n");
        return false;
    }
    AutoCloseHandle auto_close_0(g_cancel_event);

    SHARED shared;
    if (!shared.Create(999)) {
        fwprintf(stderr, L"SHARED::Create failed\n");
        return false;
    }

    FILE *fin = _wfopen(g_input_file.c_str(), L"rb");
    if (!fin) {
        fwprintf(stderr, L"cli: Cannot open file '%s'\n", g_input_file.c_str());
        return false;
    }
    FILE *fout;
    if (g_output_file.empty())
        fout = stdout;
    else
        fout = _wfopen(g_output_file.c_str(), L"wb");
    if (!fout) {
        fwprintf(stderr, L"cli: Cannot open file '%s'\n", g_output_file.c_str());
        fclose(fin);
        return false;
    }

    char buf[512];
    SHARED::value_type value;
    while (fgets(buf, sizeof(buf), fin)) {
        shared.Write((SHARED::value_type)strlen(buf));
        if (WaitForSingleObject(g_cancel_event, 0) == WAIT_OBJECT_0) {
            g_canceled = true;
            break;
        }
        fprintf(fout, "%s", buf);
        Sleep(1000);
    }

    fclose(fin);
    if (g_output_file.size())
        fclose(fout);

    if (g_canceled) {
        fwprintf(stderr, L"cli: operation canceled\n");
        return false;
    }

    return true;
}

bool run(void)
{
    if (g_help) {
        usage();
        return false;
    }
    if (g_version) {
        version();
        return false;
    }

    if (!do_it()) {
        return false;
    }

    return true;
}

bool parse_cmdline(int argc, wchar_t **wargv)
{
    for (int iarg = 1; iarg < argc; ++iarg) {
        std::wstring arg = wargv[iarg];
        if (arg == L"-h" || arg == L"--help") {
            g_help = true;
            continue;
        }
        if (arg == L"-v" || arg == L"--version") {
            g_version = true;
            continue;
        }
        if (arg == L"-i" || arg == L"--input") {
            if (++iarg < argc) {
                if (g_input_file.empty()) {
                    g_input_file = wargv[iarg];
                } else {
                    fwprintf(stderr, L"cli: too many argument\n");
                    return false;
                }
            } else {
                fwprintf(stderr, L"cli: missing argument for -i\n");
                return false;
            }
            continue;
        }
        if (arg == L"-o" || arg == L"--output") {
            if (++iarg < argc) {
                if (g_output_file.empty()) {
                    g_output_file = wargv[iarg];
                } else {
                    fwprintf(stderr, L"cli: too many argument\n");
                    return false;
                }
            } else {
                fwprintf(stderr, L"cli: missing argument for -o\n");
                return false;
            }
            continue;
        }
        if (arg.size() && arg[0] == L'-') {
            fwprintf(stderr, L"cli: invalid argument '%s'\n", arg.c_str());
            return false;
        }
        if (g_input_file.empty()) {
            g_input_file = arg;
        } else if (g_output_file.empty()) {
            g_output_file = arg;
        } else {
            fwprintf(stderr, L"cli: invalid argument '%s'\n", arg.c_str());
            return false;
        }
    }

    if (g_input_file.empty()) {
        g_input_file = L"cli.cpp";
    }

    return true;
}

int wmain(int argc, wchar_t **wargv)
{
    if (!parse_cmdline(argc, wargv))
        return 1;
    return run() ? 0 : 1;
}

int main(void)
{
    INT argc;
    LPWSTR *wargv = CommandLineToArgvW(GetCommandLineW(), &argc);
    INT ret = wmain(argc, wargv);
    LocalFree(wargv);
    return ret;
}
