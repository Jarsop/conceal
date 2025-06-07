#include <iostream>
#include <string>

#include <nlohmann/json.hpp>

#include <conceal/conceal.hpp>

struct User {
  int id;
  std::string username;
  conceal::Conceal<std::string> password;

  friend std::ostream &operator<<(std::ostream &os, const User &user) {
    os << "User {\n"
       << "id: " << user.id << ",\n  username: " << user.username
       << ",\n  password: " << user.password << "\n}";
    return os;
  }

  inline friend void to_json(nlohmann::json &j, const User &user) {
    j = nlohmann::json{{"id", user.id},
                       {"username", user.username},
                       {"password", user.password}};
  }
  inline friend void from_json(const nlohmann::json &j, User &user) {
    j.at("id").get_to(user.id);
    j.at("username").get_to(user.username);
    if (j.contains("password")) {
      j.at("password").get_to(user.password);
    }
  }
};

int main() {
  User user = {1, "admin", conceal::Conceal<std::string>{"topsecret"}};

  std::cout << user << std::endl;

  nlohmann::json j = user;
  std::cout << std::endl<< j.dump(2) << std::endl;

  User from_json = j.get<User>();
  std::cout << std::endl << from_json << std::endl;
  std::cout << "Password (hidden): " << from_json.password << std::endl;
  std::cout << "Password (lost): " << *from_json.password << std::endl;

  std::string json_payload = R"({
    "id": 2,
    "username": "guest",
    "password": "guestpass"
  })";
  nlohmann::json j2 = nlohmann::json::parse(json_payload);
  User from_json2 = j2.get<User>();
  std::cout << std::endl << from_json2 << std::endl;

  std::cout << "Password (hidden): " << from_json2.password << std::endl;
  std::cout << "Password (clear): " << *from_json2.password << std::endl;

  return 0;
}
