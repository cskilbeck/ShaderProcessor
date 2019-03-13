#pragma once

template <typename T> struct _fmt {
    static constexpr T const *empty_string = null;
    static size_t length(T const *p) = delete;
    static size_t count(T const *p, va_list v) = delete;
    static size_t print(T *const b, size_t const c, size_t const m, T const *const f, va_list a) = delete;
    static bool compare(T const *a, T const *b) = delete;
    static bool compare_nocase(T const *a, T const *b) = delete;
    static bool compare_len(T const *a, T const *b, size_t len) = delete;
    static T const *find(T const *n, T const *h) = delete;
    static T const *find_chr(T const *n, T h) = delete;
    static T *alloc(size_t n) = delete;
};

template <> struct _fmt<char> {
    static constexpr char const *empty_string = "";

    static size_t length(char const *p) {
        return strlen(p);
    }
    static size_t count(char const *p, va_list v) {
        return _vscprintf(p, v);
    }
    static size_t print(char *const b, size_t const c, size_t const m, char const *const f, va_list a) {
        return _vsnprintf_s(b, c, m, f, a);
    }
    static bool compare(char const *a, char const *b) {
        return strcmp(a, b);
    }
    static bool compare_nocase(char const *a, char const *b) {
        return _stricmp(a, b);
    }
    static bool compare_len(char const *a, char const *b, size_t len) {
        return strncmp(a, b, len);
    }
    static char const *find(char const *n, char const *h) {
        return strstr(n, h);
    }
    static char const *find_chr(char const *n, char h) {
        return strchr(n, h);
    }
    static char *alloc(size_t n) {
        return (char *)malloc(n + 1);
    }
    static bool is_whitespace(char c) {
        return strchr(" \t\r\n", c) != null;
    }
};

template <> struct _fmt<wchar> {
    static constexpr wchar const *empty_string = L"";

    static size_t length(wchar const *p) {
        return wcslen(p);
    }
    static size_t count(wchar const *p, va_list v) {
        return _vscwprintf(p, v);
    }
    static size_t print(wchar *const b, size_t const c, size_t const m, wchar const *const f, va_list a) {
        return _vsnwprintf_s(b, c, m, f, a);
    }
    static bool compare(wchar const *a, wchar const *b) {
        return wcscmp(a, b);
    }
    static bool compare_nocase(wchar const *a, wchar const *b) {
        return _wcsicmp(a, b);
    }
    static bool compare_len(wchar const *a, wchar const *b, size_t len) {
        return wcsncmp(a, b, len);
    }
    static wchar const *find(wchar const *n, wchar const *h) {
        return wcsstr(n, h);
    }
    static wchar const *find_chr(wchar const *n, wchar h) {
        return wcschr(n, h);
    }
    static wchar *alloc(size_t n) {
        return (wchar *)malloc((n + 1) * sizeof(wchar));
    }
};

//////////////////////////////////////////////////////////////////////

template <typename T> inline std::basic_string<T> str_format_v(T const *fmt, va_list v) {
    using F = _fmt<T>;
    size_t c = F::count(fmt, v) + 1;
    T *buffer = reinterpret_cast<T *>(alloca(c * sizeof(T)));
    F::print(buffer, c, c - 1, fmt, v);
    return std::basic_string<T>(buffer);
}

//////////////////////////////////////////////////////////////////////

template <typename T> inline std::basic_string<T> str_format(T const *fmt, ...) {
    va_list v;
    va_start(v, fmt);
    return str_format_v(fmt, v);
}
