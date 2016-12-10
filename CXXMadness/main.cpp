#include <iostream>
#include <memory>
#include <sstream>
#include <random>
#include <chrono>
#include <cassert>
#include <functional>
#include <tuple>
#include <numeric>
#include <iterator>
#include <vector>
#include <string>
#include <algorithm>
#include <regex>
#include <sstream>

#include "StupidString.h"

namespace ConstReferenceToTemporary {
    std::string foo() {
        return "foo";
    }

    void main() {
        const std::string& cs = foo();
        std::cout << cs << std::endl;
    }
}

namespace Assertions {
    void main() {

#if false
        static_assert(false, "This sentence is false");
#endif

    }
}

namespace InitializerLists {
    void foo(std::initializer_list<int> list) {
        std::cout << list.size() << " elements:\n";
        for (int i : list) {
            std::cout << i << '\n';
        }
    }

    void main() {
        foo({1, 2, 3});
    }
}

namespace Templates {
    template<typename From = std::string, typename To = std::string>
    To convert(const From& source) {
        To result;
        std::stringstream stream;

        if (!(stream << source) || // Write source as a string
            !(stream >> result) || // Read result from a string
            !(stream >> std::ws).eof()) { // Discard whitespace and check if stream still has content
            throw std::runtime_error("conversion failed");
        }

        return result;
    }

    void main() {
        double d = Templates::convert<std::string, double>("3.14");
        std::string s = Templates::convert("Foo");
        std::string sd = Templates::convert<double, std::string>(d);

        auto sda = Templates::convert(2.7f);

        std::cout << "d = " << d << '\n';
        std::cout << "s = " << s << '\n';
        std::cout << "sd = " << sd << '\n';
        std::cout << "sda = " << sda << '\n';

        // Should throw an exception
        //double foo = Templates::convert<std::string, double>("Foo");
    }
}

namespace SmartPointers {
    std::unique_ptr<int> i_return_a_pointer(std::unique_ptr<int> return_me) {
        return return_me;
    }

    void main() {
        auto pi = std::make_unique<int>(42);

        std::cout << "pi = " << pi.get() << '\n';

        auto pi_1 = std::move(pi);

        std::cout << "After move:" << '\n';
        std::cout << "pi   = " << pi.get() << '\n';
        std::cout << "pi_1 = " << pi_1.get() << '\n';

        auto pi_2 = i_return_a_pointer(std::move(pi_1));

        std::cout << "After function call:" << '\n';
        std::cout << "pi   = " << pi.get() << '\n';
        std::cout << "pi_1 = " << pi_1.get() << '\n';
        std::cout << "pi_2 = " << pi_2.get() << '\n';
    }
}

namespace Lambdas {
    struct MyLambda {
        const int& a;

        MyLambda(const int& a) : a{a} {}

        int operator()(int b) {
            return a + b;
        }
    };

    void main() {
        int a = 0;

        auto add_a = [&a](int b) -> int {
            return a + b;
        };
        auto add_a_my = MyLambda(a);

        a = 42;

        const int b = 100;
        std::cout << "add_a   :" << add_a(b) << '\n';
        std::cout << "add_a_my:" << add_a_my(b) << '\n';
    }
}

namespace Random {
    void main() {
        std::random_device random_device;
        std::uniform_real_distribution<double> distribution(0, 1);
        std::mt19937_64 engine(random_device());

        for (int i = 0; i < 10; ++i) {
            const double rnd = distribution(engine);
            const int count = (int) std::floor(80 * rnd);

            std::cout << rnd;
            for (int j = 0; j < count; ++j) {
                std::cout << '*';
            }
            std::cout << '\n';
        }
    }
}

namespace ValueCategories {
    template<typename T>
    T& same_object(T& ref) {
        return ref;
    }

    template<typename T>
    T make_copy(const T& val) {
        return val;
    }

    class Foo {
    public:
        Foo() {
            std::cout << "Default-ctor\n";
        }

        Foo(const Foo&) {
            std::cout << "Copy-ctor\n";
        }

        Foo& operator=(const Foo&) {
            std::cout << "Copy-assignment\n";
            return *this;
        }

        Foo(Foo&&) {
            std::cout << "Move-ctor\n";
        }

        Foo& operator=(Foo&&) {
            std::cout << "Move-assignment\n";
            return *this;
        }

        ~Foo() {
            std::cout << "Destructor\n";
        }
    };

    Foo make_foo() {
        return Foo();
    }

    void main() {
        int a = 123;

        a = 321;
        same_object(a) = 42;

        int b = make_copy(a);

        std::cout << b << '\n';

        Foo foo1;
        Foo foo2 = make_copy(foo1);
        Foo foo3 = foo2;
        Foo foo4;
        foo4 = foo3;
        Foo foo5 = make_foo();
        Foo foo6 = std::move(foo5);
        Foo foo7;
        foo7 = make_foo();
        Foo foo8;
        foo8 = std::move(foo7);
        Foo foo9(foo8);
        Foo foo10(std::move(foo9));
    }
}

namespace Madness {
    template<typename T>
    class LinkedList {
    private:
        const T head;
        const LinkedList<T>* tail;
    public:
        LinkedList(const T& head) : head{head}, tail{nullptr} {}

        LinkedList(const T& head, const LinkedList<T>& tail) : head{head}, tail{&tail} {}

        const T& get_head() const {
            return head;
        }

        const LinkedList<T>& get_tail() const {
            return *tail;
        }

        bool has_tail() const {
            return tail != nullptr;
        }
    };

    template<typename T>
    LinkedList<T> conj(const LinkedList<T>& list, const T& head) {
        return LinkedList<T>(head, list);
    }

    template<typename T>
    void print_list(std::ostream& os, const LinkedList<T>& list) {
        os << list.get_head();
        if (list.has_tail()) {
            os << ' ';
            print_list(os, list.get_tail());
        }
    }

    template<typename T>
    std::ostream& operator<<(std::ostream& os, const LinkedList<T>& list) {
        os << '(';
        print_list(os, list);
        os << ')';
        return os;
    }

    void main() {
        const auto l1 = LinkedList<std::string>(std::string("one"));
        const auto l2 = conj(l1, std::string("two"));
        const auto l3 = conj(l2, std::string("three"));
        std::cout << l1 << '\n';
        std::cout << l2 << '\n';
        std::cout << l3 << '\n';
    }
}

namespace GoTo {
    void main() {
        using namespace ValueCategories;

        bool jump{true};

    Label1:
        {
            Foo foo1;
            Foo foo2;

            //            Label2:
            if (jump) {
                goto Label3;
            }

            Foo foo3;
        }

    Label3:
        if (jump) {
            jump = false;
            //            goto Label2; // Error: skips initialization
            goto Label1;
        }
    }
}

namespace Typeid {
    void main() {
        ValueCategories::Foo foo;
        int a{42};
        double b{3.14};
        std::string s{"I am a string"};
        std::cout << typeid(foo).name() << '\n';
        std::cout << typeid(a).name() << '\n';
        std::cout << typeid(b).name() << '\n';
        std::cout << typeid(&a).name() << '\n';
        std::cout << typeid(&b).name() << '\n';
        std::cout << typeid(s).name() << '\n';
    }
}

namespace OOFail {
    class EventHandler {
    public:
        virtual int operator()(int) const = 0;
    };

    static std::vector<std::unique_ptr<const EventHandler>> handlers;

    void register_handler(std::unique_ptr<const EventHandler>&& h) {
        handlers.push_back(std::move(h));
    }

    void call_handlers() {
        for (const auto& h : handlers) {
            std::cout << (*h)(1) << '\n';
        }
    }

    class MyEventHandler : public EventHandler {
    private:
        int b;
    public:
        explicit MyEventHandler(int b) : b{b} {}

        int operator()(int a) const override {
            return a + b;
        }
    };

    void register_handlers() {
        register_handler(std::make_unique<const MyEventHandler>(9000));
    }

    void main() {
        register_handlers();
        call_handlers();
    }
}

namespace LambdaFail {
    class IHoldLambdas {
    private:
        std::function<void()> f;
    public:
        IHoldLambdas(const std::function<void()>& f) : f{f} {}

        void do_f() const {
            f();
        }
    };

    void main() {
        IHoldLambdas foo([]() {
            std::cout << "Boo!" << '\n';
        });
        foo.do_f();
    }
}

namespace PointerToTemporaryFail {
    template<typename T>
    const T* addr(const T& t) {
        return &t;
    }

    template<typename T>
    T deref(const T* iptr) {
        return *iptr;
    }

    void main() {
        //const int* iptr = &5;           // Address expression must be an lvalue, bla bla bla
        const int* iptr = addr(42);       // We took the address of a temporary and the compiler is silent as fish
        const int* iptr2 = &(*iptr);      // We can even do this nonsense
        std::cout << *iptr << '\n';       // It will even work "as expected" in debug build
        std::cout << *iptr2 << '\n';      // But not in release
        std::cout << deref(iptr) << '\n';
    }
}

namespace DuffsDevice {
    void count_down(int count) {
        assert(count > 0 && "Count <= 0");
        int n = (count + 7) / 8;
        // @formatter:off
        switch (count % 8) {
            case 0: do {
                std::cout << count-- << ' ';
            case 7:      std::cout << count-- << ' ';
            case 6:      std::cout << count-- << ' ';
            case 5:      std::cout << count-- << ' ';
            case 4:      std::cout << count-- << ' ';
            case 3:      std::cout << count-- << ' ';
            case 2:      std::cout << count-- << ' ';
            case 1:      std::cout << count-- << ' ';
            } while (--n > 0);
        }
        // @formatter:on
    }

    void main() {
        for (int i = 1; i <= 16; i++) {
            std::cout << i << ": ";
            count_down(i);
            std::cout << '\n';
        }
    }
}

namespace Tie {
    template<typename... _Elements>
    std::tuple<_Elements& ...> my_tie(_Elements& ... __args) noexcept {
        return std::tuple<_Elements& ...>(__args...);
    }

    class Foo {
    public:
        void operator=(int c) {
            std::cout << "Boo!" << c << '\n';
        }

        Foo() {}

        Foo(const Foo&) {
            std::cout << "Foo copied\n";
        }

        Foo(Foo&&) noexcept {
            std::cout << "Foo moved\n";
        }

        ~Foo() {
            std::cout << "Foo died\n";
        }
    };

    Foo make_foo() {
        return Foo();
    }

    int make_int() {
        return 42;
    }

    void main() {
        Foo foo;

        int a1{42};
        std::string s1{"The ultimate answer"};

        int a2;
        std::string s2;

        my_tie(a2, s2, std::ignore) = std::tuple<int, std::string, std::string>(a1, s1, "whaaa");

        //make_int() = 9000;
        make_foo() = 9000;

        std::cout << "a1 = " << a1 << '\n';
        std::cout << "21 = " << s1 << '\n';

        std::cout << "a2 = " << a2 << '\n';
        std::cout << "s2 = " << s2 << '\n';
    }
}

namespace CPlusPlusMacro {
    void main() {
        std::cout << "__cplusplus = " << __cplusplus << '\n';
    }
}

namespace CXX14 {
    template<typename T>
    constexpr const T& min(const T& a, const T& b) noexcept {
        if (a < b) {
            return a;
        }
        return b;
    }

    void println() {
        std::cout << '\n';
    }

    // This is needed to avoid an extra ' ' at the end of line
    // AHTUNG: needs to be declared !before! the main variadic function
    //         otherwise the 'void println()' overload takes over
    template<typename Head>
    void println(const Head& first) {
        std::cout << first << '\n';
    }

    template<typename Head, typename... Tail>
    void println(const Head& first, const Tail& ... rest) {
        std::cout << first << ' ';
        println(rest...);
    }

    [[deprecated]]
    void i_am_deprecated() {}

    template<typename T>
    constexpr T pi = T(3.141592653589793238462643383L);

    template<typename T>
    constexpr const T& rsum(const T& first) {
        return first;
    }

    template<typename T, typename... R>
    auto rsum(const T& first, const R& ... rest) {
        return first + rsum(rest...);
    }

    void main() {
        println(min(42, 9'000'000));
        println(1, "2", '3');

        i_am_deprecated(); // Issues a warning

        println(pi<int>);
        println(pi<float>);
        println(pi<double>);

        println(rsum(pi<int>, pi<float>, pi<double>));

        auto ip = std::make_unique<int>(42);
        println("ip =", ip.get());

        auto glce = [a = 42, b = std::move(ip)](auto c) {
            println("a =", a);
            println("b =", *b);
            println("c =", c);
        };

        println("ip =", ip.get());
        glce(100500);
        glce("A string");

        auto p = std::make_tuple<std::string, int>("The answer", 42);

        auto s2 = std::get<0>(p);
        auto s3 = std::get<std::string>(p);

        auto i2 = std::get<1>(p);
        auto i3 = std::get<int>(p);

        using namespace std::literals;

        auto t1 = 10ns;
        auto s1 = "I am a std::string"s;
        auto cs = "I am a C string";
    }
}

//#define PAIN_AND_SUFFERING
#ifdef PAIN_AND_SUFFERING
// This is why we should use "Clang with Microsoft CodeGen" for debug
namespace PortabilityIsFun {
    template<typename T>
    struct S {
        template<typename U> void foo() {
            std::cout << "Boo!\n";
        }
    };

    template<typename T>
    void bar() {
        typename S<T> s; // error: expected a qualified name after 'typename'
        s.foo<T>(); // error: use 'template' keyword to treat 'foo' as a dependent template name
    }

    // error: no matching function for call to 'rsum'
    // because overload below is not in scope yet
    // but MSVC finds it somehow
    template<typename T, typename... R>
    auto rsum(const T& first, const R&... rest) {
        return first + rsum(rest...);
    }

    template<typename T>
    constexpr const T& rsum(const T& first) {
        return first;
    }

    void main() {
        bar<void>();
        rsum(1, 2.0f, 3.0)
    }
}
#endif

namespace Tree {
    template<typename T>
    struct Tree {
        std::shared_ptr<Tree<T>> left;
        std::shared_ptr<Tree<T>> right;
        T value;

        ~Tree() {
            std::cout << "Tree node destroyed\n";
        }
    };

    template<typename T>
    std::shared_ptr<Tree<T>> find(std::shared_ptr<Tree<T>> tree, std::function<bool(const T&)> predicate) {
        if (predicate(tree->value)) {
            return tree;
        }

        for (auto child : {tree->left, tree->right}) {
            if (child) {
                auto result = find(child, predicate);
                if (result) {
                    return result;
                }
            }
        }

        return std::shared_ptr<Tree<T>>();
    }

    std::shared_ptr<Tree<int>> make_int_tree(int depth, int top_value) {
        assert(depth > 0);
        auto tree = std::make_shared<Tree<int>>();

        tree->value = top_value;

        if (--depth) {
            tree->left = make_int_tree(depth, ++top_value);
            tree->right = make_int_tree(depth, ++top_value);
        }

        return tree;
    }

    void main() {
        auto tree = make_int_tree(3, 1);
        auto t3 = find<int>(tree, [](int v) {
            return v == 3;
        });
        auto t5 = find<int>(tree, [](int v) {
            return v == 5;
        });
        auto t7 = find<int>(tree, [](int v) {
            return v == 7;
        });
    }
}

namespace FizzBuzz {
    void fizz_buzz(int count) {
        for (int i = 1; i <= count; i++) {
            std::string out{};

            if (i % 3 == 0) {
                out += "fizz";
            }

            if (i % 5 == 0) {
                out += "buzz";
            }

            if (out.length() == 0) {
                std::cout << i << '\n';
            } else {
                std::cout << out << '\n';
            }
        }
    }
}

namespace UsingTypedefAndTemplates {
    template<typename T>
    using It1 = std::istream_iterator<T>;

    template<typename T>
    struct It2 {
        typedef std::istream_iterator<T> type;
    };

    void main() {
        std::istream_iterator<int> it;
        It1<int> it1;
        It2<int>::type it2;
    }
}

namespace CallingConventions {
    float __stdcall sfoo(float a, int b) {
        return a * b;
    }

    float __cdecl cfoo(float a, int b) {
        return a * b;
    }

    void main() {
        const float r1 = sfoo(3.14, 42);
        const float r2 = cfoo(3.14, 42);
    }
}

namespace OOP {
    struct IDevice {
        virtual void do_shit() const = 0;

        // AHTUNG: all interfaces/abstract classes must have a virtual destructor
        //         otherwise, an object of implementing concrete class that has a destructor
        //         will not have its destructor called when used through a pointer to an interface
        virtual ~IDevice() {}
    };

    struct ConsoleDevice : public IDevice {
        void do_shit() const final override {
            std::cout << "Device did shit\n";
        }

        ~ConsoleDevice() {
            std::cout << "Device died\n";
        }
    };

    struct IDrawable {
        virtual void draw(const IDevice& device) const = 0;

        virtual void non_const_function() = 0;

        virtual ~IDrawable() {}
    };

    class Circle : public IDrawable {
    private:
        float radius;
    public:
        Circle() : Circle(0) {}

        Circle(float radius) : radius{radius} {}

        void draw(const IDevice& device) const override {
            std::cout << "Drawing a circle of radius " << radius << '\n';
            device.do_shit();
        }

        void non_const_function() override {
            radius = -1;
            std::cout << "Oops\n";
        }
    };

    // So how are we supposed to write this in C++ where there aren't any standard interfaces?
    // Like Iterable, or Set, or List...
    // Write a separate draw_all for each kind of container we can put our IDrawable`s into?
    //void draw_all(const std::vector<std::unique_ptr<const IDrawable>>& drawables, const IDevice& device) {
    //    for (const auto& d : drawables) {
    //        d->draw(device);
    //    }
    //}

    // Ok...
    template<typename Container>
    void draw_and_modify_all(const Container& drawables, const IDevice& device) {
        static_assert(std::is_base_of<IDrawable, typename Container::value_type::element_type>::value,
                      "I am expecting a container of smart pointers to IDrawable");

        for (const auto& d : drawables) {
            d->draw(device);
            d->non_const_function();
        }
    }

    void main() {
        const ConsoleDevice device{};

        auto c1 = std::make_unique<Circle>(42);
        auto c2 = std::make_unique<Circle>(43);
        auto c3 = std::make_unique<Circle>(44);

        // Here, the shiny new initialization syntax fails spectacularly:
        // the vector's constructor can't copy unique_ptr`s out of the initializer list (because they are unique)
        // nor can it move them out (because they are in an initializer list)
        //std::vector<std::unique_ptr<const IDrawable>> circles{std::move(c1), std::move(c2), std::move(c3)};
        //std::vector<std::unique_ptr<const IDrawable>> circles{std::make_unique<Circle>(9000.0f)};

        // A vector should probably own its contents, so we use unique_ptr
        std::vector<std::unique_ptr<IDrawable>> circles;
        circles.push_back(std::move(c1));
        circles.push_back(std::move(c2));
        circles.push_back(std::move(c3));

        draw_and_modify_all(circles, device);

        // Should not compile
        //std::vector<int> ints{1, 2, 3};
        //draw_and_modify_all(ints, device);

        // This works with any kind of container that supports iteration
        std::for_each(std::begin(circles), std::end(circles), [&](const auto& pc) {
            pc->draw(device);
        });

        std::unique_ptr<const IDevice> call_my_destructor_plz = std::make_unique<ConsoleDevice>();
    }
}

namespace InitializerList {
    class Foo {
    public:
        Foo() {}

        Foo(int, long) {}

        //Foo(std::initializer_list<double>) {}
        Foo(std::initializer_list<double>, double = 3.14) {}

        template<typename T>
        void foo(std::initializer_list<T> args) {
            for (const auto& arg : args) {
                std::cout << arg << '\n';
            }
        }
    };

    void main() {
        Foo foo1{};

        Foo foo2(3.14, 2.7);

        Foo foo3{3.14, 2.7};

        Foo foo4{{3.14}, 2.7};

        Foo foo5({3.14}, 2.7);

        foo1.foo({1, 2, 3});
    }
}

namespace ConstructorThrows {
    class Bar {
    private:
        std::string name;
    public:
        Bar(const std::string& name) : name{name} {
            std::cout << "Bar " << name << " constructed\n";
        }

        Bar(const Bar& other) : name{other.name} {}

        Bar& operator=(const Bar& other) {
            name = other.name;
            return *this;
        }

        ~Bar() {
            std::cout << "Bar " << name << " died\n";
        }
    };

    class Foo {
    private:
        Bar bar;
    public:
        Foo() : bar{"Member"} {
            Bar bar{"Local"};
            throw std::exception();
        }

        ~Foo() {
            std::cout << "Foo died\n";
        }
    };

    void main() {
        try {
            Foo foo;
        } catch (...) {
            std::cout << "Exception caught\n";
        }
    }
}

namespace TemplateMadness {
    template<typename T>
    struct Complex {
        static_assert(std::is_arithmetic<T>::value, "T must be arithmetic");

        T re;
        T im;

        Complex(T re = {}, T im = {}) : re{re}, im{im} {
            std::cout << "Complex constructed\n";
        }

        Complex(const Complex& other) : re{other.re}, im{other.im} {
            std::cout << "Complex copy-constructed\n";
        }

        ~Complex() {
            std::cout << "Complex died\n";
        }
    };

    struct Foo {
        explicit Foo(int) {}
    };

    void main() {
        Complex<double> c0;
        //Complex<std::string> c1;
        //Complex<Foo> c2;

        //using Complex = Complex<int>;

        //Complex c0;
        //Complex c1{1};
        //Complex c2{1, 2};
        //Complex c3{c0};

        //c1 = 42;
        //c3 = c2;
    }
}

namespace IDEChallenge {
    template<size_t S = sizeof(size_t)>
    struct A {
    };

    template<>
    struct A<4> {
        enum {
            B
        };
    };

    template<>
    struct A<8> {
        template<int>
        struct B {
        };
    };

    enum {
        C, D
    };

    void main() {
        // On 32-bit machines, this is a comparison expression.
        // On 64-bit machines, this is a local variable declaration.
        // CLion can't handle the latter case :)
        A<>::B < C > D;
    }
}

namespace ADL {
    template<typename It, typename Func>
    void my_for_each(It begin, It end, Func f) {
        for (; begin != end; ++begin) {
            f(*begin);
        }
    }

    template<typename Cont, typename Func>
    void my_for_each_cont(const Cont& c, Func f) {
        // begin() and end() are looked up in the namespace of their arguments
        for (auto it = begin(c), it_end = end(c); it != it_end; ++it) {
            f(*it);
        }
    }

    template<typename Cont, typename Func>
    void my_for_each_cont_range_loop(const Cont& c, Func f) {
        // range-based loop is implemented similarly
        // and it might shoot you in the foot if you have begin() and end() declared
        // in the same namespace as Cont that have nothing to do with iterators
        for (auto e : c) {
            f(e);
        }
    }

    namespace Foo {
        template<typename T>
        struct FooContainer {
            // Pretend this is an actual container

            // begin() and end() as member functions have higher priority, which resolves the problem
            T* begin() const {
                return nullptr;
            }

            T* end() const {
                return nullptr;
            }
        };

        template<typename T>
        void begin(T t) {
            // A function that has nothing to do with iterating over a container
        }

        template<typename T>
        void end(T t) {
            // A function that has nothing to do with iterating over a container
        }
    }

    void main() {
        std::vector<int> v{1, 2, 3};
        const auto l = [](int e) { std::cout << e << ' '; };

        my_for_each(v.begin(), v.end(), l);
        std::cout << '\n';

        my_for_each_cont(v, l);
        std::cout << '\n';

        my_for_each_cont_range_loop(v, l);
        std::cout << '\n';

        Foo::FooContainer<int> c;
        my_for_each_cont_range_loop(c, l);
        std::cout << '\n';

        // std::for_each works without std::
        // because the return type of v.begin is defined in std::
        for_each(v.begin(), v.end(), l);

        std::cout << '\n';
    }
}

namespace ExplicitConversion {
    class Foo {
    private:
        bool b;
    public:
        Foo(bool b) : b{b} {}

        explicit operator bool() const {
            return b;
        }
    };

    void main() {
        Foo foo{true};

        bool b1{foo}; // {} initialization considered explicit
        //bool b2 = foo; // = initialization considered implicit

        // One argument constructor works as an implicit conversion
        foo = b1;

        // Contextual conversion to bool works even with explicit operator bool()
        // in if/while/for statements
        // in arguments of logical operators !, ||, &&, ?:
        // in static_assert condition
        // in noexcept specifier
        if (foo) {
            std::cout << "Foo() == true\n";
        } else {
            std::cout << "Foo() == false\n";
        }
    }
}

namespace StaticAssert {
    constexpr bool the_truth = true;

    static_assert(the_truth, "");

    class Foo {
        static_assert(the_truth, "");

        void foo() {
            static_assert(the_truth, "");
        }
    };

    void foo() {
        static_assert(the_truth, "");
    }
}

namespace Replace {
    // Replace "bats" with "mice"
    const std::string the_string{"Do bats eat cats? Do cats eat bats?"};

    template<typename BasicString> // requires a std::basic_string<...>
    void replace(BasicString& str, const BasicString& target, const BasicString& replacement) {
        using size_type = typename BasicString::size_type;

        const size_type tlen = target.length();
        const size_type rlen = replacement.length();

        size_type pos = {};

        while ((pos = str.find(target, pos)) != BasicString::npos) {
            str.replace(pos, tlen, replacement);
            pos += rlen;
        }
    }

    template<typename BasicString>
    BasicString replace(const BasicString& str, const BasicString& target, const BasicString& replacement) {
        BasicString result{str};
        replace(result, target, replacement);
        return result;
    }

    void main() {
        //std::string s{the_string};
        //std::regex_replace(s.begin(), s.begin(), s.end(), std::regex("bats"), "mice");

        using namespace std::string_literals;

        std::cout << replace(the_string, "bats"s, "mice"s) << '\n';
    }
}

namespace StringTest {
    void main() {
        using namespace TCXXPL;
        
        const char* test_string{"I am a string! Yay!"};

        StupidString s1{test_string};
        StupidString s2{s1};

        assert(s1 == s2);
        assert(s1 == test_string);

        StupidString s3{std::move(s2)};
        
        assert(s1 == s3);

        s2 = "I am another string. I am here for testing! Whoohoo!";

        assert(s2 != s1);

        std::cout << s2 << '\n';

        std::stringstream sstream;
        sstream << s2;
        std::cout << sstream.str() << '\n';
    }
}

int main() {
    //Replace::main();
    //ADL::main();
    //ExplicitConversion::main();
    //TemplateMadness::main();
    //ConstructorThrows::main();
    //InitializerList::main();
    //OOP::main();
    //CallingConventions::main();
    //ConstReferenceToTemporary::main();
    //Assertions::main();
    //InitializerLists::main();
    //Templates::main();
    //SmartPointers::main();
    //Lambdas::main();
    //Random::main();
    //ValueCategories::main();
    //Madness::main();
    //GoTo::main();
    //Typeid::main();
    //OOFail::main();
    //LambdaFail::main();
    //PointerToTemporaryFail::main();
    //DuffsDevice::main();
    //Tie::main();
    //CPlusPlusMacro::main();
    //CXX14::main();
    //Tree::main();
    //FizzBuzz::fizz_buzz(100);
    StringTest::main();
}
