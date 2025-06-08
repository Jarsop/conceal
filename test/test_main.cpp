#include <conceal/conceal.hpp>
#include <gtest/gtest.h>
#include <nlohmann/json.hpp>
#include <sstream>
#include <string>

using namespace conceal;

TEST(ConcealTest, DefaultConstruct) {
  Conceal<int> c;
  EXPECT_EQ(c, 0);
  Conceal<std::string> cs;
  EXPECT_EQ(cs, "");
  Conceal<std::vector<int>> cv;
  EXPECT_TRUE(cv->empty());
}

TEST(ConcealTest, ValueConstruct) {
  Conceal<std::string> c("secret");
  EXPECT_EQ(c, "secret");
}

TEST(ConcealTest, CopyConstruct) {
  Conceal<int> a(42);
  Conceal<int> b(a);
  EXPECT_EQ(b, 42);
}

TEST(ConcealTest, MoveConstruct) {
  Conceal<int> a(99);
  Conceal<int> b(std::move(a));
  EXPECT_EQ(b, 99);
}

TEST(ConcealTest, ConstructFromOtherConcealConvertible) {
  Conceal<int> ci(42);
  Conceal<double> cd(ci);
  EXPECT_EQ(cd, 42.0);

  Conceal<int> ci2(123);
  Conceal<long> cl(std::move(ci2));
  EXPECT_EQ(cl, 123L);
  EXPECT_EQ(ci2, 0);
}

TEST(ConcealTest, ConstructFromInitializerList) {
  Conceal<std::vector<int>> cv{1, 2, 3, 4};
  std::vector<int> expected{1, 2, 3, 4};
  EXPECT_EQ(cv, expected);

  Conceal<std::string> cs{'h', 'e', 'l', 'l', 'o'};
  EXPECT_EQ(cs, "hello");
}

TEST(ConcealTest, Assignment) {
  Conceal<int> c;
  c = 123;
  EXPECT_EQ(c, 123);

  Conceal<std::string> s;
  s = std::string("abc");
  EXPECT_EQ(s, "abc");
}

TEST(ConcealTest, CopyAssignment) {
  Conceal<int> a(77);
  Conceal<int> b;
  b = a;
  EXPECT_EQ(b, 77);
}

TEST(ConcealTest, MoveAssignment) {
  Conceal<int> a(88);
  Conceal<int> b;
  b = std::move(a);
  EXPECT_EQ(b, 88);
}

TEST(ConcealTest, Comparisons) {
  Conceal<int> a(1), b(2);
  EXPECT_LT(a, b);
  EXPECT_GT(b, a);
  EXPECT_NE(a, b);
  EXPECT_EQ(a, 1);
  EXPECT_NE(b, 1);
  EXPECT_LE(a, b);
  EXPECT_GE(b, a);
}

TEST(ConcealTest, ConversionOperators) {
  Conceal<int> c(42);
  int x = c;
  EXPECT_EQ(x, 42);

  const Conceal<int> cc(55);
  int y = cc;
  EXPECT_EQ(y, 55);

  int *ptr = c;
  EXPECT_EQ(*ptr, 42);

  const int *cptr = cc;
  EXPECT_EQ(*cptr, 55);
}

TEST(ConcealTest, StreamOutput) {
  Conceal<int> c(1234);
  Conceal<std::vector<int>> cv{1, 2, 3, 4};
  std::ostringstream oss;

  oss << c;
  EXPECT_EQ(oss.str(), "<hidden>");
  oss.str("");
  oss << cv;
  EXPECT_EQ(oss.str(), "<hidden>");
}

TEST(ConcealTest, JsonSerialization) {
  Conceal<int> c(42);
  nlohmann::json j = c;
  EXPECT_TRUE(j.is_null());
}

TEST(ConcealTest, JsonDeserializationNull) {
  nlohmann::json j = nullptr;
  Conceal<int> c(99);
  from_json(j, c);
  EXPECT_EQ(c, 0);
}

TEST(ConcealTest, JsonDeserializationValue) {
  nlohmann::json j = 123;
  Conceal<int> c;
  from_json(j, c);
  EXPECT_EQ(c, 123);
}

struct TestStruct {
  int a;
  Conceal<int> b;

  friend void to_json(nlohmann::json &j, const TestStruct &obj) {
    j = nlohmann::json{{"a", obj.a}, {"b", obj.b}};
  }
  friend void from_json(const nlohmann::json &j, TestStruct &obj) {
    j.at("a").get_to(obj.a);
    if (j.contains("b"))
      j.at("b").get_to(obj.b);
  }
};

TEST(ConcealTest, StructJsonSerialization) {
  TestStruct obj{123, Conceal<int>{999}};
  nlohmann::json j = obj;
  EXPECT_TRUE(j.contains("a"));
  EXPECT_TRUE(j.contains("b"));
  EXPECT_EQ(j["a"], 123);
  EXPECT_TRUE(j["b"].is_null());

  TestStruct restored = j.get<TestStruct>();
  EXPECT_EQ(restored.a, 123);
  EXPECT_EQ(restored.b, 0);
}

TEST(ConcealTest, StructJsonDeserialization) {
  std::string json_payload = R"({"a": 21, "b": 42})";
  nlohmann::json j2 = nlohmann::json::parse(json_payload);
  TestStruct from_json = j2.get<TestStruct>();
  EXPECT_EQ(from_json.a, 21);
  EXPECT_EQ(from_json.b, 42);
}
