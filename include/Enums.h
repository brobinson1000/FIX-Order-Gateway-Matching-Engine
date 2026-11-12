#pragma once

enum class TimeInForce {
    GTC, IOC, FOK, Day,
};

enum class Type {
    Market,
    Limit,
    Stop,
    StopLimit,
    Unknown,
};

enum class Side {
    Buy,
    Sell,
};

enum class Status {
    Filled,
    PartiallyFilled,
    New,
    Cancelled,
    Rejected,
};
