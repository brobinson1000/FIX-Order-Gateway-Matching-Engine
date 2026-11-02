#pragma once

enum class TimeInForce {
    GTC, IOC, FOK, Day,
};

enum class Status {
    Filled,
    PartiallyFilled,
    New,
    Cancelled,
    Rejected,
};
