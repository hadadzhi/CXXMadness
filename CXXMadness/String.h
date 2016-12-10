#ifndef TCXXPL_STRING_H
#define TCXXPL_STRING_H

#include <cstddef>
#include <iostream>

namespace TCXXPL {
    // What am I doing?
    class String {
    public:
        String(const char* str);
        
        String(const String& rhs);
        String& operator=(const String& rhs);

        String(String&& rhs) noexcept;
        String& operator=(String&& rhs) noexcept;

        size_t length() const;

        char operator[](size_t pos) const;

        ~String();

    private:
        static constexpr size_t short_limit = 16;
        
        size_t size;
        char* data;
        char short_data[short_limit];

        void copy_construct(const String& rhs) noexcept;
        void move_construct(String& rhs) noexcept;
    };

    bool operator==(const String& l, const String& r);
    bool operator!=(const String& l, const String& r);

    std::ostream& operator<<(std::ostream& os, const String& s);
}

#endif // TCXXPL_STRING_H
