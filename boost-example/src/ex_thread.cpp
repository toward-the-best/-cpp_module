#include <boost/thread.hpp>
#include <iostream>
#include <map>

class ThreadManager {
  public:
    typedef struct thread_info {
        std::string thread_name;
        boost::thread thread;
        bool stop_flag;
    } thread_info;
    ThreadManager() {}

    void registerThread(std::string name, boost::thread &thread) {
        thread_info info;
        info.thread_name = name;
        info.thread = std::move(thread);
        info.stop_flag = false;
        m_mThreads[name] = std::move(info);
    }

    void stop(std::string th_name) {
        // m_mThreads[th_name].thread.interrupt();
        m_mThreads[th_name].thread.join();
    }

    void stop_timed(std::string th_name, int timeout) {
        // m_mThreads[th_name].thread.interrupt();
        if (!m_mThreads[th_name].thread.timed_join(
                boost::posix_time::seconds(timeout))) {
            std::cout << "Thread stop timed out, stopping the thread\n";
            m_mThreads[th_name].thread.interrupt();
            m_mThreads[th_name].thread.join();
        } else {
            std::cout << "Operation completed within timeout\n";
        }
    }

  private:
    std::map<std::string, thread_info> m_mThreads;
};

void threadFunction() {
    try {
        for (int i = 0; i < 100; i++) {
            std::cout << "thread is running " << i << std::endl;
            boost::this_thread::sleep(boost::posix_time::seconds(1));
        }

    } catch (const boost::thread_interrupted &) {
        std::cout << "Thread was interrupted" << std::endl;
    }
}

int main() {
    ThreadManager myThread;

    boost::thread m_thread = boost::thread(threadFunction);

    myThread.registerThread("th1", m_thread);

    // Wait for the thread to complete, but only for 5 seconds
    myThread.stop_timed("th1", 5);
    return 0;
}