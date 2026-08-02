#pragma once

#include "enums.h"

struct Order {
    OrderID id;
    Price price;
    Quantity qty;
    Side side;
    OrderType ot;
    TimeInForce tif;


    Order* prev = nullptr;
    Order* next = nullptr;
};
