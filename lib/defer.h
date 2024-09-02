#ifndef DEFER_H
#define DEFER_H

#include <Arduino.h>
#include <functional>

class Defer {
public:
    Defer(std::function<void()> f) : f_(f) {}
    ~Defer() { f_(); }
private:
    std::function<void()> f_;
};

#endif // !DEFER_H
