#pragma once
#include "Using.h"
#include "Enums.h"

class Order {
public:

    Order(OrderId orderId, Quantity originalQuantity, Quantity remainingQuantity, Price price, TimeInForce timeInForce, Status status, Type type, Side side) :   
        orderId_(orderId), originalQuantity_(originalQuantity), remainingQuantity_(remainingQuantity), price_(price), timeInForce_(timeInForce), status_(status), type_(type), side_(side) {}

    OrderId GetOrderId() const { return orderId_; }
    Quantity GetOriginalQuantity() const { return originalQuantity_; }
    Quantity GetRemainingQuantity() const { return remainingQuantity_; }
    Quantity GetFilledQuantity() const { return originalQuantity_ - remainingQuantity_; }
    Price GetPrice() const { return price_; }
    TimeInForce GetTimeInForce() const { return timeInForce_; }
    Status GetStatus() const { return status_; }
    Type GetType() const { return type_; }
    Side GetSide() const { return side_; }

private:
    OrderId orderId_;
    Quantity originalQuantity_;
    Quantity remainingQuantity_;
    Price price_;
    TimeInForce timeInForce_;
    Status status_;
    Type type_;
    Side side_;
};

