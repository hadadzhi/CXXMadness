#include "String.h"
#include <cstring>
#include <iostream>

namespace TCXXPL {
    String::String(const char* str) {
        size = std::strlen(str);
        data = size > short_limit ? new char[size] : short_data;
        std::memcpy(data, str, size);
    }

    String::String(const String& rhs) {
        copy_construct(rhs);
    }

    String& String::operator=(const String& rhs) {
        if (this != &rhs) {
            this->~String();
            copy_construct(rhs);
        }
        return *this;
    }

    String::String(String&& rhs) noexcept {
        move_construct(rhs);
    }

    String& String::operator=(String&& rhs) noexcept {
        if (this != &rhs) {
            this->~String();
            move_construct(rhs);
        }
        return *this;
    }

    size_t String::length() const {
        return size;
    }

    char String::operator[](size_t pos) const {
        return data[pos];
    }

    String::~String() {
        if (data && size > short_limit) {
            delete[] data;
        }
    }
    
    void String::copy_construct(const String& rhs) noexcept {
        if (rhs.size > short_limit) {
            size = rhs.size;
            data = new char[size];
            std::memcpy(data, rhs.data, size);
        } else {
            std::memcpy(this, &rhs, sizeof(rhs));
        }
    }

    void String::move_construct(String& rhs) noexcept {
        if (rhs.size > short_limit) {
            size = rhs.size;
            data = rhs.data;
        } else {
            std::memcpy(this, &rhs, sizeof(rhs));
        }
        rhs.size = 0;
        rhs.data = nullptr;
    }

    bool operator==(const String& l, const String& r) {
        if (&l == &r) {
            return true;
        }

        if (l.length() == r.length()) {
            size_t len{r.length()};
            for (size_t pos{}; pos < len; ++pos) {
                if (l[pos] != r[pos]) {
                    return false;
                }
            }
            return true;
        }

        return false;
    }

    bool operator!=(const String& l, const String& r) {
        return !(l == r);
    }

    std::ostream& operator<<(std::ostream &os, const String& s) {
        for (size_t p{}, l{s.length()}; p < l; ++p) {
            os << s[p];
        }
        return os;
    }
}
