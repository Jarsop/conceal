# Conceal

**Conceal** is a modern, header-only C++ library for safely wrapping sensitive values.  
It prevents accidental leaks of secrets (like passwords or tokens) by ensuring that printing or serializing a `Conceal<T>` never reveals the underlying value, while still allowing seamless use of the wrapped type in your code.

---

## Features

- **Type-safe wrapper** for any type (`Conceal<T>`)
- **Prevents accidental leaks**: printing or serializing a `Conceal<T>` never reveals the value
- **Seamless usage**: implicit conversion and operators for easy integration
- **JSON support**: integrates with [nlohmann/json](https://github.com/nlohmann/json) by default
- **Header-only**: just include and use, no build step required

---

## Simple Example

```cpp
#include <conceal/conceal.hpp>
#include <iostream>
#include <cassert>
#include <sstream>

int main() {
    conceal::Conceal<std::string> secret("my_password");

    // Use as a normal string
    assert(secret->size() == 11);

    // Printing never reveals the value
    std::ostringstream oss;
    oss << secret;
    assert(oss.str() == "<hidden>");
}
```

---

## Installation & Integration

### Using CMake (Recommended)

1. **Add to your CMake project:**

   ```cmake
   add_subdirectory(path/to/conceal)
   target_link_libraries(your_target PRIVATE conceal)
   ```

2. **JSON support**
   - [nlohmann/json](https://github.com/nlohmann/json) will be required and automatically linked.
   - To disable JSON support, set the option in your `CMakeLists.txt`:

     ```cmake
     set(CONCEAL_USE_JSON OFF)
     ```

   - Or from the command line:

     ```sh
     cmake .. -DCONCEAL_USE_JSON=OFF
     ```

3. **Custom hidden message**
   - By default, the hidden message is `<hidden>`.
   - To override it, set the option in your CMakeLists.txt

    ```cmake
    set(CONCEAL_HIDDEN_MSG "***SECRET***")
    ```

   -Or from the command line:

    ```cmake
    cmake .. -DCONCEAL_HIDDEN_MSG="***SECRET***"
    ```

### Standalone / Single-header

- Copy `include/conceal/conceal.hpp` into your project.
- By default, JSON support is **not** enabled. To enable JSON support, define the macro `CONCEAL_USE_JSON`  **before** including the header:

  ```cpp
  #define CONCEAL_USE_JSON
  // You can also override the default hidden message
  #define CONCEAL_HIDDEN_MSG "***SECRET***"
  #include <conceal/conceal.hpp>
  ```

---

## Advanced Example (with JSON and Structs)

```cpp
#include <conceal/conceal.hpp>
#include <nlohmann/json.hpp>
#include <iostream>
#include <string>
#include <cassert>
#include <sstream>

struct User {
    int id;
    std::string username;
    conceal::Conceal<std::string> password;

    friend void to_json(nlohmann::json& j, const User& user) {
        j = nlohmann::json{{"id", user.id}, {"username", user.username}, {"password", user.password}};
        // password will always be serialized as null
    }
    friend void from_json(const nlohmann::json& j, User& user) {
        j.at("id").get_to(user.id);
        j.at("username").get_to(user.username);
        if (j.contains("password")) j.at("password").get_to(user.password);
    }
};

int main() {
    User admin{1, "admin", conceal::Conceal<std::string>{"super_secret"}};

    // Print user (password is hidden)
    std::ostringstream oss1;
    oss1 << admin.password;
    assert(oss1.str() == "<hidden>");

    // Serialize to JSON (password is hidden)
    nlohmann::json j = admin;
    assert(j["password"].is_null());

    // Deserialize from JSON
    std::string json_payload = R"({"id":2,"username":"guest","password":"guestpass"})";
    User guest = nlohmann::json::parse(json_payload).get<User>();

    std::ostringstream oss2;
    oss2 << guest.password;
    assert(oss2.str() == "<hidden>");
    assert(*guest.password == "guestpass");
}
```

---

## Build & Test

To build and run tests/examples with CMake:

```sh
git clone https://github.com/Jarsop/conceal.git
cd conceal
cmake -B build -S .
cmake --build build

# Run exemple
./build/examples/conceal_example

# Run tests
ctest --test-dir build
```

**Dependencies:**

- [nlohmann/json](https://github.com/nlohmann/json) (for JSON support, enabled by default)
- [GoogleTest](https://github.com/google/googletest) (optional, for tests)

---

## API Overview

- `Conceal<T>()`
- `Conceal<T>(const T&)`
- `Conceal<T>(T&&)`
- Copy/move constructors and assignment
- Implicit conversion to/from `T`
- Comparison operators
- Stream output (`<<`) always prints `<hidden>`
- JSON serialization always outputs `null` (by default)

---

## FAQ

**Q: Can I use Conceal with any type?**  
A: Yes, as long as the type is copyable/movable. It works with standard and user-defined types.

**Q: Does Conceal provide cryptographic security?**  
A: No. Conceal is for accidental leak prevention (e.g., logs, serialization). It does **not** encrypt or securely erase memory.

**Q: How do I access the real value?**  
A: Use implicit conversion or dereference:  

```cpp
std::string s = secret; // or
std::string s = *secret;
```

---

## License

MIT License

---

## Disclaimer

This library is intended as a convenience for accidental leaks.  
It does **not** provide cryptographic security or memory erasure.
