#include <coroutine> // 코루틴을 사용하기 위한 헤더
#include <iostream>

// 지금은 비어 있지만 앞으로 완성 되어져갈 코루틴 반환 객체
class Task {
  public:
    // 규칙 1. C++에서 정의된 규칙을 구현한 promise_type 이라는 이름의 타입이
    // 정의되어야 한다.
    struct promise_type {
        // 사용자 정의 "코루틴 반환 객체"를 반환 한다
        Task get_return_object() {
            return Task{
                std::coroutine_handle<promise_type>::from_promise(*this)};
        }

        // 코루틴 최초 실행 시 호출. awaitable 객체를 반환 한다.
        auto initial_suspend() { return std::suspend_always{}; }

        // co_return을 사용하는 경우 구현. 나중에 코루틴 종료를 설명 할 때 같이
        // 설명
        auto return_void() { return std::suspend_never{}; }

        // 코루틴 종료 시 호출. 나중에 코루틴 종료를 설명 할 때 같이 설명
        auto final_suspend() noexcept(true) { return std::suspend_always{}; }

        // 코루틴 수행 중 예외 발생 시 호출
        void unhandled_exception() { std::exit(1); }
    };

    // 규칙 2. std::coroutine_handle<promise_type> 타입의 멤버 변수가 있어야
    // 한다.
    std::coroutine_handle<promise_type> co_handler;

    // 규칙 3. std::coroutine_handle<promise_type> 을 인자로 받아
    // 멤버 변수를 초기화 하는 생성자가 있어야 한다.
    Task(std::coroutine_handle<promise_type> handler) : co_handler(handler) {}

    // 규칙 4. 소멸자에서 std::coroutine_handle<promise_type> 타입의
    // 코루틴 핸들러 멤버 변수의 destroy를 호출 해야 한다.
    ~Task() {
        if (true == (bool)co_handler) {
            co_handler.destroy();
        }
    }
};

// 코루틴 함수
//     규칙 2. co_await를 사용한다
//     규칙 3. 코루틴 반환 객체(Task)를 리턴한다
Task foo() {
    std::cout << "foo 1" << std::endl;
    co_await std::suspend_always{};
    std::cout << "foo 2" << std::endl;
}

int main() {
    // 코루틴 foo()를 실행하면 본문 실행 전에 중단하고 호출자(main)에게 제어권
    // 넘김 최초 중단시 코루틴 반환 객체(Task)를 호출자에게 돌려 준다
    Task task = foo();
    std::cout << "\t main 1" << std::endl;

    // 코루틴 반환 객체의 멤버 coroutine_handle.resume()을 이용해 코루틴 재개
    task.co_handler.resume();
    std::cout << "\t main 2" << std::endl;
    task.co_handler.resume();
}