#include <iostream>
#include <bitset>
#include <charconv>

// 8=FIX.4.40x019=1220x0135=D0x0134=1050x01...10=2340x01
// 8=FIX.4.4|9=145|35=D|49=CLIENT_A|56=BINANCE|34=12|52=20261101-19:45:00|11=ORD12345|55=BTCUSDT|54=1|38=2|40=2|44=96500.00|10=182|


struct FIXOrder {
    std::string_view Version;
    uint32_t BodyLength;
};

void bitshift(std::string_view fixBits, FIXOrder fp) {
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
            case 8: fp.Version = value; std::cout << value; break;
            case 9: std::from_chars(value.data(), value.data() + value.size(), fp.BodyLength); std::cout << "Success"; break;
            case 
        }
        ptr = SOHptr + 1;
    }
}



int main() {
    FIXOrder fo;
    auto rawTestMsg = "8=FIX.4.4\x01"
                      "9=157\x01"
                      "35=D\x01"
                      "34=482\x01"
                      "11=CL_BTC_99821\x01"
                      "55=BTCUSDT\x01"
                      "54=1\x01"
                      "38=25\x01"       // Qty: 25
                      "44=96500.50\x01"  // Price: 96500.50
                      "10=143\x01";
    bitshift(rawTestMsg, fo);
    std::cout << "Hello";

}
