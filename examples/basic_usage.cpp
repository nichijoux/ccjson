#pragma once

#include "ccjson.h"
#include "ccjson_reflec.h"
#include <fstream>
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

    REFLECT(name, age, is_student, hobbies, scores, contacts);
};

struct Class {
    int                 room{};
    std::vector<Person> students;
};

REFLECT_TYPE(Class, room, students);

// 打印 Person 对象的辅助函数
void print_person(const Person& person) {
    std::cout << "Name: " << person.name << '\n';
    std::cout << "Age: " << person.age << '\n';
    std::cout << "Is Student: " << (person.is_student ? "Yes" : "No") << '\n';

    std::cout << "Hobbies: ";
    for (const auto& hobby : person.hobbies) {
        std::cout << hobby << " ";
    }
    std::cout << '\n';

    std::cout << "Scores: ";
    for (const auto& score : person.scores) {
        std::cout << score << " ";
    }
    std::cout << '\n';

    std::cout << "Contacts: " << '\n';
    for (const auto& [platform, id] : person.contacts) {
        std::cout << "  " << platform << ": " << id << '\n';
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
    JsonValue j;
    j["name"]       = p.name;
    j["age"]        = p.age;
    j["is_student"] = p.is_student;
    j["hobbies"]    = p.hobbies;
    j["scores"]     = p.scores;
    j["contacts"]   = p.contacts;
    return j;
}

JsonValue toJson(const Class& c) {
    JsonValue j;
    j["room"]     = c.room;
    j["students"] = c.students;
    return j;
}

void fromJson(const JsonValue& j, Class& c) {
    c.room     = j["room"];
    c.students = j["students"].get<std::vector<Person>>();
}

struct Address {
    std::string country;
    std::string province;

    REFLECT(country, province)
};

// REFLECT_TYPE(Address,country, province)

struct Student {
    std::string name;
    int         age{0};
    int         sex{0};
    Address     address;
};

REFLECT_TYPE(Student, name, age, sex, address)

template <class T, class N>
struct Baby {
    T name{};
    N hungry{};
};

REFLECT_TYPE_TEMPLATED(((Baby<T, N>), class T, class N), name, hungry)

// 运行所有示例
void run_examples() {
    std::cout << "\n=== 运行 JsonValue 使用示例 ===\n" << '\n';

    // 数组索引访问示例
    {
        std::cout << "\n1. 数组索引访问示例:" << '\n';
        JsonValue array = {1, 2, 3, 4, 5};

        std::cout << "Array[0] = " << static_cast<int>(array[0]) << '\n';
        std::cout << "Array[4] = " << static_cast<int>(array[4]) << '\n';

        // 动态增长数组
        array[5]  = 6;   // 自动扩展数组
        array[10] = 11;  // 中间的元素会被自动填充为null
        std::cout << "Array[5] = " << static_cast<int>(array[5]) << '\n';
        std::cout << "Array[10] = " << static_cast<int>(array[10]) << '\n';
        std::cout << "Array[7] is null: " << (array[7].isNull() ? "yes" : "no") << '\n';

        // 嵌套数组访问
        JsonValue nested = JsonValue::Array();
        nested.push_back(JsonValue::Array());
        nested[0] = {1, 2, 3};
        std::cout << "\n2. 嵌套数组访问:" << '\n';
        std::cout << "nested[0][1] = " << static_cast<int>(nested[0][1]) << '\n';

        // 混合访问
        JsonValue mixed = JsonValue::Object();
        mixed["Array"]  = {1, true, "dsa"};
        std::cout << "\n3. 混合访问:" << '\n';
        std::cout << "mixed[\"Array\"][1] = " << mixed["Array"][2].get<std::string>() << '\n';

        // 错误处理示例
        try {
            std::cout << "\n4. 错误处理示例:" << '\n';
            int value = array[100];  // 索引越界
            std::cout << "This line should not be reached" << '\n';
        } catch (const std::runtime_error& e) {
            std::cout << "Expected error: " << e.what() << '\n';
        }
    }

    // 1. 基本类型示例
    {
        std::cout << "1. 基本类型示例:" << '\n';
        JsonValue basic  = JsonValue::Object();
        basic["string"]  = "Hello, World!";
        basic["number"]  = 42;
        basic["boolean"] = true;
        basic["null"]    = nullptr;

        std::string str  = basic["string"];
        int         num  = basic["number"];
        bool        flag = basic["boolean"];

        std::cout << "string: " << str << '\n';
        std::cout << "number: " << num << '\n';
        std::cout << "boolean: " << (flag ? "true" : "false") << '\n';
        std::cout << "null is null: " << (basic["null"].isNull() ? "yes" : "no") << '\n';
    }
    // 2. 数组示例
    {
        std::cout << "\n2. 数组示例:" << '\n';
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
        std::cout << '\n';

        std::cout << "Strings: ";
        for (const auto& s : vec_str)
            std::cout << s << " ";
        std::cout << '\n';
    }

    // 3. 完整的 Person 示例
    {
        std::cout << "\n3. Person 对象示例:" << '\n';
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
        std::cout << "\n4. 错误处理示例:" << '\n';
        try {
            int value = json["non_existent_key"];
            std::cout << "This line should not be reached" << '\n';
        } catch (const std::runtime_error& e) {
            std::cout << "Expected error: " << e.what() << '\n';
        }

        try {
            std::string text = json["age"];  // 尝试将数字转换为字符串
            std::cout << "This line should not be reached" << '\n';
        } catch (const std::runtime_error& e) {
            std::cout << "Expected error: " << e.what() << '\n';
        }
    }

    // 自定义类型反序列化示例
    {
        std::cout << "\n5. 自定义类型反序列化示例:" << '\n';
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
        std::cout << "Name: " << person.name << '\n';
        std::cout << "Age: " << person.age << '\n';
        std::cout << "Is student: " << (person.is_student ? "yes" : "no") << '\n';
        std::cout << "Hobbies: ";
        for (const auto& hobby : person.hobbies) {
            std::cout << hobby << " ";
        }
        std::cout << '\n';
        std::cout << "Scores: ";
        for (const auto& score : person.scores) {
            std::cout << score << " ";
        }
        std::cout << '\n';
        std::cout << "Email: " << person.contacts["email"] << '\n';
        std::cout << "Phone: " << person.contacts["phone"] << '\n';
    }

    {
        // 自定义类型序列化示例
        std::cout << "\n6. 自定义类型序列化示例:" << '\n';
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
                  << '\n';
        std::cout << "Serialized person age: " << json_from_person["age"].get<int>() << '\n';
        std::cout << "Serialized person hobbies: ";
        for (const auto& hobby : json_from_person["hobbies"].get<std::vector<std::string>>()) {
            std::cout << hobby << " ";
        }
        std::cout << '\n';

        // 完整的序列化和反序列化循环测试
        auto restored_person = json_from_person.get<Person>();
        std::cout << "\n7. 序列化和反序列化循环测试:" << '\n';
        std::cout << "Original name: " << alice.name
                  << " -> Restored name: " << restored_person.name << '\n';
        std::cout << "Original age: " << alice.age << " -> Restored age: " << restored_person.age
                  << '\n';
        std::cout << "Original email: " << alice.contacts["email"]
                  << " -> Restored email: " << restored_person.contacts["email"] << '\n';
    }

    {
        std::cout << "\n8. 对象初始化列表示例:" << '\n';
        JsonValue obj = {{"name", "Alice"},
                         {"age", 25},
                         {"scores", {95, 88, 92}},
                         {"info", {{"email", "alice@example.com"}, {"phone", "123-456-7890"}}}};

        std::cout << "Name: " << obj["name"].get<std::string>() << '\n';
        std::cout << "Age: " << obj["age"].get<int>() << '\n';
        std::cout << "Email: " << obj["info"]["email"].get<std::string>() << '\n';

        // 动态添加混合类型对象
        JsonValue dynamic;
        dynamic["data"] = {{"str", "hello"}, {"num", 42}, {"bool", true}};
        std::cout << "\n动态添加的对象:" << '\n';
        std::cout << "String: " << dynamic["data"]["str"].get<std::string>() << '\n';
        std::cout << "Number: " << dynamic["data"]["num"].get<int>() << '\n';
        std::cout << "Boolean: " << dynamic["data"]["bool"].get<bool>() << '\n';

        JsonValue allArray = 213985441333436;
        std::cout << "\n超长数字:\n" << allArray << '\n';

        std::cout << "\n嵌套结构体展示" << '\n';
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
        JsonValue j = c;
        std::cout << "\n结构体转JsonValue\n" << j.toString(4) << '\n';
        auto o = j.get<Class>();
        std::cout << "\nJsonValue转Class: room:" << o.room << '\n';
        std::cout << "\nStudents:" << '\n';
        for (const auto& p : o.students) {
            print_person(p);
            std::cout << '\n';
        }
    }

    {
        std::string s = R"("\uD834\uDD1E")";
        std::cout << "\r10.扩展JSON解析:" << '\n';
        auto v = parser::parse(s, parser::ENABLE_PARSE_X_ESCAPE_SEQUENCE);
        std::cout << v << '\n';
        std::ofstream output_file("o.json");
        if (!output_file.is_open()) {
            std::cerr << "Error: Could not open output.json for writing" << '\n';
            return;
        }

        output_file << v;  // 使用JsonParser::stringify来序列化
        output_file.close();
    }

    {
        std::cout << "\n9.const迭代器展示" << '\n';
        JsonValue dynamic;
        dynamic["data"] = {{"str", "hello"}, {"num", 42}, {"bool", true}};
        dynamic["vec"]  = {1, 2, 3, 4, 5};
        for (auto it = dynamic.begin(); it != dynamic.end(); it++) {
            std::cout << it.key() << " : " << it.value() << '\n';
        }

        std::cout << '\n';
        for (auto& d : dynamic) {
            std::cout << d << '\n';
        }
    }

    {
        std::cout << "\n10.反射注册展示" << '\n';

        {
            std::cout << "\n 普通嵌套类型" << '\n';
            Student student;
            student.name             = "Ping";
            student.age              = 23;
            student.sex              = 1;
            student.address.province = "chengdu";
            student.address.country  = "china";

            std::string data = ccjson::reflect::serialize(student).toString();
            std::cout << data << '\n';

            // 反射序列化
            student = ccjson::reflect::deserialize<Student>(data);
            std::cout << ccjson::reflect::serialize(student) << '\n';
        }

        {
            std::cout << "\n 模板嵌套类型" << '\n';
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
            std::cout << bin << '\n';
            auto o = ccjson::reflect::deserialize<Class>(bin);
            std::cout << ccjson::reflect::serialize(o).toString() << '\n';
        }

        {
            std::cout << "\n 体外模板类型" << '\n';
            Baby<int, float>        baby1{1, 1.2};
            Baby<std::string, bool> baby2{"baby2", false};

            std::string bin1 = ccjson::reflect::serialize(baby1).toString();
            std::cout << bin1 << '\n';

            baby1 = ccjson::reflect::deserialize<decltype(baby1)>(bin1);
            std::cout << ccjson::reflect::serialize(baby1).toString() << '\n';

            std::string bin2 = ccjson::reflect::serialize(baby2).toString();
            std::cout << bin2 << '\n';

            baby2 = ccjson::reflect::deserialize<decltype(baby2)>(bin2);
            std::cout << ccjson::reflect::serialize(baby2).toString() << '\n';
        }

        {
            std::cout << "\n 迭代器修改" << '\n';

            JsonValue vs = {1, 2, 3, 4};
            for (auto it = vs.begin(); it != vs.end(); it++) {
                std::cout << it.value() << ',';
            }
            std::cout << '\n';
            for (auto it = vs.rbegin(); it != vs.rend(); it++) {
                std::cout << *it << ',';
            }
            std::cout << '\n';

            for (auto& s : vs) {
                s = 5;
            }
            std::cout << '\n';
            for (auto it = vs.rbegin(); it != vs.rend(); it++) {
                std::cout << *it << ',';
            }
            std::cout << '\n';
        }
    }
}

}  // namespace ccjson;

int main() {
    ccjson::run_examples();
    return 0;
}