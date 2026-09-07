#include <iostream>
#include <cstdint>
#include <cstddef>
#include <vector>
#include <string>
#include <string_view>



namespace FIX {


    struct order {
        order.
    };







    class FIXParser {

        FIXParser() {}

        
        void ParseFixOrderMsg(const std::vector<std::byte>& p) {
            const auto* ptr = p.data();
            const auto* end = ptr + p.size();

            const auto SOH = std::byte{0x01};

            while(ptr < end) {

                const auto* nextSOH = ptr;

                while (nextSOH < end && *nextSOH != SOH) {
                    nextSOH++;
                }

                if (ptr < nextSOH) {

                    std::string_view field(
                        reinterpret_cast<const char*>(ptr),
                        static_cast<size_type>(nextSOH - ptr);
                    );


                    std::size_t equalPos = field.find('=');

                    if (equalPos != std::string_view::npos) {
                        std::string_view tagStr = field.substr(0, equalPos);
                        std::string_view valueStr = field.substr(equalPos + 1);

                        int tagNum{};

                        auto [p_err, ec] = std::from_chars(tagStr.data(), tagStr.data() + tagStr.size(), tagNum);

                        if (ec == std::errc{}) {
                            switch (tagNum) {
                                case 8:
                                    return ( valueStr == stableFIXVer ) ? 1 : 0;
                                    break;
                                case 35:
                                    order.msgType = valueStr;
                                    break;
                                case 34:
                                    order.seqNum = valueStr[0];
                                    break;
                                case 52:
                                    order.timeStamp = valueStr;
                                    break;
                                case 11:
                                    order.clientOrderID = valueStr;
                                    break;
                                case 38:
                                    order.qty = valueStr;
                                    break;
                                case 40: 
                                    order.type = valueStr[0];
                                    break;
                                case 54:
                                    order.side = valueStr[0];
                                    break;
                                case 55:
                                    order.symbol = valueStr;
                                    break;
                                default:
                                    break;



                            }
                        }
                    }
                }

                ptr = nextSOH + 1;





            }






        }

        private:
            using size_type = std::string_view::size_type;
            




            
    }







}
