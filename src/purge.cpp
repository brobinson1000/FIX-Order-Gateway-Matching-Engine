#include <thread>
#include <condition_variable>
#include <mutex>
#include <atomic>
#include <memory>

#include "order.h"
#include "orderbook.h"

bool RollOverComplete{};
std::atomic_uint_fast64_t sequenceNumber(0);


void sequenceNumberReset() {
    int prevSequenceNumber = sequenceNumber.exchange(0, std::memory_order_relaxed);
    
}





void sessionRollOverCleanout() {

    std::unique_lock<std::mutex> lock(EODMut);

    cv.wait(lock, []{ return EOD; });

    /* Implement EOD orders cleanup here */
    
    RollOverComplete = true;

    lock.unlock();
    cv.notify_one();
}
