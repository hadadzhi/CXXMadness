#ifndef TCXXPL_STUPIDSTRING_H
#define TCXXPL_STUPIDSTRING_H

#include <cstddef>
#include <iostream>

namespace TCXXPL {
    // What am I doing?
    class StupidString {
    public:
        StupidString(const char* str);
        
        StupidString(const StupidString& rhs);
        StupidString& operator=(const StupidString& rhs);

        StupidString(StupidString&& rhs) noexcept;
        StupidString& operator=(StupidString&& rhs) noexcept;

        size_t length() const;

        char operator[](size_t pos) const;

        ~StupidString();

    private:
        static constexpr size_t short_limit = 16;
        
        size_t size;
        char* data;
        char short_data[short_limit];

        void copy_construct(const StupidString& rhs) noexcept;
        void move_construct(StupidString& rhs) noexcept;
    };

    bool operator==(const StupidString& l, const StupidString& r);
    bool operator!=(const StupidString& l, const StupidString& r);

    std::ostream& operator<<(std::ostream& os, const StupidString& s);
}

#endif // TCXXPL_STUPIDSTRING_H
