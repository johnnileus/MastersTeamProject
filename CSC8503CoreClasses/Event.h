#pragma once
#include <iostream>
#include <vector>
#include <functional>

class EventBase {
public:
    virtual ~EventBase() = default;
};

template <typename T>
class Event : public EventBase {
public:
    using EventHandler = std::function<void(T)>;

    void AddListener(EventHandler handler) {
        handlers.push_back(handler);
    }

    void Invoke(T data) {
        for (auto& handler : handlers)
            handler(data);
    }

private:
    std::vector<EventHandler> handlers;
};