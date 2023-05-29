#include <chrono>
#include <future>
#include <iostream>
#include <map>
#include <thread>

typedef struct ThreadInfo {
    std::string t_name;
    std::future<bool> future;
    bool stop_flag;
} ThreadInfo;

class ThreadMgt {
  private:
    std::map<std::string, ThreadInfo> m_thread_map;

  public:
    void registerThread(std::string t_name, std::future<bool> &future);
    bool &getStopFlag(std::string t_name);
    void Join(std::string t_name, int wait_time = 0);
};

bool &ThreadMgt::getStopFlag(std::string t_name) {
    ThreadInfo info;
    m_thread_map[t_name] = std::move(info);
    m_thread_map[t_name].stop_flag = false;
    return (m_thread_map[t_name].stop_flag);
}

void ThreadMgt::registerThread(std::string t_name, std::future<bool> &future) {
    try {
        m_thread_map[t_name].t_name = t_name;
        m_thread_map[t_name].future = std::move(future);
    } catch (const std::exception &e) {
        std::cerr << e.what() << '\n';
    }
}

void ThreadMgt::Join(std::string t_name, int wait_time) {
    m_thread_map.at(t_name).stop_flag = true;
    if (m_thread_map.at(t_name).future.valid()) {
        std::future_status status = m_thread_map.at(t_name).future.wait_for(
            std::chrono::seconds(wait_time));

        if (status == std::future_status::ready) {
            std::cout << "Task completed. Result: "
                      << m_thread_map.at(t_name).future.get() << std::endl;
        } else {
            std::cout << "Task did not complete within the given time."
                      << std::endl;
        }
    } else {
        std::cout << t_name << " thread is not managed" << std::endl;
    }
}

bool longRunningTask(bool &stop_flag) {
    while (not stop_flag) {
        std::cout << "stop flag : " << stop_flag << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    std::cout << "End : stop flag " << stop_flag << std::endl;
    return true;
}

int main(int argc, char const *argv[]) {
    ThreadMgt tmgt;
    bool &stop_flag = tmgt.getStopFlag("test");
    std::cout << stop_flag << std::endl;
    std::future<bool> future =
        std::async(std::launch::async, longRunningTask, std::ref(stop_flag));

    tmgt.registerThread("test", future);
    std::this_thread::sleep_for(std::chrono::seconds(5));
    tmgt.Join("test", 5);
    return 0;
}
