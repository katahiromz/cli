// common.h --- Common part
// Author: katahiromz
// License: MIT

#pragma once

#define CLI_CANCEL_EVENT    L"cli cancel event"
#define CLI_SHARED_NAME     L"cli shared data"

struct AutoCloseHandle {
    HANDLE m_h = NULL;
    AutoCloseHandle(HANDLE h = NULL) : m_h(h) {}
    ~AutoCloseHandle() {
        Close();
    }
    void operator=(HANDLE h) {
        Close();
        m_h = h;
    }
    void Close() {
        if (m_h && m_h != INVALID_HANDLE_VALUE) {
            CloseHandle(m_h);
            m_h = NULL;
        }
    }
};

struct SHARED : AutoCloseHandle {
    typedef LONG value_type;
    volatile value_type m_value;

    SHARED() { }
    ~SHARED() { }

    BOOL Create(const value_type& initial = {}) {
        m_h = CreateFileMappingW(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, sizeof(SHARED), CLI_SHARED_NAME);
        if (!m_h)
            return FALSE;
        return Write(initial);
    }

    BOOL Open(BOOL bWrite = FALSE) {
        m_h = OpenFileMappingW(bWrite ? FILE_MAP_ALL_ACCESS : FILE_MAP_READ, TRUE, CLI_SHARED_NAME);
        return !!m_h;
    }

    BOOL Read(value_type* value) {
        auto pShared = (SHARED*)MapViewOfFile(m_h, FILE_MAP_READ, 0, 0, sizeof(SHARED));
        if (!pShared) {
            return FALSE;
        }
        *value = pShared->m_value;
        UnmapViewOfFile(pShared);
        return TRUE;
    }

    BOOL Write(const value_type& value) {
        auto pShared = (SHARED*)MapViewOfFile(m_h, FILE_MAP_WRITE, 0, 0, sizeof(SHARED));
        if (!pShared) {
            return FALSE;
        }
        pShared->m_value = value;
        UnmapViewOfFile(pShared);
        return TRUE;
    }
};
