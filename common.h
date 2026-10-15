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
    void Close() {
        if (m_h && m_h != INVALID_HANDLE_VALUE) {
            CloseHandle(m_h);
            m_h = NULL;
        }
    }
    operator HANDLE() const {
        return m_h;
    }
    PHANDLE operator&() {
        return &m_h;
    }
    void operator=(HANDLE h) {
        Close();
        m_h = h;
    }
};

template <typename T_BASE>
struct Shared : AutoCloseHandle {
    typedef T_BASE value_type;
    volatile value_type m_value;

    Shared() { }
    ~Shared() { }

    BOOL Create(const value_type& initial = {}) {
        m_h = CreateFileMappingW(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, sizeof(Shared), CLI_SHARED_NAME);
        if (!m_h)
            return FALSE;
        return Write(initial);
    }

    BOOL Open(BOOL bWrite = FALSE) {
        m_h = OpenFileMappingW(bWrite ? FILE_MAP_ALL_ACCESS : FILE_MAP_READ, TRUE, CLI_SHARED_NAME);
        return !!m_h;
    }

    BOOL Read(value_type* value) {
        auto pShared = (Shared*)MapViewOfFile(m_h, FILE_MAP_READ, 0, 0, sizeof(Shared));
        if (!pShared) {
            return FALSE;
        }
        *value = pShared->m_value;
        UnmapViewOfFile(pShared);
        return TRUE;
    }

    BOOL Write(const value_type& value) {
        auto pShared = (Shared*)MapViewOfFile(m_h, FILE_MAP_WRITE, 0, 0, sizeof(Shared));
        if (!pShared) {
            return FALSE;
        }
        pShared->m_value = value;
        UnmapViewOfFile(pShared);
        return TRUE;
    }
};

typedef Shared<LONG> SharedLong;

struct AutoFClose {
    FILE *m_fp;

    AutoFClose(FILE *fp = NULL) : m_fp(fp) { }

    ~AutoFClose() {
        Close();
    }

    void Close() {
        if (m_fp) {
            fclose(m_fp);
            m_fp = NULL;
        }
    }

    operator FILE*() const {
        return m_fp;
    }
    void operator=(FILE *fp) {
        Close();
        m_fp = fp;
    }
};
