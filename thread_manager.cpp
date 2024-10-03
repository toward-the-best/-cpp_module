#include <iostream>
#include <thread>
#include <functional>
#include <condition_variable>
#include <map>
#include <string>
#include <mutex>
#include <chrono>


// ThreadController 클래스 정의
class ThreadController {
public:
    // 생성자: lambda 함수를 받아 초기화하고 스레드를 시작합니다.
    ThreadController(std::function<void()> func)
        : func_(func), running_(false), paused_(false), terminate_(false)
    {
        // jthread는 자동으로 스레드를 관리하며, 소멸 시 자동으로 조인(join)됩니다.
        // 람다를 사용하여 this 포인터를 캡처하고 threadFunc를 호출합니다.
        worker_ = std::jthread([this](std::stop_token st) { this->threadFunc(st); });
    }

    // 소멸자: 스레드를 종료시킵니다.
    ~ThreadController() {
        {
            std::lock_guard<std::mutex> lock(mutex_);
            terminate_ = true;
            cv_.notify_all();
        }
        // jthread는 소멸 시 자동으로 스레드를 종료합니다.
    }

    // 스레드 시작 함수
    void start() {
        std::lock_guard<std::mutex> lock(mutex_);
        if (!running_ && !terminate_) {
            running_ = true;
            paused_ = false;
            cv_.notify_all();
        }
    }

    // 스레드 중지 함수
    void stop() {
        std::lock_guard<std::mutex> lock(mutex_);
        running_ = false;
        paused_ = false;
        cv_.notify_all();
    }

    // 스레드 일시 중지 함수
    void pause() {
        std::lock_guard<std::mutex> lock(mutex_);
        if (running_ && !paused_) {
            paused_ = true;
        }
    }

    // 스레드 재개 함수
    void resume() {
        std::lock_guard<std::mutex> lock(mutex_);
        if (running_ && paused_) {
            paused_ = false;
            cv_.notify_all();
        }
    }

private:
    // 스레드에서 실행될 함수
    void threadFunc(std::stop_token stopToken) {
        while (true) {
            std::unique_lock<std::mutex> lock(mutex_);
            // cv.wait 동작 방식
            // 조건 변수 대기:
            //   wait 함수가 호출되면, 현재 스레드는 대기 상태로 전환됩니다.
            //   이때, 뮤텍스는 자동으로 해제되어 다른 스레드가 공유 자원에 접근할 수 있게 됩니다.
            // 알림 수신:
            //   다른 스레드가 notify_one() 또는 notify_all()을 호출하여 조건 변수를 알립니다.
            //   스레드는 깨워지지만, 즉시 실행되지 않고 다시 뮤텍스를 잠그고 프레디케이트평가 합니다.
            // 프레디케이트 평가:
            //   wait 함수는 프레디케이트를 호출하여 조건을 평가합니다.
            //   프레디케이트가 true를 반환하면, wait 함수는 반환되고 스레드는 실행을 계속합니다.(뮤텍스는 잠긴 상태)
            //   프레디케이트가 false를 반환하면, 스레드는 다시 대기 상태로 전환되며, 뮤텍스는 해제됩니다. (계속 대시)
            // 조건 변수 대기: running_ 이고 paused_ 가 아니며, 종료 요청이 없을 때 실행
            cv_.wait(lock, [this, &stopToken]() {
                return (running_ && !paused_) || terminate_ || stopToken.stop_requested();
            });

            // 종료 신호가 오면 루프를 빠져나감
            if (stopToken.stop_requested() || terminate_) {
                break;
            }

            // mutext_가 잠겨있으므로 해제해주어야 한다.
            lock.unlock();

            // Lambda 함수 실행
            func_();

            // CPU 과부하를 방지하기 위해 잠시 대기
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        }
    }

    std::function<void()> func_;           // 실행할 lambda 함수
    std::jthread worker_;                  // jthread 객체
    std::mutex mutex_;                     // 뮤텍스
    std::condition_variable cv_;           // 조건 변수
    bool running_;                         // 스레드 실행 상태
    bool paused_;                          // 스레드 일시 중지 상태
    bool terminate_;                       // 종료 신호
};

int main() {
    // 스레드를 관리할 map 생성 (값을 unique_ptr으로 변경)
    std::map<std::string, std::unique_ptr<ThreadController>> threads;

    // 예제용 lambda 함수 정의
    auto lambda = [](){
        std::cout << "Thread is running..." << std::endl;
    };

    // unique_ptr를 사용하여 ThreadController 객체를 동적으로 할당하고 map에 삽입
    threads.emplace("thread1", std::make_unique<ThreadController>(lambda));

    // 삽입된 스레드 시작
    std::cout << "Starting thread1..." << std::endl;
    threads["thread1"]->start();

    // 스레드가 실행되는 동안 대기 (예: 2초)
    std::this_thread::sleep_for(std::chrono::seconds(2));

    // 스레드 일시 중지
    std::cout << "Pausing thread1..." << std::endl;
    threads["thread1"]->pause();

    // 일시 중지 상태에서 대기 (예: 2초)
    std::this_thread::sleep_for(std::chrono::seconds(2));

    // 스레드 재개
    std::cout << "Resuming thread1..." << std::endl;
    threads["thread1"]->resume();

    // 스레드가 다시 실행되는 동안 대기 (예: 2초)
    std::this_thread::sleep_for(std::chrono::seconds(2));

    // 스레드 중지
    std::cout << "Stopping thread1..." << std::endl;
    threads["thread1"]->stop();

    // 추가적인 작업을 위해 잠시 대기 (예: 1초)
    std::this_thread::sleep_for(std::chrono::seconds(1));

    // 프로그램 종료 시, unique_ptr가 자동으로 ThreadController를 소멸시킵니다.
    std::cout << "Program terminating..." << std::endl;
    return 0;
}
