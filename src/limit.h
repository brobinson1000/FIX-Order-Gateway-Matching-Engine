#pragma once

#include "enums.h"
#include "order.h"

struct Limit {
    Price price;
    uint32_t total_volume{};

    Order* head = nullptr;
    Order* tail = nullptr;

    void append(Order* order) {
        if (!head) {
            head = order;
            tail = order;
        } else {
            tail->next = order;
            order->prev = tail;
            tail = order;
        }
        total_volume += order->qty;
    }

    void remove(Order* order) {
        if (order->prev) order->prev->next = order->next;
        if (order->next) order->next->prev = order->prev;

        if (order == head) head = order->next;
        if (order == tail) tail = order->prev;

        total_volume -= order->qty;
        order->prev = nullptr;
        order->next = nullptr;
    }

    bool empty() const { return head == nullptr; }
};
