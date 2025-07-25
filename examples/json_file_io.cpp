#include <ccjson.h>
#include <fstream>
#include <iostream>
#include <string>

using namespace ccjson;

int main() {
    try {
        // 读取twitter.json文件
        std::ifstream input_file("twitter.json");
        if (!input_file.is_open()) {
            std::cerr << "Error: Could not open twitter.json" << std::endl;
            return 1;
        }

        // 将文件内容读入字符串
        std::string json_str((std::istreambuf_iterator<char>(input_file)),
                             std::istreambuf_iterator<char>());
        input_file.close();

        // 解析JSON
        JsonValue json;
        try {
            json = JsonParser::parse(json_str);
            std::cout << "Successfully parsed twitter.json" << std::endl;

            // 打印JSON内容
            std::cout << "\nJSON Content:" << std::endl;
            std::cout << "----------------------------------------" << std::endl;
            std::cout << json << std::endl;
            std::cout << "----------------------------------------" << std::endl;

        } catch (const JsonException& e) {
            std::cerr << "Parse error: " << e.what() << std::endl;
            return 1;
        }

        // 验证JSON结构
        if (!json.isObject()) {
            std::cerr << "Error: Root element is not an Object" << std::endl;
            return 1;
        }

        // 将解析后的JSON写入output.json
        std::ofstream output_file("output.json");
        if (!output_file.is_open()) {
            std::cerr << "Error: Could not open output.json for writing" << std::endl;
            return 1;
        }

        output_file << JsonParser::stringify(json, 4);  // 使用JsonParser::stringify来序列化
        output_file.close();

        std::cout << "Successfully wrote to output.json" << std::endl;
        return 0;

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}