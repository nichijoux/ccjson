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
            json = parser::parse(json_str);
            std::cout << "Successfully parsed twitter.json" << '\n';

            // 打印JSON内容
            std::cout << "\nJSON Content:" << '\n';
            std::cout << "----------------------------------------" << '\n';
            std::cout << json << '\n';
            std::cout << "----------------------------------------" << '\n';

        } catch (const JsonException& e) {
            std::cerr << "Parse error: " << e.what() << '\n';
            return 1;
        }

        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}