# ccjson - 现代 C++ JSON 库

## 概述

`ccjson` 是一个轻量级的 C++ 库，专为解析、操作和序列化 JSON 数据而设计。

`ccjson` 提供简单直观的 API，利用现代 C++ 特性（C++17 及以上）确保类型安全、性能和易用性。

`ccjson` 支持所有标准 JSON 数据类型（空值、布尔值、数字、字符串、数组和对象），并包括高级功能，如自定义类型序列化、异常处理和灵活的解析选项。

## 特性

- **仅头文件**：无需构建或链接外部库，只需包含 `ccjson.h` 和 `ccjson.cc`。
- **类型安全**：使用 `JsonValue` 类表示 JSON 数据，编译时检查类型正确性。
- **现代 C++**：利用 C++17 特性，如 `std::variant`、`std::string_view` 和 SFINAE，编写健壮且表达力强的代码。
- **自定义序列化**：通过 `toJson` 和 `fromJson` 函数支持用户定义类型，自动通过模板元编程检测。
- **灵活解析**：可配置的解析选项，支持非标准转义序列（`\x` 和 `\0`）。
- **异常处理**：提供 `JsonException` 和 `JsonParseException`，包含详细错误信息和解析错误的位置。
- **容器支持**：无缝序列化/反序列化 `std::vector`、`std::map` 和 `std::unordered_map`。
- **美化输出**：支持自定义缩进的 JSON 输出，便于阅读。
- **用户友好 API**：直观的操作符（`[]`、`=`）和方法（`get<T>`、`set`、`push_back`），简化 JSON 操作。
- **字面量支持**：使用 `_json` 用户定义字面量直接解析 JSON 字符串。
- **编译器反射**：基于c++17的静态反射实现，用于注册序列化和反序列化结构体

## 要求

- C++17 或更高版本
- 标准 C++ 库
- 现代 C++ 编译器（如 GCC 7+、Clang 5+、MSVC 2017+）

## 安装

由于 `ccjson` 是仅头文件库，安装非常简单：

1. 下载或克隆仓库。
2. 将 `ccjson.h` 复制到项目的包含目录。
3. 在源文件中包含头文件：

```cpp
#include "ccjson.h"
```

## 使用

### 基本示例

```cpp
#include <iostream>
#include "ccjson.h"

int main() {
    using namespace ccjson;

    // 创建 JSON 对象
    JsonValue json = {
        {"name", "Alice"},
        {"age", 25},
        {"scores", {90, 85, 88}},
        {"address", {
            {"city", "Wonderland"},
            {"zip", "12345"}
        }}
    };

    // 打印 JSON
    std::cout << json.toString(2) << std::endl;

    // 访问和修改值
    json["age"] = 26;
    json["scores"].push_back(95);

    // 解析 JSON 字符串
    auto parsed = R"({"key": "value"})"_json;
    std::cout << parsed.toString() << std::endl;

    // 获取类型化值
    std::string name = json["name"].get<std::string>();
    int age = json["age"].get<int>();
    std::vector<int> scores = json["scores"].get<std::vector<int>>();

    return 0;
}
```

**输出**：

```json
[
  {
    "address": {
      "city": "Wonderland",
      "zip": "12345"
    },
    "age": 26,
    "name": "Alice",
    "scores": [
      90,
      85,
      88,
      95
    ]
  },
  {
    "key": "value"
  }
]
```

### 带选项的解析

```cpp
auto json = ccjson::JsonParser::parse(R"({"text": "Hello\0World"})",
                                      ccjson::JsonParser::ENABLE_PARSE_0_ESCAPE_SEQUENCE);
```

### 自定义类型序列化

```cpp
struct Person {
    std::string name;
    int age;
};

// 定义 toJson 用于序列化
ccjson::JsonValue toJson(const Person& p) {
    return ccjson::JsonValue({
        {"name", p.name},
        {"age", p.age}
    });
}

// 定义 fromJson 用于反序列化
void fromJson(const ccjson::JsonValue& json, Person& p) {
    p.name = json["name"].get<std::string>();
    p.age = json["age"].get<int>();
}

int main() {
    using namespace ccjson;
    Person p{"Bob", 30};
    JsonValue json = toJson(p);
    std::cout << json.toString(2) << std::endl;

    Person p2;
    fromJson(json, p2); // 反序列化
}
```

### 编译期反射

```c++
struct Address {
    std::string country;
    std::string province;

    REFLECT(country, province)
};

struct Student {
    std::string name;
    int         age{0};
    int         sex{0};
    Address     address;

    //    REFLECT(name, age, sex, address)
};

REFLECT_TYPE(Student, name, age, sex, address)

template <class T, class N>
struct Baby {
    T name{};
    N hungry{};
};

REFLECT_TYPE_TEMPLATED(((Baby<T, N>), class T, class N), name, hungry)

std::cout << "\n10.反射注册展示" << std::endl;

{
    std::cout << "\n 普通嵌套类型" << std::endl;
    Student student;
    student.name             = "Ping";
    student.age              = 23;
    student.sex              = 1;
    student.address.province = "chengdu";
    student.address.country  = "china";

    std::string data = ccjson::reflect::serialize(student).toString();
    std::cout << data << std::endl;

    // 反射序列化
    student = ccjson::reflect::deserialize<Student>(data);
    std::cout << ccjson::reflect::serialize(student) << std::endl;
}

{
    std::cout << "\n 模板嵌套类型" << std::endl;
    Class c;
    c.room = 1;
    for (int i = 0; i < 5; ++i) {
        Person alice;
        alice.name              = "name" + std::to_string(i);
        alice.age               = i * 10;
        alice.is_student        = true;
        alice.hobbies           = {"reading", "gaming", "coding"};
        alice.scores            = {95, 88, 92};
        alice.contacts["email"] = "alice@example.com";
        alice.contacts["phone"] = "123-456-7890";
        c.students.emplace_back(alice);
    }

    std::string bin = ccjson::reflect::serialize(c).toString();
    std::cout << bin << std::endl;
    auto o = ccjson::reflect::deserialize<Class>(bin);
    std::cout << ccjson::reflect::serialize(o).toString() << std::endl;
}

{
    std::cout << "\n 体外模板类型" << std::endl;
    Baby<int, float>        baby1{1, 1.2};
    Baby<std::string, bool> baby2{"baby2", false};

    std::string bin1 = ccjson::reflect::serialize(baby1).toString();
    std::cout << bin1 << std::endl;

    baby1 = ccjson::reflect::deserialize<decltype(baby1)>(bin1);
    std::cout << ccjson::reflect::serialize(baby1).toString() << std::endl;

    std::string bin2 = ccjson::reflect::serialize(baby2).toString();
    std::cout << bin2 << std::endl;

    baby2 = ccjson::reflect::deserialize<decltype(baby2)>(bin2);
    std::cout << ccjson::reflect::serialize(baby2).toString() << std::endl;
}
```

## API 亮点

### `JsonValue` 类

- 支持所有 JSON 类型的构造函数（空值、布尔值、数字、字符串、数组、对象）。
- 操作符：`[]` 用于访问元素，`=` 用于赋值。
- 方法：`get<T>`、`set`、`push_back`、`toString`、`type`、`isNull` 等。
- const 迭代器：`begin()` 和 `end()` 用于遍历数组和对象。

### `JsonParser` 类

- `parse`：解析 JSON 字符串，支持自定义选项。
- `stringify`：将 `JsonValue` 序列化为 JSON 字符串，支持可选缩进。

### 异常

- `JsonException`：通用 JSON 错误（如类型不匹配）。
- `JsonParseException`：解析错误，包含位置信息。

## 许可证

`ccjson` 采用 MIT 许可证。详情见 `LICENSE` 文件。