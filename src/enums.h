#pragma once

enum class Side : char {
    '1' = Buy,
    '2' = Sell,
    '5' = Short_Sell
};

enum class OrderType : char {
    '1' = Market,
    '2' = Limit,
    '3' = Stop,
    'P' = Pegged
};

enum class TimeInForce : char {
    '0' = Day,
    '1' = GTC,
    '3' = IOC,
    '4' = FOK
};

enum class SecurityIDSource : char {
    '4' = ISIN,
    '1' = CUSIP
};
