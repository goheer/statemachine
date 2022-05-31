#include <cstring>
#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>

enum State { StateA, StateB, StateC, StateD };

class IHandler {
 public:
  virtual ~IHandler() = default;

  virtual State handle_input(int) = 0;

  virtual State handle_input(const char*) = 0;
};

typedef std::unordered_map<State, std::shared_ptr<IHandler>> StateMachineMap;

class IAction {
 public:
  virtual ~IAction() = default;
  virtual void SomeActionA() = 0;
  virtual void SomeActionB() = 0;
};

class BaseHandler : public IHandler {
 public:
  BaseHandler(State defaultState) : m_defaultState(defaultState) {}

 protected:
  State getDefaultState() const {
    return m_defaultState;
  }

 private:
  State m_defaultState;
};

class StateAHandler : public BaseHandler {
 public:
  explicit StateAHandler(IAction& action)
      : BaseHandler(State::StateA), m_action(action) {}

  virtual State handle_input(int number) override {
    if (number == 100) {
      std::cout << "StateAHandler: Moving to State B" << std::endl;
      return State::StateB;
    }
    return getDefaultState();
  }

  virtual State handle_input(const char*) override {
    std::cout << "StateAHandler: This state cannot accept string input"
              << std::endl;
    return getDefaultState();
  }

 private:
  IAction& m_action;
};

class StateBHandler : public BaseHandler {
 public:
  explicit StateBHandler(IAction& action)
      : BaseHandler(State::StateB), m_action(action) {}

  virtual State handle_input(int number) override {
    std::cout << "StateBHandler: This state cannot accept int input"
              << std::endl;

    return getDefaultState();
  }

  virtual State handle_input(const char* str) override {
    if (std::strcmp(str, "reset") == 0) {
      std::cout << "StateBHandler: Moving to State A" << std::endl;
      return State::StateA;
    }
    return getDefaultState();
  }

 private:
  IAction& m_action;
};

class StateMachineConfigX {
 public:
  static StateMachineMap get(IAction& action) {
    StateMachineMap result;
    result[State::StateA] = std::make_shared<StateAHandler>(action);
    result[State::StateB] = std::make_shared<StateBHandler>(action);
    // result[State::StateC] = std::make_shared<StateCHandler>(action);

    return result;
  }
};

class StateMachine {
 public:
  StateMachine(IAction& action) : m_currentState(State::StateA) {
    m_map = StateMachineConfigX::get(action);
  }

  void handle_input(int number) {
    auto& ptr = m_map[m_currentState];
    m_currentState = ptr->handle_input(number);
  }

  void handle_input(const char* str) {
    auto& ptr = m_map[m_currentState];
    m_currentState = ptr->handle_input(str);
  }

 private:
  State m_currentState;
  StateMachineMap m_map;
};

class Action : public IAction {
 public:
  void SomeActionA() override {
    std::cout << "ActionA" << std::endl;
  }

  void SomeActionB() override {
    std::cout << "ActionB" << std::endl;
  }
};

int main(int argc, char** argv) {
  Action a;
  StateMachine sm(a);

  sm.handle_input("Hello");
  sm.handle_input(100);
  sm.handle_input(123);
  sm.handle_input("reset");
  sm.handle_input("Hello");

  return 0;
}
