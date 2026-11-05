#pragma once

#include <string_view>
#include <memory>

struct FIXOrder {
    std::string_view Version;
    uint32_t BodyLength;
    char MsgType;
    uint64_t MsgSeq;
    std::string_view SenderCompId;
    std::string_view TargetCompId;
    std::string_view SendingTime;
    std::string_view ClOrdId;
    std::string_view Symbol;
    std::string_view TransactTime;
    Quantity OrderQty;
    char OrdType;
    uint64_t Price;
    char Side;

};

void FIXDecode(std::string_view fixBits, std::unique_ptr<FIXOrder> fp);


