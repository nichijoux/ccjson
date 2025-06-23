#include "json.hpp"
#include <ccjson.h>
#include <chrono>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#ifdef _WIN32
#    include <windows.h>  // 该头文件一定要在psapi.h前
#    include <psapi.h>
#else
#    include <sys/resource.h>
#endif

using json = nlohmann::json;
using namespace ccjson;

// 测试数据结构
struct TestData {
    std::string              name;
    int                      age{};
    std::vector<std::string> tags;
    double                   score{};
    bool                     active{};
    std::vector<int>         numbers;
};

// 生成测试数据
TestData generate_test_data() {
    TestData data;
    data.name    = "Test User";
    data.age     = 30;
    data.tags    = {"tag1", "tag2", "tag3", "tag4", "tag5"};
    data.score   = 95.5;
    data.active  = true;
    data.numbers = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    return data;
}

// 使用ccjson序列化
JsonValue ccjson_serialize(const TestData& data) {
    // 创建一个对象类型的JsonValue
    JsonValue j;

    // 添加基本字段
    j["name"]    = data.name;
    j["name"]    = data.name;
    j["age"]     = data.age;
    j["score"]   = data.score;
    j["active"]  = data.active;
    j["tags"]    = data.tags;
    j["numbers"] = data.numbers;
    return j;
}

// 使用ccjson反序列化
TestData ccjson_deserialize(const JsonValue& json_value) {
    TestData data;

    // 检查是否为对象类型
    if (!json_value.isObject()) {
        throw JsonException("Expected JSON Object");
    }

    // 获取基本字段
    data.name    = json_value["name"].get<std::string>();
    data.age     = json_value["age"];
    data.tags    = json_value["tags"].get<std::vector<std::string>>();
    data.score   = json_value["score"];
    data.active  = json_value["active"];
    data.numbers = json_value["numbers"].get<std::vector<int>>();

    return data;
}

// 使用nlohmann/json序列化
json nlohmann_serialize(const TestData& data) {
    json j;
    j["name"]    = data.name;
    j["age"]     = data.age;
    j["tags"]    = data.tags;
    j["score"]   = data.score;
    j["active"]  = data.active;
    j["numbers"] = data.numbers;
    return j;
}

// 使用nlohmann/json反序列化
TestData nlohmann_deserialize(const json& j) {
    TestData data;

    data.name    = j["name"];
    data.age     = j["age"];
    data.tags    = j["tags"].get<std::vector<std::string>>();
    data.score   = j["score"];
    data.active  = j["active"];
    data.numbers = j["numbers"].get<std::vector<int>>();
    return data;
}

// 测量内存使用
size_t get_memory_usage() {
#ifdef _WIN32
    PROCESS_MEMORY_COUNTERS_EX pmc;
    if (GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc))) {
        return pmc.WorkingSetSize;
    }
#else
    struct rusage usage {};
    if (getrusage(RUSAGE_SELF, &usage) == 0) {
        return usage.ru_maxrss * 1024;  // Convert KB to bytes
    }
#endif
    return 0;
}

// 性能测试函数
void run_performance_test(int iterations) {
    std::cout << "Performance Test (" << iterations << " iterations)\n";
    std::cout << "==========================================\n\n";

    // 准备测试数据
    std::vector<TestData> test_data(iterations);
    for (int i = 0; i < iterations; ++i) {
        test_data[i] = generate_test_data();
    }

    // 测试ccjson序列化
    size_t                 ccjson_memory_start = get_memory_usage();
    auto                   start               = std::chrono::high_resolution_clock::now();
    std::vector<JsonValue> ccjson_results;
    ccjson_results.reserve(iterations);
    for (const auto& data : test_data) {
        ccjson_results.push_back(ccjson_serialize(data));
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto ccjson_serialize_time =
        std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();

    // 测试ccjson反序列化
    start = std::chrono::high_resolution_clock::now();
    std::vector<TestData> ccjson_deserialized;
    ccjson_deserialized.reserve(iterations);
    for (const auto& json_value : ccjson_results) {
        ccjson_deserialized.push_back(ccjson_deserialize(json_value));
    }
    end = std::chrono::high_resolution_clock::now();
    auto ccjson_deserialize_time =
        std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();

    size_t nlohmann_memory_start = get_memory_usage();
    // 测试nlohmann/json序列化
    start = std::chrono::high_resolution_clock::now();
    std::vector<json> nlohmann_results;
    nlohmann_results.reserve(iterations);
    for (const auto& data : test_data) {
        nlohmann_results.push_back(nlohmann_serialize(data));
    }
    end = std::chrono::high_resolution_clock::now();
    auto nlohmann_serialize_time =
        std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();

    // 测试nlohmann/json反序列化
    start = std::chrono::high_resolution_clock::now();
    std::vector<TestData> nlohmann_deserialized;
    nlohmann_deserialized.reserve(iterations);
    for (const auto& j : nlohmann_results) {
        nlohmann_deserialized.push_back(nlohmann_deserialize(j));
    }
    end = std::chrono::high_resolution_clock::now();
    auto nlohmann_deserialize_time =
        std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();

    // 测量内存使用
    size_t ccjson_memory   = get_memory_usage() - ccjson_memory_start;
    size_t nlohmann_memory = get_memory_usage() - nlohmann_memory_start;

    // 输出结果
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "ccjson Results:\n";
    std::cout << "  Serialization time: " << static_cast<double>(ccjson_serialize_time) / 1000.0
              << " ms\n";
    std::cout << "  Deserialization time: " << static_cast<double>(ccjson_deserialize_time) / 1000.0
              << " ms\n";
    std::cout << "  Memory usage: " << static_cast<double>(ccjson_memory) / 1024.0 / 1024.0
              << " MB\n\n";

    std::cout << "nlohmann/json Results:\n";
    std::cout << "  Serialization time: " << static_cast<double>(nlohmann_serialize_time) / 1000.0
              << " ms\n";
    std::cout << "  Deserialization time: "
              << static_cast<double>(nlohmann_deserialize_time) / 1000.0 << " ms\n";
    std::cout << "  Memory usage: " << static_cast<double>(nlohmann_memory) / 1024.0 / 1024.0
              << " MB\n\n";

    // 计算性能比率
    double serialize_ratio =
        static_cast<double>(nlohmann_serialize_time) / static_cast<double>(ccjson_serialize_time);
    double deserialize_ratio = static_cast<double>(nlohmann_deserialize_time) /
                               static_cast<double>(ccjson_deserialize_time);
    double memory_ratio = static_cast<double>(nlohmann_memory) / static_cast<double>(ccjson_memory);

    std::cout << "Performance Comparison (nlohmann/json vs ccjson):\n";
    std::cout << "  Serialization: " << serialize_ratio << "x\n";
    std::cout << "  Deserialization: " << deserialize_ratio << "x\n";
    std::cout << "  Memory usage: " << memory_ratio << "x\n";
}

// 读取文件内容
std::string read_file(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open file: " + filename);
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

// 测试ccjson解析性能
void test_ccjson_parse_performance(const std::string& json_str, int iterations) {
    std::cout << "Testing ccjson parse performance (" << iterations << " iterations)..."
              << std::endl;

    auto start = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < iterations; ++i) {
        JsonValue value = JsonParser::parse(json_str);
    }

    auto end      = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    std::cout << "Parse time: " << duration.count() << "ms" << std::endl;
    std::cout << "Average time per parse: "
              << static_cast<double>(duration.count()) / static_cast<double>(iterations) << "ms"
              << std::endl;
}

// 测试nlohmann/json解析性能
void test_nlohmann_parse_performance(const std::string& json_str, int iterations) {
    std::cout << "Testing nlohmann/json parse performance (" << iterations << " iterations)..."
              << std::endl;

    auto start = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < iterations; ++i) {
        json value = json::parse(json_str);
    }

    auto end      = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    std::cout << "Parse time: " << duration.count() << "ms" << std::endl;
    std::cout << "Average time per parse: "
              << static_cast<double>(duration.count()) / static_cast<double>(iterations) << "ms"
              << std::endl;
}

// 测试ccjson序列化性能
void test_ccjson_stringify_performance(const JsonValue& value, int iterations) {
    std::cout << "Testing ccjson stringify performance (" << iterations << " iterations)..."
              << std::endl;

    auto start = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < iterations; ++i) {
        std::string json_str = JsonParser::stringify(value);
    }

    auto end      = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    std::cout << "Stringify time: " << duration.count() << "ms" << std::endl;
    std::cout << "Average time per stringify: "
              << static_cast<double>(duration.count()) / static_cast<double>(iterations) << "ms"
              << std::endl;
}

// 测试nlohmann/json序列化性能
void test_nlohmann_stringify_performance(const json& value, int iterations) {
    std::cout << "Testing nlohmann/json stringify performance (" << iterations << " iterations)..."
              << std::endl;

    auto start = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < iterations; ++i) {
        std::string json_str = value.dump();
    }

    auto end      = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    std::cout << "Stringify time: " << duration.count() << "ms" << std::endl;
    std::cout << "Average time per stringify: "
              << static_cast<double>(duration.count()) / static_cast<double>(iterations) << "ms"
              << std::endl;
}

// 测试ccjson往返性能
void test_ccjson_roundtrip_performance(const std::string& json_str, int iterations) {
    std::cout << "Testing ccjson roundtrip performance (" << iterations << " iterations)..."
              << std::endl;

    auto start = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < iterations; ++i) {
        JsonValue   value  = JsonParser::parse(json_str);
        std::string result = JsonParser::stringify(value);
    }

    auto end      = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    std::cout << "Roundtrip time: " << duration.count() << "ms" << std::endl;
    std::cout << "Average time per roundtrip: "
              << static_cast<double>(duration.count()) / static_cast<double>(iterations) << "ms"
              << std::endl;
}

// 测试nlohmann/json往返性能
void test_nlohmann_roundtrip_performance(const std::string& json_str, int iterations) {
    std::cout << "Testing nlohmann/json roundtrip performance (" << iterations << " iterations)..."
              << std::endl;

    auto start = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < iterations; ++i) {
        json        value  = json::parse(json_str);
        std::string result = value.dump();
    }

    auto end      = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    std::cout << "Roundtrip time: " << duration.count() << "ms" << std::endl;
    std::cout << "Average time per roundtrip: "
              << static_cast<double>(duration.count()) / static_cast<double>(iterations) << "ms"
              << std::endl;
}

// 测试ccjson内存使用
void test_ccjson_memory_usage(const std::string& json_str) {
    std::cout << "Testing ccjson memory usage..." << std::endl;

    JsonValue   value  = JsonParser::parse(json_str);
    std::string result = value.toString();

    std::cout << "Original JSON size: " << json_str.size() << " bytes" << std::endl;
    std::cout << "Serialized JSON size: " << result.size() << " bytes" << std::endl;
    std::cout << "Size difference: "
              << std::max(result.size(), json_str.size()) - std::min(result.size(), json_str.size())
              << " bytes" << std::endl;
}

// 测试nlohmann/json内存使用
void test_nlohmann_memory_usage(const std::string& json_str) {
    std::cout << "Testing nlohmann/json memory usage..." << std::endl;

    json        value  = json::parse(json_str);
    std::string result = value.dump();

    std::cout << "Original JSON size: " << json_str.size() << " bytes" << std::endl;
    std::cout << "Serialized JSON size: " << result.size() << " bytes" << std::endl;
    std::cout << "Size difference: "
              << std::max(result.size(), json_str.size()) - std::min(result.size(), json_str.size())
              << " bytes" << std::endl;
}

int main() {
    try {
        // 运行不同规模的测试
        std::cout << "Running performance tests...\n\n";

        run_performance_test(1000);  // 小规模测试
        std::cout << "\n----------------------------------------\n\n";
        run_performance_test(10000);  // 中等规模测试
        std::cout << "\n----------------------------------------\n\n";
        run_performance_test(100000);  // 大规模测试

        // 读取twitter.json文件
        std::string json_str = read_file("twitter.json");
        std::cout << "Twitter.json size: " << json_str.size() << " bytes" << std::endl;

        // 设置测试迭代次数
        const int iterations = 500;

        // 运行性能测试
        std::cout << "\n=== Twitter.json Performance Tests ===\n" << std::endl;

        // 测试解析性能
        std::cout << "\n--- Parse Performance ---" << std::endl;
        test_ccjson_parse_performance(json_str, iterations);
        test_nlohmann_parse_performance(json_str, iterations);

        // 测试序列化性能
        std::cout << "\n--- Stringify Performance ---" << std::endl;
        JsonValue ccjson_value   = JsonParser::parse(json_str);
        json      nlohmann_value = json::parse(json_str);
        test_ccjson_stringify_performance(ccjson_value, iterations);
        test_nlohmann_stringify_performance(nlohmann_value, iterations);

        // 测试往返性能
        std::cout << "\n--- Roundtrip Performance ---" << std::endl;
        test_ccjson_roundtrip_performance(json_str, iterations);
        test_nlohmann_roundtrip_performance(json_str, iterations);

        // 测试内存使用
        std::cout << "\n--- Memory Usage ---" << std::endl;
        test_ccjson_memory_usage(json_str);
        test_nlohmann_memory_usage(json_str);

        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}