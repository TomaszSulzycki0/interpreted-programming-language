#include <emscripten/bind.h>
#include <string>
#include <iostream>

#include "interpreter.hpp"

class MarkedBuf : public std::streambuf 
{
private:
    std::string& sink_;
    bool isError_;
protected:
    std::streamsize xsputn(const char* s, std::streamsize n) override {
        if (isError_) sink_ += '\x01';
        sink_.append(s, static_cast<size_t>(n));
        if (isError_) sink_ += '\x02';
        return n;
    }

    int_type overflow(int_type ch) override {
        if (!traits_type::eq_int_type(ch, traits_type::eof())) {
            char c = traits_type::to_char_type(ch);
            xsputn(&c, 1);
        }
        return traits_type::not_eof(ch);
    }
public:
    MarkedBuf(std::string& sink, bool isError) : sink_(sink), isError_(isError) {}

};

std::string run_code(const std::string& source) {
    std::string result;
    MarkedBuf outBuf(result, false);
    MarkedBuf errBuf(result, true);

    std::streambuf* oldOut = std::cout.rdbuf(&outBuf);
    std::streambuf* oldErr = std::cerr.rdbuf(&errBuf);

    try {
        Interpreter interpreter {};
        interpreter.run(source);
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
    } catch (...) {
        std::cerr << "Error: unknown exception\n";
    }

    std::cout.rdbuf(oldOut);
    std::cerr.rdbuf(oldErr);
    return result;
}

EMSCRIPTEN_BINDINGS(interp) {
    emscripten::function("runCode", &run_code);
}