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
#include <atomic>
#include <future>
#include <type_traits>
#include <stack>
#include <stdexcept>
#include <map>

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
            !(std::ws(stream)).eof()) { // Discard whitespace and check if stream still has content
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
        switch (count % 8) {
            do {
                case 0: std::cout << count-- << ' ';
                case 7: std::cout << count-- << ' ';
                case 6: std::cout << count-- << ' ';
                case 5: std::cout << count-- << ' ';
                case 4: std::cout << count-- << ' ';
                case 3: std::cout << count-- << ' ';
                case 2: std::cout << count-- << ' ';
                case 1: std::cout << count-- << ' ';
            } while (--n > 0);
        }
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
    
    // This is needed to avoid an extra ' ' at the end of the line
    // AHTUNG: must be declared !before! the main variadic template function,
    //         otherwise, the 'void println()' overload takes over
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
        
        //i_am_deprecated(); // Issues a warning
        
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
// Here is one reason to use "Clang with Microsoft CodeGen" for debug
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
            }
            else {
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
        const float r1 = sfoo(3.14f, 42);
        const float r2 = cfoo(3.14f, 42);
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
        }
        catch (...) {
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
        }
        else {
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
        
        size_type pos{0};
        
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

namespace DefaultDtor {
    class Foo {
    public:
        Foo() {
            std::cout << "Foo constructed\n";
        }
        ~Foo() {
            std::cout << "Foo died\n";
        }
    };
    
    class Bar1 {
        Foo foo;
    public:
        Bar1() {}
        ~Bar1() {
            std::cout << "Bar's destructor does something...\n";
        }
    };
    
    class Bar2 {
        Foo foo;
    public:
        Bar2() {}
        ~Bar2() = default;
    };
    
    class Bar3 {
        Foo foo;
    public:
        Bar3() {}
        
        Bar3(Bar3&&) {}
        // BTW, it doesn't bother Clang that a function that
        // must return a value, does not return a value.
        // MSVC refuses to compile this
        //Bar3& operator=(Bar3&&) {}
        Bar3& operator=(Bar3&&) { return *this; }
    };
    
    void main() {
        Bar1 bar1;
        Bar2 bar2;
        Bar3 bar3;
        //Bar3 bar3_1;
        //bar3 = bar3_1;
    }
}

namespace UserDefinedLiterals {
    // Integral literals are always unsigned -- '-' is not part of the literal
    long long operator ""_km(unsigned long long lv) {
        assert(lv < std::numeric_limits<long long>::max());
        std::cout << "Value of the literal = " << lv << '\n';
        return lv;
    }
    
    void main() {
        // '-' before the literal is a unary minus operator
        std::cout << -0xFF'FF'FF'FF'FF'FF'FF'FF_km << '\n';
        
        // implicit unsigned -> signed conversion yields the wrong result
        std::cout << -0xFF'FF'FF'FF'FF'FF'FF'FFULL << '\n';
        
        // another funny implicit conversion
        std::cout << std::boolalpha << (1U < -1) << '\n'; // prints "true"
    }
}

namespace Whaa {
    template<typename Func>
    auto dirac_delta(const Func& f) {
        return f(0);
    }
    
    double f(double x) {
        return std::exp(x);
    }
    
    void main() {
        std::cout << dirac_delta(f) << '\n';
    }
}

namespace Predefined {
    void main() {
        std::cout << "Standard:\n";
        
        std::cout << "__FILE__ = " << __FILE__ << '\n';
        std::cout << "__LINE__ = " << __LINE__ << '\n';
        std::cout << "__func__ = " << __func__ << '\n';
        
        std::cout << "Non-standard:\n";

#ifdef __PRETTY_FUNCTION__
        std::cout << "__PRETTY_FUNCTION__ = " << __PRETTY_FUNCTION__ << '\n';
#endif

#ifdef __FUNCTION__
        std::cout << "__FUNCTION__ = " << __FUNCTION__ << '\n';
#endif
    }
}

namespace Threads {
    constexpr int count_to{1'000'000'000};
    constexpr int num_threads{8};
    constexpr int thread_count_to{count_to / num_threads};
    
    void count_st() {
        auto count = std::make_unique<int>(0);
        
        for (int i{0}; i < count_to; ++i) {
            ++*count;
        }
        
        std::cout << "Counted to " << *count << " using 1 thread!\n";
    }
    
    void count_st_async() {
        auto count = std::make_unique<int>(0);
        auto f = std::async([](int* count_ptr) {
            for (int i{0}; i < count_to; ++i) {
                ++*count_ptr;
            }
        }, count.get());
        
        f.wait();
        
        std::cout << "Counted to " << *count << " using 1 async thread!\n";
    }
    
    template<typename Func, typename... ArgTypes>
    void run_and_wait(const int num_threads, Func&& func, ArgTypes&& ... args) {
        std::vector<std::future<void>> fs;
        
        for (int i{0}; i < num_threads; ++i) {
            fs.push_back(std::async(std::forward<Func>(func), std::forward<ArgTypes>(args)...));
        }
        
        for (const auto& f : fs) {
            f.wait();
        }
    }
    
    void count_mt_atomic() {
        auto count = std::make_unique<std::atomic_int>(0);
        auto counter_lambda = [](std::atomic_int* count_ptr) {
            for (int i{0}; i < thread_count_to; ++i) {
                ++*count_ptr;
            }
        };
        
        run_and_wait(num_threads, counter_lambda, count.get());
        
        std::cout << "Counted to " << *count << " using " << num_threads << " threads and an atomic variable!\n";
    }
    
    void count_mt_lock() {
        auto count = std::make_unique<int>(0);
        auto lock = std::make_unique<std::mutex>();
        auto counter_lambda = [](int* count_ptr, std::mutex* lock_ptr) {
            for (int i{0}; i < thread_count_to; ++i) {
                std::lock_guard<std::mutex> lg{*lock_ptr};
                ++*count_ptr;
            }
        };
        
        run_and_wait(num_threads, counter_lambda, count.get(), lock.get());
        
        std::cout << "Counted to " << *count << " using " << num_threads << " threads and a normal variable guarded by a lock!\n";
    }
    
    void main(const int argc, const char* argv[]) {
        auto print_error = []() { std::cout << "Bad args\n"; };
        
        if (argc != 2) {
            print_error();
            return;
        }
        
        switch (std::atoi(argv[1])) {
            case 1:count_st();
                break;
            case 2:count_st_async();
                break;
            case 3:count_mt_atomic();
                break;
            case 4:count_mt_lock();
                break;
            default:print_error();
                return;
        }
    }
}

namespace NiceFunctions {
    template<typename E>
    auto uniform_distribution(E from, E to, std::true_type) {
        return std::uniform_int_distribution<E>(from, to);
    }
    
    template<typename E>
    auto uniform_distribution(E from, E to, std::false_type) {
        return std::uniform_real_distribution<E>(from, to);
    }
    
    template<typename E>
    std::vector<E> make_rnd_vals(size_t length, E from, E to) {
        static_assert(std::is_arithmetic<E>(), "E must be an arithmetic type");
        auto rnd = std::bind(
                uniform_distribution(from, to, std::is_integral<E>()),
                std::default_random_engine(std::random_device()())
        );
        std::vector<E> v;
        
        v.reserve(length);
        
        for (size_t i = 0; i < length; ++i) {
            v.push_back(rnd());
        }
        
        return v;
    }
    
    template<typename E>
    std::vector<E> make_decreasing_vals(size_t length, E from) {
        std::vector<int> v;
        v.reserve(length);
        while (length--) {
            v.push_back(from--);
        }
        return v;
    }
    
    template<typename E>
    std::ostream& operator<<(std::ostream& os, const std::vector<E>& v) {
        os << '[';
        const size_t l = v.size();
        const size_t l1 = l - 1;
        for (size_t i = 0; i < l; ++i) {
            os << v[i];
            if (i != l1) {
                os << ", ";
            }
        }
        os << ']';
        return os;
    }
    
    // Action is a no-arg function (return value is ignored)
    // Returns nanoseconds
    template<typename Action>
    int64_t time(const Action& action) {
        auto start = std::chrono::high_resolution_clock::now();
        
        action();
        
        auto end = std::chrono::high_resolution_clock::now();
        
        return std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
    }
    
    // Computes n to the power of k recursively
    template<typename T>
    constexpr T ipow_r(T n, int k) {
        return k < 0 ? 0
                     : n == 1 || k < 1 ? 1
                                       : k & 1 // k is odd
                                         ? n * ipow_r(n * n, k >> 1)
                                         : ipow_r(n * n, k >> 1);
    }
    
    // Computes n to the power of k (tail-call optimized)
    template<typename T>
    constexpr T ipow_tr(T n, int k, int64_t result = 1) {
        return k < 0 ? 0
                     : n == 1 || k < 1 ? result
                                       : ipow_tr(n * n, k >> 1, k & 1 ? n * result : result);
    }
    
    // Computes n to the power of k
    constexpr int64_t ipow(int64_t n, int k) {
        return ipow_tr(n, k);
    }
    
    void main() {
        using pow_t = std::function<int64_t(int64_t, int)>;
        
        const pow_t f1 = [](int64_t n, int k) { return ipow_r(n, k); };
        const pow_t f2 = [](int64_t n, int k) { return ipow_tr(n, k); };
        
        for (auto f : {f1, f2}) {
            std::cout << f(2, 62) << '\n';
            std::cout << f(10, 18) << '\n';
            std::cout << f(12, 3) << '\n';
            std::cout << f(0, 0) << '\n';
            std::cout << f(1, 0) << '\n';
            std::cout << f(0, 1) << '\n';
            std::cout << f(-1, 1) << '\n';
            std::cout << f(-1, 0) << "\n\n";
        }
    }
}

namespace Quicksort {
    template<typename E>
    void swap(E& a, E& b) {
        const E t = std::move(a);
        a = std::move(b);
        b = std::move(t);
    }
    
    template<typename Container>
    int hoare_partition(Container& a, int start, int end) {
        assert(start < end);
        
        const auto pivot = a[start + ((end - start) >> 1)];
        
        --start;
        ++end;
        while (true) {
            while (a[++start] < pivot) {}
            while (a[--end] > pivot) {}
            if (start >= end) {
                return end;
            }
            swap(a[start], a[end]);
        }
    }
    
    template<typename Container>
    void quicksort(Container& a, const int start, const int end) {
        const int length = end - start + 1;
        
        if (length < 2) {
            return;
        }
        
        if (length == 2) {
            if (a[start] > a[end]) {
                swap(a[start], a[end]);
            }
            return;
        }
        
        const int p = hoare_partition<Container>(a, start, end);
        quicksort<Container>(a, start, p);
        quicksort<Container>(a, p + 1, end);
    }
    
    template<typename Container>
    void quicksort(Container& a) {
        quicksort(a, 0, a.size() - 1);
    }
    
    void main() {
        using namespace NiceFunctions;
        
        constexpr int min = -1000;
        constexpr int max = 1000;
        
        auto v = make_rnd_vals<int32_t>(100'000'000, min, max);
        //std::vector<int> v = make_bad_ints(1000);
        
        if (v.size() <= 100) {
            std::cout << v << '\n';
        }
        
        int64_t duration = time([&]() {
            quicksort(v);
        });
        
        std::cout << duration << '\n';
        
        if (v.size() <= 100) {
            std::cout << v << '\n';
        }
    }
}

// For small arrays, quicksort is faster.
// Empirically, the cutoff for arrays of int16_t is around 3000 elements, which agrees with Java's hardcoded cutoff of 3200
// Though, if min/max is specified and significantly smaller than maximum range of Iter's value type, then count sort may still be faster
// For example, for 1000 `int16_t`s from -1000 to 1000 count sort is 2.5x faster
namespace CountSort {
    // This is either a stack or an int
    template<typename ElemType, bool IsIntegral = std::is_integral<ElemType>::value>
    struct Counter {};
    
    // Integral element type specialization
    template<typename ElemType>
    struct Counter<ElemType, true> {

        auto count() { return m_count; }
        
        void slurp(ElemType) { ++m_count; }
        
        void barf(ElemType& el, size_t j, int32_t min) { el = (int32_t) j + min; }

    private:

        int64_t m_count = 0;
    
    };
    
    // Any element type specialization
    template<typename ElemType>
    struct Counter<ElemType, false> {

        auto count() { return m_stack.size(); }
        
        void slurp(ElemType& el) { m_stack.push(std::move(el)); }
        
        void barf(ElemType& el, int32_t, int32_t) {
            el = std::move(m_stack.top());
            m_stack.pop();
        }

    private:

        std::stack<ElemType> m_stack;

    };
    
    // Iter must be bidirectional (support the decrement operator).
    //
    // MapToInt is a key function (iter's value type -> int).
    //
    // min/max is the minimum/maximum value that MapToInt returns.
    // If not specified or invalid (max <= min), performs additional linear search.
    //
    // The sort is stable (preserves the order of elements with the same key).
    template<
        typename Iter, 
        typename MapToInt,
        typename ElemType = typename std::iterator_traits<Iter>::value_type
    >
    void count_sort(Iter begin, Iter end, MapToInt map_to_int, int32_t min, int32_t max) {
        
        // input array is empty
        if (begin == end) {
            return;
        }
        
        // find min/max if not specified or invalid
        if (max <= min) {
            Iter it = begin;
            min = map_to_int(*it);
            max = map_to_int(*it);
            for (++it; it != end; ++it) {
                int m = map_to_int(*it);
                max = std::max(max, m);
                min = std::min(min, m);
            }
        }
        
        // all elements are equal
        if (max == min) {
            return;
        }

        const size_t diff = (int64_t) max - min;
        std::vector<Counter<ElemType>> counters(diff + 1);
        
        Iter it = begin;
        for (; it != end; ++it) {
            ElemType& e = *it;
            counters[(int64_t) map_to_int(e) - min].slurp(e);
        }
        // remember that `it` is at `end`
        
        size_t j = counters.size();
        while (it != begin) {
            // Skip empty
            auto count = counters[--j].count();
            while (count == 0) {
                count = counters[--j].count();
            }
            
            // The initial order of values with the same key is preserved by the counter
            while (count-- > 0) {
                counters[j].barf(*(--it), j, min);
            }
        }
    }
    
    // Overload for integral element type
    template<typename Iter, typename E = typename std::iterator_traits<Iter>::value_type>
    typename std::enable_if_t<std::is_integral<E>::value> count_sort(Iter begin, Iter end, int32_t min, int32_t max) {
        count_sort(begin, end, [](E x) { return x; }, min, max);
    }

    // This overload for integral element type finds min/max
    template<typename Iter, typename E = typename std::iterator_traits<Iter>::value_type>
    typename std::enable_if_t<std::is_integral<E>::value> count_sort(Iter begin, Iter end) {
        count_sort(begin, end, [](E x) { return x; }, 0, 0);
    }

    // This overload finds min/max
    template<typename Iter, typename MapToInt>
    void count_sort(Iter begin, Iter end, MapToInt map_to_int) {
        count_sort(begin, end, map_to_int, 0, 0);
    }
}

namespace CountSortTests {
    void versus() {
        using namespace NiceFunctions;
        using namespace CountSort;
        
        using elem_type = int32_t;
        using array_type = std::vector<elem_type>;
        
        constexpr elem_type min = -100000;
        constexpr elem_type max = 100000;
        constexpr size_t count = 10'000'000;
        
        std::function<void(array_type*)> l_countsort = [=](array_type* pv) {
            count_sort(std::begin(*pv), std::end(*pv), min, max);
        };
        
        std::function<void(array_type*)> l_stdsort = [=](array_type* pv) {
            std::sort(pv->begin(), pv->end());
        };
        
        auto v1 = make_rnd_vals<elem_type>(count, min, max);
        auto v2 = v1;
        for (auto& t : { std::make_tuple(&v1, l_countsort),
                         std::make_tuple(&v2, l_stdsort)    }) {
            auto pv = std::get<0>(t);
            auto f = std::get<1>(t);
            
            if (pv->size() <= 100) {
                std::cout << *pv << '\n';
            }
            
            int64_t duration = time([&]() {
                f(pv);
            });
            
            std::cout << duration << "ns" << '\n';
            
            if (pv->size() <= 100) {
                std::cout << *pv << '\n';
            }
        }
    }
    
    struct Task {
        int priority;
        std::string name;
        
        Task(Task&& other) noexcept {
            priority = other.priority;
            name = std::move(other.name);
            std::cerr << "task move-constructed\n";
        };
        
        Task& operator=(Task&& other) noexcept {
            priority = other.priority;
            name = std::move(other.name);
            std::cerr << "task move-assigned\n";
            return *this;
        }
        
        Task(Task& other) {
            assert(false);
            std::cerr << "task copy-constructed\n";
        }
        
        Task& operator=(const Task& other) {
            assert(false);
            std::cerr << "task copy-assigned\n";
            return *this;
        }
        
        Task(int priority, std::string name) : priority(priority), name(std::move(name)) {
            std::cerr << "task constructed\n";
        } // TODO research this trick: accepting string by value and moving
    };

// For some reason, after uncommmeting this, another overload of operator<< defined in NiceFunctions breaks :(
//     std::ostream& operator<<(std::ostream& os, const Task& t) {
//         os << "{priority=" << t.priority << ", name='" << t.name << "'}";
//         return os;
//     }
    
    void sort_anything() {
        using namespace NiceFunctions;
        using namespace CountSort;
        
        std::vector<Task> ts;
        ts.emplace_back(42, "One");
        ts.emplace_back(100500, "Three");
        ts.emplace_back(100500, "Four");
        ts.emplace_back(42, "Two");
        
        for (const auto& t : ts) {
            std::cout << '{' << t.priority << ' ' << t.name << "}, ";
        }
        std::cout << '\n';
        
        std::cerr << '\n';
        count_sort(begin(ts), end(ts), [](const Task& p) { return p.priority; }, 42, 100500);
        
        // verify sort stability
        for (const auto& t : ts) {
            std::cout << '{' << t.priority << ' ' << t.name << "}, ";
        }
        std::cout << '\n';
    }
}

namespace MapExperiments {
    void main() {
        std::map<int, std::string> m;
        m[15] = "15";
        m[-1] = "-1";
        m[22] = "22";
        m[-1234] = "-1234";
        
        for (auto& kv : m) {
            std::get<1>(kv) += "!"; // std::get returns a reference
        }
        
        for (const auto& kv : m) {
            std::cout << std::get<1>(kv) << '\n';
        }
    }
}

namespace Newton {
    // Assuming Func is a function TVal -> TVal that is non-decreasing on (from, to) and crosses zero on (from, to),
    // finds a value R such that |f(R)| < tolerance
    template<typename Func, typename TVal>
    TVal find_root(const Func& f, TVal from, TVal to, TVal tolerance = std::numeric_limits<TVal>::epsilon()) {
        static_assert(std::is_same<decltype(f(from)), TVal>::value);
        assert(from < to);
        assert(tolerance > 0);
        assert(f(from) < 0);
        assert(f(to) > 0);
        
        const TVal two = 2;
        const TVal mid = (from + to) / two;
        const TVal f_mid = f(mid);
        
        if (std::abs(f_mid) < tolerance) {
            return mid;
        }
        
        return find_root(
                f,
                f_mid > 0 ? from : mid,
                f_mid > 0 ? mid : to,
                tolerance
        );
    }
    
    void main() {
        // CLRS 1.2-2
        std::cout << find_root([](double x) { return 8 * x * x / (64 * x * std::log2(x)) - 1; }, 2., 1000.) << '\n';
    }
}

namespace MergeSort {
    constexpr size_t InsertionSortThreshold = 46;
    
    // Fallback to this if array length is lte InsertionSortThreshold
    // Generally the best algorithm for small arrays
    // InsertionSortThreshold is machine- and implementation- dependent
    template<typename Iter>
    void insertion_sort(Iter begin, Iter end) {
        auto length = end - begin;
        
        if (length < 2) {
            return;
        }
        
        using it_diff_t = decltype(length);
        
        // The loop invariant is that array is always of the form [s1, s2, ..., sN, u1, u2, ..., uN]
        // where s1..sN are sorted, and u1..uN are unsorted
        // i points to the last element in the sorted subarray
        // j runs through unsorted elements
        for (it_diff_t j = 1; j < length; ++j) {
            auto jth = std::move(begin[j]); // stage jth element
            auto i = j - 1;
            for (; i >= 0 && begin[i] > jth; --i) { // for each element in the sorted array that is gt jth
                begin[i + 1] = std::move(begin[i]); // move it to the right
            }
            begin[i + 1] = std::move(jth); // move jth to its position in the sorted array
            // Note to self -- resist the temptaion to rewrite this with std::swap -- it's 3x slower, b/c it does 3x more moves!
        }
    }
    
    // Iter must be a random-access mutable iterator
    template<typename Iter>
    void merge_sort(Iter begin, Iter end) {
        const auto length = std::distance(begin, end);
        
        if (length <= InsertionSortThreshold) {
            insertion_sort(begin, end);
            return;
        }
        
        const auto mid = begin + length / 2;
        merge_sort(begin, mid);
        merge_sort(mid, end);
        
        std::inplace_merge(begin, mid, end);
    }
    
    void main() {
        using namespace NiceFunctions;
        
        auto v = make_rnd_vals(100'000'000, -100'000'000, 100'000'000);
        if (v.size() <= 100) {
            std::cout << v << '\n';
        }
        
        int64_t duration = time([&]() {
//            merge_sort(v.begin(), v.end());
//            std::stable_sort(v.begin(), v.end());
            std::sort(v.begin(), v.end());
        });
        
        std::cout << duration << '\n';
        
        if (v.size() <= 100) {
            std::cout << v << '\n';
        }
    }
}

namespace StaticInBlock {
    void inc_and_print() {
        static int counter = 0;
        ++counter;
        std::cout << counter << '\n';
    }
    
    void inc_and_print_block() {
        {
            static int counter = 0;
            ++counter;
            std::cout << counter << '\n';
        }
    }
    
    void count_and_print(int count_to) {
        goto outside;
        
        {
            static int counter = 0;
            
            inc_counter:
            ++counter;
            goto outside;
            
            print:
            std::cout << counter << '\n';
            return;
        }
        
        outside:
        if (count_to--) {
            goto inc_counter;
        }
        goto print;
    }
    
    void main() {
        count_and_print(10);
        count_and_print(10);
        inc_and_print();
        inc_and_print();
        inc_and_print_block();
        inc_and_print_block();
    }
}

namespace Kadane {
    // CLRS 4.1-5
    // Iter must be a forward iterator. Elements msut support general arithmetic and comparison operators.
    // Returns a tuple (max_begin, max_end, max_sum), where [max_begin, max_end) is the maximum subarray, and max_sum is its sum.
    // !If the input array is empty, behavior is undefined!
    template<typename Iter>
    auto max_sub_array(Iter begin, Iter end) {
        assert(begin != end);
        
        // "Global" maximum
        auto max_begin = begin;
        auto max_end = begin;
        auto max_sum = *begin;
        
        // "Current" maximum (maximum subarray ending at current value of c_max_end)
        auto c_max_begin = max_begin;
        auto c_max_end = max_end;
        auto c_max_sum = max_sum;
        
        while (++c_max_end != end) {
            auto sum_to_c_max_end = c_max_sum + *c_max_end;
            
            if (*c_max_end > sum_to_c_max_end) {
                c_max_sum = *c_max_end;
                c_max_begin = c_max_end;
            }
            else {
                c_max_sum = sum_to_c_max_end;
            }
            
            if (c_max_sum > max_sum) {
                max_begin = c_max_begin;
                max_end = c_max_end;
                max_sum = c_max_sum;
            }
        }
        
        // Drop leading zeros
        while (max_begin != max_end && max_sum + *max_begin == max_sum) {
            ++max_begin;
        }
        // Increment max_end to comply with [begin, end) iterators contract
        ++max_end;
        return std::make_tuple(max_begin, max_end, max_sum);
    }
    
    template<typename Container, typename Iter, typename T>
    void print(const Container& c, std::tuple<Iter, Iter, T> max_sub_array) {
        std::cout << '('
                  << std::get<0>(max_sub_array) - c.begin() << ", "
                  << std::get<1>(max_sub_array) - c.begin() - 1 << ", "
                  << std::get<2>(max_sub_array)
                  << ")\n";
    }
    
    void main() {
        const std::vector<int> v1{-13, -3, -25, 20, -3, -16, -23, 18, 20, -7, 12, -5, -22, 15, -4, 7};
        print(v1, max_sub_array(v1.begin(), v1.end()));
        
        const std::vector<int> v2{-13, -3, -25, -4, -7};
        print(v2, max_sub_array(v2.begin(), v2.end()));
        
        const std::vector<int> v3{3, 2, 1, 0, 0, 25};
        print(v3, max_sub_array(v3.begin(), v3.end()));
        
        const std::vector<int> v4{0, 0, 0, 12, -10, 11, 0, 0, 0};
        print(v4, max_sub_array(v4.begin(), v4.end()));
        
        const std::vector<int> v5{0, 0, 0, 0};
        print(v5, max_sub_array(v5.begin(), v5.end()));
        
        const std::vector<int> v6{0, 0, -1, 0, 0, 1};
        print(v6, max_sub_array(v6.begin(), v6.end()));
    }
}

namespace FloatingPointMadness {
    template<typename T>
    struct C {
        T re;
        T im;
        
        C(T re, T im) : re{re}, im{im} {}
    };
    
    template<typename T>
    constexpr C<T> mul4(C<T> a, C<T> b) {
        const T re = a.re * b.re - a.im * b.im;
        const T im = a.im * b.re + a.re * b.im;
        return {re, im};
    }
    
    template<typename T>
    constexpr C<T> mul3(C<T> a, C<T> b) {
        const T p1 = a.re * b.re;
        const T p2 = a.im * b.im;
        const T p3 = (a.re + a.im) * (b.re + b.im);
        a.re = p1 - p2;
        a.im = p3 - p2 - p1;
        return a;
    }
    
    template<typename T>
    std::vector<C<T>> make_data(size_t length, T m) {
        auto rnd = std::bind(
                std::uniform_real_distribution<T>(-m, m),
                std::default_random_engine(std::random_device()())
        );
        std::vector<C<T>> v;
        v.reserve(length);
        while (length--) {
            v.emplace_back(rnd(), rnd());
        }
        return v;
    }
    
    // Different ways to compare floats
    template<typename T>
    bool eq(T a, T b) {
        if (a == b) { return true; } // Shortcut, handles infinities
        if (a - b == 0) { return true; } // Shortcut if subnormals are disabled
        if (std::isnan(a) || std::isnan(b)) { return false; } // NaN != anything, including itself
        if (std::signbit(a) != std::signbit(b)) { return false; } // Different signs
        
        const union { T f; int64_t i; } ia = {a};
        const union { T f; int64_t i; } ib = {b};
        // This (ia.i) is undefined behavior in C++, but YOLO
        if (std::abs(ia.i - ib.i) <= 1) { return true; } // 0..1 ULP difference
        
        const auto d = std::abs(a - b); // Note: debugging shows that by this time the difference is almost always 1..5 * epsilon
        if (d < std::numeric_limits<T>::min()) { return true; } // Subnormal difference => equal
        
        const auto r = a / b;
        if (std::abs(1 - r) < std::numeric_limits<T>::epsilon()) { return true; } // Relation within an epsilon of 1 => equal
        
        return false;
    }
    
    void main() {
        using T = double;
        
        const auto c1 = C<T>(T(2), T(3));
        const auto v = make_data(100'000'000, T(10e10));
        
        size_t neq_count = 0;
        
        auto t = NiceFunctions::time([&]() {
            for (auto c2 : v) {
                auto p1 = mul3(c1, c2);
                auto p2 = mul4(c1, c2);
                if (!(eq(p1.re, p2.re) && eq(p1.im, p2.im))) {
                    ++neq_count;
                }
            }
        });
        
        std::cout << "neq: " << neq_count
                  << " (" << std::lround(neq_count * 100.0 / v.size()) << "%)\n"
                  << t / 10e6 << "ms"
                  << '\n';
    }
}

int main(const int argc, const char* argv[]) {
    CountSortTests::versus();
    CountSortTests::sort_anything();
//    MapExperiments::main();
//    Newton::main();
//    StaticInBlock::main();
//    MergeSort::main();
//    Kadane::main();
//    FloatingPointMadness::main();
}
