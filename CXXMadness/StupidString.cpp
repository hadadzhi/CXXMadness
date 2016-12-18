#include "StupidString.h"

#include <iostream>
#include <cstring>

namespace TCXXPL {
    StupidString::StupidString(const char* str) {
        size = std::strlen(str);
        data = size > short_limit ? new char[size] : short_data;
        std::memcpy(data, str, size);
    }

    StupidString::StupidString(const StupidString& rhs) {
        copy_construct(rhs);
    }

    StupidString& StupidString::operator=(const StupidString& rhs) {
        if (this != &rhs) {
            this->~StupidString();
            copy_construct(rhs);
        }
        return *this;
    }

    StupidString::StupidString(StupidString&& rhs) noexcept {
        move_construct(rhs);
    }

    StupidString& StupidString::operator=(StupidString&& rhs) noexcept {
        if (this != &rhs) {
            this->~StupidString();
            move_construct(rhs);
        }
        return *this;
    }

    size_t StupidString::length() const {
        return size;
    }

    char StupidString::operator[](size_t pos) const {
        return data[pos];
    }

    StupidString::~StupidString() {
        if (data && size > short_limit) {
            delete[] data;
        }
    }
    
    void StupidString::copy_construct(const StupidString& rhs) noexcept {
        if (rhs.size > short_limit) {
            size = rhs.size;
            data = new char[size];
            std::memcpy(data, rhs.data, size);
        } else {
            std::memcpy(this, &rhs, sizeof(rhs));
        }
    }

    void StupidString::move_construct(StupidString& rhs) noexcept {
        if (rhs.size > short_limit) {
            size = rhs.size;
            data = rhs.data;
        } else {
            std::memcpy(this, &rhs, sizeof(rhs));
        }
        rhs.size = 0;
        rhs.data = nullptr;
    }

    bool operator==(const StupidString& l, const StupidString& r) {
        if (&l == &r) {
            return true;
        }

        if (l.length() == r.length()) {
            size_t len{r.length()};
            for (size_t pos{0}; pos < len; ++pos) {
                if (l[pos] != r[pos]) {
                    return false;
                }
            }
            return true;
        }

        return false;
    }

    bool operator!=(const StupidString& l, const StupidString& r) {
        return !(l == r);
    }

    std::ostream& operator<<(std::ostream &os, const StupidString& s) {
        for (size_t p{0}, l{s.length()}; p < l; ++p) {
            os << s[p];
        }
        return os;
    }
}
