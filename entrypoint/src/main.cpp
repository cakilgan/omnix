#include <omnix/omnix.h>
#define OXL_BYTE
#define OXL_TIME
#define OXL_LOGGER
#include <thread>
#include <omnix/platform/literals.h>
ox::logger::log_queue<> *ox::logger::queue;
std::thread log_queue_thread;
void logger_init() {
    ox::logger::queue = new ox::logger::log_queue<>();
    log_queue_thread = std::thread(  []{
        ox::logger::log_event ev{};
        while (ox::logger::queue->pop(ev)) {
            process(ev);
        }
    });
}
void logger_shutdown() {
    while (!ox::logger::queue->empty()) {
        std::this_thread::yield();
    }
    ox::logger::queue->stop();
    if (log_queue_thread.joinable()) log_queue_thread.join();
    delete ox::logger::queue;
}

int main(int argc, char **argv) {
    logger_init();
    logger_shutdown();
    return EXIT_SUCCESS;
}
