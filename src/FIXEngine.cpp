#include <iostream>
#include <charconv>
#include <memory>
#include "../include/Usings.h"

// 8=FIX.4.40x019=1220x0135=D0x0134=1050x01...10=2340x01
// 8=FIX.4.4|9=145|35=D|49=CLIENT_A|56=BINANCE|34=12|52=20261101-19:45:00|11=ORD12345|55=BTCUSDT|54=1|38=2|40=2|44=96500.00|10=182|


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

void FIXDecode(std::string_view fixBits, std::unique_ptr<FIXOrder> fp) {
    const char* ptr = fixBits.data();
    const char* end = ptr + fixBits.size();

    while(ptr < end) {
        // find tag number
        const char* eqptr = ptr;
        while(eqptr < end && *eqptr != '=') {
            eqptr++;
        }
        if (eqptr == end) break;

        int tag{}; // id for FIX fields
        std::from_chars(ptr, eqptr, tag);

        ptr = eqptr + 1; // move ptr past '='
        

        // find the SOH delimeter
        const char* SOHptr = ptr;
        while(SOHptr < end && *SOHptr != 0x01) {
            SOHptr++;
        }
        
        std::string_view value(ptr, SOHptr - ptr);


        // extract each FIX field
        switch(tag) {
            case 8: fp->Version = value; std::cout << value << "\n"; break;
            case 9: std::from_chars(value.data(), value.data() + value.size(), fp->BodyLength); std::cout << "Success\n"; break;
            case 35: fp->MsgType = value[0]; std::cout << value[0] << "\n"; break;
            case 34: std::from_chars(value.data(), value.data() + value.size(), fp->MsgSeq); std::cout << "Success\n"; break;
            case 49: fp->SenderCompId = value; std::cout << value << "\n"; break;
            case 56: fp->TargetCompId = value; std::cout << value << "\n"; break;
            case 52: fp->SendingTime = value; std::cout << value << "\n"; break;
            case 11: fp->ClOrdId = value; std::cout << value << "\n"; break;
            case 55: fp->Symbol = value; std::cout << value << "\n"; break;
            case 54: fp->Side = value[0]; std::cout << value[0] << "\n"; break;
            case 60: fp->TransactTime = value; std::cout << value << "\n"; break; // crashes here
            case 38: std::from_chars(value.data(), value.data() + value.size(), fp->OrderQty); std::cout << "Success\n";break;
            case 40: fp->OrdType = value[0]; std::cout << value[0] << "\n"; break;
            case 44: std::from_chars(value.data(), value.data() + value.size(), fp->Price);std::cout << "Success\n"; break;
            case 59: fp->Side = value[0]; std::cout <<  value[0] << "\n"; break;
            default: std::cerr << "Error parsing FIX msg\n"; break;
        }
        ptr = SOHptr + 1;
    }
}



int main() {
    auto fo = std::make_unique<FIXOrder>();
    auto standardBtcBuyOrder = "8=FIX.4.4\001"
                           "9=178\001"
                           "35=D\001"
                           "34=101\001"
                           "49=MY_ALGO_DESK\001"
                           "56=BINANCE_GATEWAY\001"
                           "52=20261101-20:45:00.105\001" // High-precision session timestamp
                           "11=ORD_BTC_20261101_01\001"
                           "55=BTCUSDT\001"
                           "54=1\001"                     // 1 = Buy
                           "60=20261101-20:45:00.100\001" // Transaction creation time
                           "38=0.250000\001"              // Buying 0.25 BTC
                           "40=2\001"                     // 2 = Limit Order
                           "44=96500.00\001"              // Price limit in USDT
                           "59=1\001"                     // 1 = Good 'Till Cancelled (GTC)
                           "10=114\001";
    FIXDecode(standardBtcBuyOrder, std::move(fo));
    std::cout << "Hello";

}
