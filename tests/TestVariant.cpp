#include <gtest/gtest.h>
#include <variant>

struct State1 {};
struct State2 {};

using AppState = std::variant<State1, State2>;

struct MockVisitor {
  int &value;
  void operator()(State1) const { value = 1; }
  void operator()(State2) const { value = 2; }
};

TEST(VariantTest, StateTransition) {
  AppState state = State1{};

  EXPECT_TRUE(std::holds_alternative<State1>(state));

  int val = 0;
  std::visit(MockVisitor{val}, state);
  EXPECT_EQ(val, 1);

  state = State2{};
  EXPECT_TRUE(std::holds_alternative<State2>(state));

  std::visit(MockVisitor{val}, state);
  EXPECT_EQ(val, 2);
}
