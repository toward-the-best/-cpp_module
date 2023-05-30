#include <chrono>
#include <future>
#include <iostream>
#include <map>
#include <thread>
#include <optional>

typedef struct ThreadInfo {
    std::string t_name;
    std::future<bool> future;
    bool status;
    bool stop_flag;
    std::string result;
    std::string error;
} ThreadInfo;

class ThreadMgt {
    private:
        std::map<std::string, ThreadInfo> m_thread_map;

    public:
        std::optional<std::reference_wrapper<bool>> getStopFlag(const std::string t_name);
        bool registerThread(const std::string t_name, std::future<bool> &future);
        bool Join(const std::string t_name, const int wait_time = 0);
};

 

std::optional<std::reference_wrapper<bool>> ThreadMgt::getStopFlag(const std::string t_name) {
    try {
        ThreadInfo info;
        m_thread_map[t_name] = std::move(info);
        m_thread_map[t_name].stop_flag = false;
        m_thread_map[t_name].status = "Not Running";
        return m_thread_map[t_name].stop_flag;
    } catch (std::exception &e) {
        return std::nullopt;
    }
}

bool ThreadMgt::registerThread(const std::string t_name, std::future<bool> &future) {
    try {
        m_thread_map[t_name].t_name = t_name;
        m_thread_map[t_name].future = std::move(future);
        m_thread_map[t_name].status = "Running";
    } catch (const std::exception &e) {
        m_thread_map[t_name].error = e.what();
    }

    if (m_thread_map[t_name].error.empty()) {
        return false;
    }

    m_thread_map.at(t_name).error.clear();
    return true;
}

bool ThreadMgt::Join(const std::string t_name, const int wait_time) {
    try {

        if (m_thread_map.at(t_name).future.valid()) {
            
            if (0 == wait_time) {
                m_thread_map.at(t_name).future.wait();
                m_thread_map[t_name].status = "Done";
                m_thread_map[t_name].result =
                    m_thread_map.at(t_name).future.get();
            } else {
                m_thread_map.at(t_name).stop_flag = true;
                std::future_status status = m_thread_map.at(t_name).future.wait_for(
                        std::chrono::seconds(wait_time));
                if (status == std::future_status::ready) {
                    m_thread_map[t_name].status = "Done";
                    m_thread_map[t_name].result =
                        m_thread_map.at(t_name).future.get();
                } else {
                    m_thread_map[t_name].status = "Error";
                    m_thread_map.at(t_name).error =
                        "Task did not complete within the given time";
                    return false;
                }
            }
        } else {
            m_thread_map[t_name].status = "Error";
            m_thread_map.at(t_name).error = "Task is not managed";
            return false;
        }
    } catch (const std::exception &e) {
        m_thread_map[t_name].status = "Error";
        m_thread_map.at(t_name).error = e.what();
    }

    if (not m_thread_map.at(t_name).error.empty()) {
        return false;
    }

    m_thread_map.at(t_name).error.clear();
    return true;
}

bool longRunningTask(bool &stop_flag) {
    int cnt = 0, max = 10;
    while (not stop_flag) {
        if (cnt++ == max) {
            break;
        }
        std::cout << "stop flag : " << stop_flag << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    std::cout << "End : stop flag " << stop_flag << std::endl;
    return true;
}

int main(int argc, char const *argv[]) {
    ThreadMgt tmgt;
    auto opt = tmgt.getStopFlag("test");
    if (not opt.has_value()) {
        return false;
    }
    bool &stop_flag = opt->get();
    std::cout << stop_flag << std::endl;
    std::future<bool> future =
        std::async(std::launch::async, longRunningTask, std::ref(stop_flag));

    tmgt.registerThread("test", future);
    std::this_thread::sleep_for(std::chrono::seconds(5));
    tmgt.Join("test", 5);
    return 0;
}
