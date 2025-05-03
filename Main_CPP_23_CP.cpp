/*! BENCHMARK: 
- Big int numbers: 8-bit to 64-bit (max negative and max positive)
- Extreme floating: __float128 or double (auto detect, if 128bit supported, use __float128; else use double)
- Char and String: Classic, Wide, ultra-wide, coloring with ANSI and 8-bit RGB (std::string with fmt)
- Stack and Heap Memory
- Smart memory: Unique, Shared and Weak
- Mem-frag: Using for loop to allocate and deallocate quickly, if mem-frag detected, stop test, print "Mem frag detected! Stopping now" then move to next test
- Threading & Mutex: Multi-thread (4 cores), `for` loop, Memory access, Mutex lock/unlock, Measure total time and CPU usage
- Numbers Brute-forcing: 6 digits, each digit is from 0 to 9
*/

/*! ATTENTION!
* THIS IS THE CP "CROSS PLATFORM" VERSION OF `Main_CPP_23.cpp`
* YOU MAY USE THIS IF YOU'RE ON LINUX
*/


#define NOMINMAX

//! TODO: Add Terminal size for Linux

#include <cstdio>
#include <cstring>
#include <expected>
#include <limits>
#include <iostream>
#include <chrono>
#include <vector>
#include <string>
#include <memory>
#include <map>
#include <sstream>
#include <iomanip>
#include <cstdint>
#include <sstream>
#include <atomic>
#include <thread>
#include <future>
#include <random>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <cstddef>
#include <mutex>

/* OS Works */

// Platform Detection
#if defined(_WIN32) || defined(WIN32)
    #define OS_WINDOWS
    #if defined(__clang__)
        #define COMPILER "Clang"
        #include <windows.h>
        #include <malloc.h>
        //! ACHTUNG: Redefine DWORD will cause warning!
        // using DWORD = unsigned int;
        
    #elif defined(_MSC_VER)
        #define FMT_HEADER_ONLY // Required for fmt in header-only mode
        #define COMPILER "MSVC"
        #include <Windows.h>
        //! ACHTUNG: Redefine DWORD will cause warning!
        // using DWORD = unsigned int;

    #else
        #define COMPILER "Unknown-Win"
        #include <windows.h>
    #endif
#elif defined(__linux__) || defined(__unix__)
    #define OS_LINUX
    #define COMPILER "Clang/GCC/Linux"
    #include <sys/ioctl.h>
    #include <unistd.h>
    #include <termios.h>
#endif

#include <fmt/core.h>  // For fmt::print / println if needed

/**
 * @brief Get terminal dimensions (columns/rows) cross-platform.
 * 
 * @param COR "X" = columns, "Y" = rows, "D" = debug print
 * @param offset Optional padding
 * @return int Terminal dimension or error (-1)
 */
int TerminalSize(const char* COR = "X", int offset = 0) {
    #ifdef OS_WINDOWS
        CONSOLE_SCREEN_BUFFER_INFO csbi;
        if (!GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi)) {
            std::fprintf(stderr, "Failed to retrieve console screen buffer info.\n");
            return -1;
        }

        int columns = csbi.srWindow.Right - csbi.srWindow.Left + 1;
        int rows = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;

        if (std::strcmp(COR, "D") == 0) {
            std::printf("Columns: %d\n", columns);
            std::printf("Rows: %d\n", rows);
            return 0;
        } else if (std::strcmp(COR, "X") == 0) {
            return columns + offset;
        } else if (std::strcmp(COR, "Y") == 0) {
            return rows + offset;
        }
    #elif defined(OS_LINUX)
        struct winsize ws;
        if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) != 0) {
            std::perror("ioctl failed");
            return -1;
        }

        int columns = ws.ws_col;
        int rows = ws.ws_row;

        if (std::strcmp(COR, "D") == 0) {
            std::printf("Columns: %d\n", columns);
            std::printf("Rows: %d\n", rows);
            return 0;
        } else if (std::strcmp(COR, "X") == 0) {
            return columns + offset;
        } else if (std::strcmp(COR, "Y") == 0) {
            return rows + offset;
        }
    #endif

    return -1; // Invalid option
}

/**
 * @brief Get the current process ID in a cross-platform way.
 * 
 * @return std::expected<uint32_t, int> On success: PID; On failure: error code
 */
std::expected<uint32_t, int> GetPID() {
    #ifdef OS_WINDOWS
    #include <windows.h>
        DWORD pid = GetCurrentProcessId();
        return static_cast<uint32_t>(pid);
    #elif defined(OS_LINUX)
        pid_t pid = getpid();
        if (pid < 0) {
            return std::unexpected<int>(errno); // Return error code if failed
        }
        return static_cast<uint32_t>(pid);
    #else
        return std::unexpected<int>(-1); // Unsupported platform
    #endif
}

/* End OS Works */

#include <fmt/core.h>
#include <fmt/chrono.h>
#include <fmt/color.h>
#include <format>

using namespace std::chrono_literals;
using str = std::string;

// Styler Namespace for Colored Output
namespace Styler_fmt {
    inline str bold(const str& txt) { return "\x1b[1m" + txt + "\x1b[22m"; }
    inline str italic(const str& txt) { return "\x1b[3m" + txt + "\x1b[23m"; }
    inline str under(const str& txt) { return "\x1b[4m" + txt + "\x1b[24m"; }
    inline str strike(const str& txt) { return "\x1b[9m" + txt + "\x1b[29m"; }

    inline void hex_to_rgb(uint32_t hex, int& r, int& g, int& b) {
        r = (hex >> 16) & 0xFF;
        g = (hex >> 8) & 0xFF;
        b = hex & 0xFF;
    }

    inline str ColorFG(const str& txt, uint32_t hex = 0x89E67D, int alpha = 100) {
        int r, g, b;
        hex_to_rgb(hex, r, g, b);
        float a = alpha / 100.0f;
        r = static_cast<int>(r * a);
        g = static_cast<int>(g * a);
        b = static_cast<int>(b * a);
        return fmt::format("\x1b[38;2;{};{};{}m{}\x1b[39m", r, g, b, txt);
    }

    inline str ColorBG(const str& txt, uint32_t hex, int alpha = 100) {
        int r, g, b;
        hex_to_rgb(hex, r, g, b);
        float a = alpha / 100.0f;
        r = static_cast<int>(r * a);
        g = static_cast<int>(g * a);
        b = static_cast<int>(b * a);
        return fmt::format("\x1b[48;2;{};{};{}m{}\x1b[49m", r, g, b, txt);
    }

    inline str styler_all(
        const str& text,
        uint32_t fg = 0xFFFFFF,
        uint32_t bg = 0x000000,
        bool is_bold = false,
        bool is_italic = false,
        bool is_underline = false,
        bool is_strike = false,
        int alpha = 100
    ) {
        str styled = text;
        if (is_strike) styled = strike(styled);
        if (is_underline) styled = under(styled);
        if (is_italic) styled = italic(styled);
        if (is_bold) styled = bold(styled);

        styled = ColorFG(styled, fg, alpha);
        if (bg != 0x000000)
            styled = ColorBG(styled, bg, alpha);

        return styled + "\x1b[0m";
    }

    inline void view_raw(const str& s) {
        std::cout << "DEBUG RAW: ";
        for (char c : s) {
            if (std::isprint(c))
                std::cout << c;
            else
                std::cout << "\\x" << std::hex << (int)(uint8_t)c;
        }
        std::cout << "\n";
    }
}

// ===== BENCHMARKS =====

class Program {
public:
    // Benchmark Integer Types
    class BigInteger {
    public:
        void main() {
            fmt::println("Benchmark: {}{}", Styler_fmt::ColorFG("Big Integers", 0xffa500), "");

            auto start = std::chrono::high_resolution_clock::now();

            int64_t i8 = INT8_MAX, i16 = INT16_MAX, i32 = INT32_MIN, i64 = INT64_MIN;
            uint64_t u8 = UINT8_MAX, u16 = UINT16_MAX, u32 = UINT32_MAX, u64 = UINT64_MAX;

            // Do something with them to prevent optimization
            i8 -= 1; u8 += 1;
            i16 *= -1; u16 += 1;
            i32 /= -1; u32 *= 2;
            i64 %= 999999; u64 >>= 1;

            auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
            fmt::println(" > Time taken: {} ns", duration);
        }
    };

    // Benchmark Floating Points
    class XtremeFloat {
        public:
        void main() {
            fmt::println("Benchmark: {}{}", Styler_fmt::ColorFG("Extreme Floats", 0xff4500), "");
    
            auto start = std::chrono::high_resolution_clock::now();
    
            // Ensure we undefine the Windows macros before using numeric_limits
            auto FLT_MAX_val = std::numeric_limits<float>::max();
            auto DBL_MAX_val = std::numeric_limits<long double>::max();
    
            using fp = double; // Fallback to double if __float128 not supported
            fp f1 = static_cast<fp>(FLT_MAX_val) / 2;
            fp f2 = static_cast<fp>(DBL_MAX_val) / 1000;
            fp f3 = fp(1) / 3;
            volatile fp result = f1 + f2 * f3;
    
            (void)result; // Prevent unused warning
    
            auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
            fmt::println(" > Time taken: {} ns", duration);
        }
    };

    // Benchmark Strings and ANSI Coloring
    class Strings {
    public:
        void main() {
            fmt::println("Benchmark: {}{}", Styler_fmt::ColorFG("Strings & ANSI", 0x00ccff), "");

            auto start = std::chrono::high_resolution_clock::now();

            // char cstr[] = "classic string";
            // wchar_t wcstr[] = L"wide string";
            // char8_t utf8str[] = u8"UTF-8 string";

            // ANSI coloring via fmt
            /**! WARNING:
             * no instance of constructor "std::__1::basic_string<_CharT, _Traits, _Allocator>::basic_string [with _CharT=char, _Traits=std::__1::char_traits<char>, _Allocator=std::__1::allocator<char>]" matches the argument listC/C++(289)
             * main.cpp(174, 55): argument types are: (char8_t [13])
             */
            char cstr[] = "classic string";
            wchar_t wcstr[] = L"wide string";
            char16_t utf8str[] = u"UTF-8 string";
            
            // ANSI coloring via fmt
            fmt::println("{}", Styler_fmt::styler_all(std::string(reinterpret_cast<const char*>(utf8str)), 0xa0522d, 0xffffff, true, false, true, false, 100));
            fmt::println("{}", Styler_fmt::styler_all("ANSI colored string", 0x00ff00, 0x0000ff, true, true, true, false, 100));

            auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
            fmt::println(" > Time taken: {} ns", duration);
        }
    };

    // Stack Allocation Benchmark
    class StackBench {
    public:
        void main() {
            fmt::println("Benchmark: {}{}", Styler_fmt::ColorFG("Stack Allocations", 0x9acd32), "");

            auto start = std::chrono::high_resolution_clock::now();

            constexpr size_t N = 1'000'000;
            for (size_t i = 0; i < N; ++i) {
                int stackVal = i % 100;
                (void)stackVal;
            }

            auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
            fmt::println(" > Stack allocations time: {} ns", duration);
        }
    };

    // Heap Allocation Benchmark
    class HeapBench {
    public:
        void main() {
            fmt::println("Benchmark: {}{}", Styler_fmt::ColorFG("Heap Allocations", 0x8A2BE2), "");

            auto start = std::chrono::high_resolution_clock::now();

            constexpr size_t N = 1'000'000;
            for (size_t i = 0; i < N; ++i) {
                auto ptr = new int(i);
                delete ptr;
            }

            auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
            fmt::println(" > Heap allocations time: {} ns", duration);
        }
    };

    // Smart Pointer Benchmark
    class Smarts {
    public:
        void main() {
            fmt::println("Benchmark: {}{}", Styler_fmt::ColorFG("Smart Pointers", 0xff69b4), "");

            auto start = std::chrono::high_resolution_clock::now();

            for (int i = 0; i < 100'000; ++i) {
                auto uptr = std::make_unique<int>(i);
                auto sptr = std::make_shared<int>(i);
                std::weak_ptr<int> wptr = sptr;
                (void)uptr; (void)sptr; (void)wptr.lock();
            }

            auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
            fmt::println(" > Smart pointer operations time: {} ns", duration);
        }
    };

    // Memory Fragmentation Test
    class MemFragTest {
    public:
        void main() {
            fmt::println("Benchmark: {}{}", Styler_fmt::ColorFG("Memory Fragmentation", 0xff4500), "");

            std::vector<void*> allocations;

            auto start = std::chrono::high_resolution_clock::now();
            for (size_t i = 0; i < 10'000'000; ++i) {
                void* p = malloc(16);
                if (!p) {
                    fmt::println("Mem frag detected! Stopping now");
                    break;
                }
                allocations.push_back(p);
                if (i % 10 == 0) {
                    free(allocations.back());
                    allocations.pop_back();
                }
            }
            for (auto p : allocations) free(p);

            auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
            fmt::println(" > Memory fragmentation test completed in: {} ns", duration);
        }
    };

    // Threading and Mutex Benchmark
    class ThreadMutex {
        std::mutex mtx;
        std::atomic<bool> done = false;
        std::vector<int> shared_data;

        void worker() {
            for (int i = 0; !done.load(); i++) {
                std::lock_guard<std::mutex> lock(mtx);
                shared_data.push_back(i);
            }
        }

    public:
        void main() {
            fmt::println("Benchmark: {}{}", Styler_fmt::ColorFG("Threading & Mutex", 0xff00ff), "");

            auto start = std::chrono::high_resolution_clock::now();

            std::vector<std::thread> workers;
            for (int i = 0; i < 4; ++i) {
                workers.emplace_back([this]() { worker(); });
            }

            std::this_thread::sleep_for(2s); // Run for 2 seconds
            done.store(true);

            for (auto& t : workers) t.join();

            auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
            fmt::println(" > Total thread operations: {}", shared_data.size());
            fmt::println(" > Thread execution time: {} ns", duration);
        }
    };

    // Brute-force 6-digit number generator
    class NumberBruteForce {
        std::atomic<size_t> count{0};
        std::atomic<bool> found{false};

    public:
        void main() {
            fmt::println("Benchmark:{}{}", Styler_fmt::ColorFG(" 6-Digit Brute Force", 0x00ffff), "");

            auto start = std::chrono::high_resolution_clock::now();

            auto worker = [this](int tid, int total_threads) {
                for (int i = tid; !found.load(); i += total_threads) {
                    char buffer[7];
                    std::snprintf(buffer, sizeof(buffer), "%06d", i);
                    if (std::string(buffer) == "999999") {
                        found.store(true);
                        count.fetch_add(1);
                        break;
                    }
                    count.fetch_add(1);
                }
            };

            constexpr int THREAD_COUNT = 4;
            std::vector<std::thread> threads;
            // for (int i = 0; i < THREAD_COUNT; ++i){
            //     //! WARNING: an enclosing-function local variable cannot be referenced in a lambda body unless it is in the capture listC/C++(1735) 
            //     threads.emplace_back([this, THREAD_COUNT, i](){ worker(i, THREAD_COUNT); });
            // };
            for (int i = 0; i < THREAD_COUNT; ++i) threads.emplace_back([=](){ worker(i, THREAD_COUNT); });

            for (auto& t : threads) t.join();

            auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();

            fmt::println(" > Brute force complete at iteration: {}", count.load());
            fmt::println(" > Execution time: {} ns", duration);
        }
    };

    // Full Benchmark Executor
    static void main() {
        fmt::println("{}{}", Styler_fmt::styler_all("Starting Benchmark...", 0xffff00, 0x000000, true, false, false, false, 100), "");

        BigInteger bigInt;
        XtremeFloat extFloat;
        Strings strings;
        StackBench stack;
        HeapBench heap;
        Smarts smart;
        MemFragTest frag;
        ThreadMutex thread;
        NumberBruteForce brute;

        fmt::println("");
        bigInt.main();
        fmt::println("");
        extFloat.main();
        fmt::println("");
        strings.main();
        fmt::println("");
        stack.main();
        fmt::println("");
        heap.main();
        fmt::println("");
        smart.main();
        fmt::println("");
        frag.main();
        fmt::println("");
        thread.main();
        fmt::println("");
        brute.main();

        fmt::println("{}{}", Styler_fmt::ColorFG("Benchmark Complete!", 0x00ff00), "");
    }
};

template <typename T, typename E>
struct fmt::formatter<std::expected<T, E>> : fmt::formatter<T> {
    template <typename FormatContext>
    auto format(const std::expected<T, E>& e, FormatContext& ctx) {
        if (e.has_value()) {
            return fmt::formatter<T>::format(e.value(), ctx);
        } else {
            return fmt::format_to(ctx.out(), "Error({})", e.error());
        }
    }
};

int main() {
    fmt::println("{:-^{}}", fmt::format(" BENCH {} ", COMPILER), TerminalSize());
    
    auto pid_result = GetPID();
    if (pid_result.has_value()) {
        fmt::println("Process ID : {}", pid_result.value());
    } else {
        fmt::println("Failed to get PID. Error code: {}", pid_result.error());
    }
    // fmt::println("Process ID : {}", GetPID());
    
    #ifdef __clang__
        fmt::println("{}", Styler_fmt::ColorFG("Compiler: LLVM Clang", 0x00ccff));
    #elif _MSC_VER
        fmt::println("{}", Styler_fmt::ColorFG("Compiler: Microsoft MSVC", 0xffd700));
    #else
        fmt::println("{}", Styler_fmt::ColorFG("Unknown Compiler Detected", 0xff4500));
        fmt::println("{}", Styler_fmt::ColorFG("This is a benchmark for LLVM Clang and MSVC.", 0xffa07a));
    #endif
    
    Program::main();
    
    // fmt::println("{}", Styler_fmt::ColorFG("Press Enter to Exit...", 0x00ffff));
    fmt::println("{}", Styler_fmt::ColorFG("Exiting now..."));
    // std::cin.get();
    return 0;

    fmt::println("{:-^{}}", fmt::format(" BENCH {} ", COMPILER), TerminalSize());
}
