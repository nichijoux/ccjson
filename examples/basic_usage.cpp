#pragma once

#include "ccjson.h"
#include <iostream>

namespace ccjson {

// 示例数据结构
struct Person {
    std::string                        name;
    int                                age{};
    bool                               is_student{};
    std::vector<std::string>           hobbies;
    std::vector<int>                   scores;
    std::map<std::string, std::string> contacts;
};

// 打印 Person 对象的辅助函数
void print_person(const Person& person) {
    std::cout << "Name: " << person.name << std::endl;
    std::cout << "Age: " << person.age << std::endl;
    std::cout << "Is Student: " << (person.is_student ? "Yes" : "No") << std::endl;

    std::cout << "Hobbies: ";
    for (const auto& hobby : person.hobbies) {
        std::cout << hobby << " ";
    }
    std::cout << std::endl;

    std::cout << "Scores: ";
    for (const auto& score : person.scores) {
        std::cout << score << " ";
    }
    std::cout << std::endl;

    std::cout << "Contacts: " << std::endl;
    for (const auto& [platform, id] : person.contacts) {
        std::cout << "  " << platform << ": " << id << std::endl;
    }
}

// 示例：为 Person 结构体实现 from_json
void fromJson(const JsonValue& j, Person& p) {
    p.name       = j["name"].get<std::string>();
    p.age        = j["age"].get<int>();
    p.is_student = j["is_student"].get<bool>();
    p.hobbies    = j["hobbies"].get<std::vector<std::string>>();
    p.scores     = j["scores"].get<std::vector<int>>();
    p.contacts   = j["contacts"].get<std::map<std::string, std::string>>();
}

JsonValue toJson(const Person& p) {
    JsonValue j     = JsonValue::Object();
    j["name"]       = p.name;
    j["age"]        = p.age;
    j["is_student"] = p.is_student;
    j["hobbies"]    = p.hobbies;
    j["scores"]     = p.scores;
    j["contacts"]   = p.contacts;
    return j;
}

// 运行所有示例
void run_examples() {
    std::cout << "\n=== 运行 JsonValue 使用示例 ===\n" << std::endl;

    // 数组索引访问示例
    {
        std::cout << "\n1. 数组索引访问示例:" << std::endl;
        JsonValue array = {1, 2, 3, 4, 5};

        std::cout << "Array[0] = " << static_cast<int>(array[0]) << std::endl;
        std::cout << "Array[4] = " << static_cast<int>(array[4]) << std::endl;

        // 动态增长数组
        array[5]  = 6;   // 自动扩展数组
        array[10] = 11;  // 中间的元素会被自动填充为null
        std::cout << "Array[5] = " << static_cast<int>(array[5]) << std::endl;
        std::cout << "Array[10] = " << static_cast<int>(array[10]) << std::endl;
        std::cout << "Array[7] is null: " << (array[7].isNull() ? "yes" : "no") << std::endl;

        // 嵌套数组访问
        JsonValue nested = JsonValue::Array();
        nested.push_back(JsonValue::Array());
        nested[0] = {1, 2, 3};
        std::cout << "\n2. 嵌套数组访问:" << std::endl;
        std::cout << "nested[0][1] = " << static_cast<int>(nested[0][1]) << std::endl;

        // 混合访问
        JsonValue mixed = JsonValue::Object();
        mixed["Array"]  = {1, true, "dsa"};
        std::cout << "\n3. 混合访问:" << std::endl;
        std::cout << "mixed[\"Array\"][1] = " << mixed["Array"][2].get<std::string>() << std::endl;

        // 错误处理示例
        try {
            std::cout << "\n4. 错误处理示例:" << std::endl;
            int value = array[100];  // 索引越界
            std::cout << "This line should not be reached" << std::endl;
        } catch (const std::runtime_error& e) {
            std::cout << "Expected error: " << e.what() << std::endl;
        }
    }

    // 1. 基本类型示例
    {
        std::cout << "1. 基本类型示例:" << std::endl;
        JsonValue basic  = JsonValue::Object();
        basic["string"]  = "Hello, World!";
        basic["number"]  = 42;
        basic["boolean"] = true;
        basic["null"]    = nullptr;

        std::string str  = basic["string"];
        int         num  = basic["number"];
        bool        flag = basic["boolean"];

        std::cout << "string: " << str << std::endl;
        std::cout << "number: " << num << std::endl;
        std::cout << "boolean: " << (flag ? "true" : "false") << std::endl;
        std::cout << "null is null: " << (basic["null"].isNull() ? "yes" : "no") << std::endl;
    }
    // 2. 数组示例
    {
        std::cout << "\n2. 数组示例:" << std::endl;
        JsonValue arr = JsonValue::Array();
        arr.push_back(1);
        arr.push_back("text");
        arr.push_back(true);

        // 使用初始化列表
        JsonValue numbers = {1, 2, 3, 4, 5};
        JsonValue strings = {"a", "b", "c"};

        auto vec_int = numbers.get<std::vector<int>>();
        auto vec_str = strings.get<std::vector<std::string>>();

        std::cout << "Numbers: ";
        for (int n : vec_int)
            std::cout << n << " ";
        std::cout << std::endl;

        std::cout << "Strings: ";
        for (const auto& s : vec_str)
            std::cout << s << " ";
        std::cout << std::endl;
    }

    // 3. 完整的 Person 示例
    {
        std::cout << "\n3. Person 对象示例:" << std::endl;
        JsonValue json     = JsonValue::Object();
        json["name"]       = "Alice";
        json["age"]        = 20;
        json["is_student"] = true;
        json["hobbies"]    = {"reading", "gaming", "coding"};
        json["scores"]     = {95, 88, 92, 90};

        json["contacts"]            = JsonValue::Object();
        json["contacts"]["email"]   = "alice@example.com";
        json["contacts"]["phone"]   = "123-456-7890";
        json["contacts"]["twitter"] = "@alice";

        Person person;
        person.name       = json["name"].get<std::string>();
        person.age        = json["age"];
        person.is_student = json["is_student"];
        person.hobbies    = json["hobbies"].get<std::vector<std::string>>();
        person.scores     = json["scores"].get<std::vector<int>>();
        person.contacts   = json["contacts"].get<std::map<std::string, std::string>>();

        print_person(person);

        // 4. 错误处理示例
        std::cout << "\n4. 错误处理示例:" << std::endl;
        try {
            int value = json["non_existent_key"];
            std::cout << "This line should not be reached" << std::endl;
        } catch (const std::runtime_error& e) {
            std::cout << "Expected error: " << e.what() << std::endl;
        }

        try {
            std::string text = json["age"];  // 尝试将数字转换为字符串
            std::cout << "This line should not be reached" << std::endl;
        } catch (const std::runtime_error& e) {
            std::cout << "Expected error: " << e.what() << std::endl;
        }
    }

    // 自定义类型反序列化示例
    {
        std::cout << "\n5. 自定义类型反序列化示例:" << std::endl;
        JsonValue person_json            = JsonValue::Object();
        person_json["name"]              = "Alice";
        person_json["age"]               = 25;
        person_json["is_student"]        = true;
        person_json["hobbies"]           = {"reading", "gaming", "coding"};
        person_json["scores"]            = {95, 88, 92};
        person_json["contacts"]          = JsonValue::Object();
        person_json["contacts"]["email"] = "alice@example.com";
        person_json["contacts"]["phone"] = "123-456-7890";

        auto person = person_json.get<Person>();
        std::cout << "Name: " << person.name << std::endl;
        std::cout << "Age: " << person.age << std::endl;
        std::cout << "Is student: " << (person.is_student ? "yes" : "no") << std::endl;
        std::cout << "Hobbies: ";
        for (const auto& hobby : person.hobbies) {
            std::cout << hobby << " ";
        }
        std::cout << std::endl;
        std::cout << "Scores: ";
        for (const auto& score : person.scores) {
            std::cout << score << " ";
        }
        std::cout << std::endl;
        std::cout << "Email: " << person.contacts["email"] << std::endl;
        std::cout << "Phone: " << person.contacts["phone"] << std::endl;
    }

    {
        // 自定义类型序列化示例
        std::cout << "\n6. 自定义类型序列化示例:" << std::endl;
        Person alice;
        alice.name              = "Alice";
        alice.age               = 25;
        alice.is_student        = true;
        alice.hobbies           = {"reading", "gaming", "coding"};
        alice.scores            = {95, 88, 92};
        alice.contacts["email"] = "alice@example.com";
        alice.contacts["phone"] = "123-456-7890";

        // 直接从 Person 构造 JsonValue
        JsonValue json_from_person = alice;

        // 使用赋值运算符
        JsonValue another_json;
        another_json = alice;

        // 验证序列化结果
        std::cout << "Serialized person name: " << json_from_person["name"].get<std::string>()
                  << std::endl;
        std::cout << "Serialized person age: " << json_from_person["age"].get<int>() << std::endl;
        std::cout << "Serialized person hobbies: ";
        for (const auto& hobby : json_from_person["hobbies"].get<std::vector<std::string>>()) {
            std::cout << hobby << " ";
        }
        std::cout << std::endl;

        // 完整的序列化和反序列化循环测试
        auto restored_person = json_from_person.get<Person>();
        std::cout << "\n7. 序列化和反序列化循环测试:" << std::endl;
        std::cout << "Original name: " << alice.name
                  << " -> Restored name: " << restored_person.name << std::endl;
        std::cout << "Original age: " << alice.age << " -> Restored age: " << restored_person.age
                  << std::endl;
        std::cout << "Original email: " << alice.contacts["email"]
                  << " -> Restored email: " << restored_person.contacts["email"] << std::endl;
    }

    {
        std::cout << "\n8. 对象初始化列表示例:" << std::endl;
        JsonValue obj = {{"name", "Alice"},
                         {"age", 25},
                         {"scores", {95, 88, 92}},
                         {"info", {{"email", "alice@example.com"}, {"phone", "123-456-7890"}}}};

        std::cout << "Name: " << obj["name"].get<std::string>() << std::endl;
        std::cout << "Age: " << obj["age"].get<int>() << std::endl;
        std::cout << "Email: " << obj["info"]["email"].get<std::string>() << std::endl;

        // 动态添加混合类型对象
        JsonValue dynamic;
        dynamic["data"] = {{"str", "hello"}, {"num", 42}, {"bool", true}};
        std::cout << "\n动态添加的对象:" << std::endl;
        std::cout << "String: " << dynamic["data"]["str"].get<std::string>() << std::endl;
        std::cout << "Number: " << dynamic["data"]["num"].get<int>() << std::endl;
        std::cout << "Boolean: " << dynamic["data"]["bool"].get<bool>() << std::endl;

        JsonValue allArray = 213985441333436;
        std::cout << "\n超长数字:\n" << allArray << std::endl;
    }
    {
        std::string s = R"("Hello\0World")";
        auto        a = JsonParser::parse(s, JsonParser::ENABLE_PARSE_0_ESCAPE_SEQUENCE);
        std::cout << a;
    }
}

}  // namespace ccjson

int main() {
    ccjson::run_examples();
    return 0;
}