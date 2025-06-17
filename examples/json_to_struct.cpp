#include <ccjson.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using namespace ccjson;

// 定义结构体
struct User {
    double                   id{};
    std::string              name;
    std::string              screen_name;
    std::string              location;
    std::string              description;
    int                      followers_count{};
    int                      friends_count{};
    std::vector<std::string> hashtags;
};

// 从JSON解析到结构体的函数
User parse_user_from_json(const JsonValue& json) {
    User user;

    // 检查是否为对象类型
    if (!json.isObject()) {
        throw JsonException("Expected JSON Object for user");
    }

    // 获取基本字段
    user.id              = json["id"];
    user.name            = json["name"].get<std::string>();
    user.screen_name     = json["screen_name"].get<std::string>();
    user.location        = json["location"].get<std::string>();
    user.description     = json["description"].get<std::string>();
    user.followers_count = json["followers_count"];
    user.friends_count   = json["friends_count"];

    return user;
}

// 将结构体转换为JSON的函数
JsonValue user_to_json(const User& user) {
    JsonValue json;

    // 设置基本字段
    json["id_str"]          = user.id;  // 使用id_str而不是id
    json["name"]            = user.name;
    json["screen_name"]     = user.screen_name;
    json["location"]        = user.location;
    json["description"]     = user.description;
    json["followers_count"] = static_cast<double>(user.followers_count);
    json["friends_count"]   = static_cast<double>(user.friends_count);

    // 创建entities对象
    json["entities"] = JsonValue(std::map<std::string, JsonValue>());

    // 创建hashtags数组
    json["entities"]["hashtags"] = JsonValue(std::vector<JsonValue>());

    // 使用新的API添加hashtags
    for (const auto& hashtag : user.hashtags) {
        JsonValue hashtag_obj;
        hashtag_obj["text"] = hashtag;
        json["entities"]["hashtags"].push_back(hashtag_obj);
    }

    return json;
}

// 演示修改JSON的函数
void demonstrate_json_modification(JsonValue& json) {
    std::cout << "\nDemonstrating JSON Modification:" << std::endl;
    std::cout << "----------------------------------------" << std::endl;

    // 获取第一个status的用户信息
    if (!json["statuses"].isArray() || json["statuses"].get<JsonArray>().empty()) {
        throw JsonException("No statuses found in JSON");
    }

    JsonValue& user = json["statuses"][0]["user"];

    // 1. 修改现有值
    std::cout << "1. Modifying existing values:" << std::endl;
    user["name"]            = "Modified Name";
    user["followers_count"] = 9999;
    std::cout << "   Modified name: " << user["name"] << std::endl;
    std::cout << "   Modified followers: " << user["followers_count"] << std::endl;

    // 2. 添加新的hashtag
    std::cout << "\n2. Adding new hashtag:" << std::endl;
    JsonValue new_hashtag;
    new_hashtag["text"] = "new_hashtag";
    user["entities"]["hashtags"].push_back(new_hashtag);
    std::cout << "   Added new hashtag" << std::endl;

    // 3. 使用insert添加新的字段
    std::cout << "\n3. Adding new field using insert:" << std::endl;
    user.set("new_field", JsonValue("new value"));
    std::cout << "   Added new field: " << user["new_field"] << std::endl;

    // 4. 检查并修改hashtags
    std::cout << "\n4. Checking and modifying hashtags:" << std::endl;
    auto hashtags = user["entities"]["hashtags"].get<JsonArray>();
    if (!hashtags.empty()) {
        hashtags[0]["text"] = "modified_hashtag";
        std::cout << "   Modified first hashtag" << std::endl;
    }

    // 5. 使用contains检查字段
    std::cout << "\n5. Checking field existence:" << std::endl;
    if (user.get<JsonObject>().count("new_field") != 0) {
        std::cout << "   'new_field' exists" << std::endl;
    }

    std::cout << "----------------------------------------" << std::endl;
}

int main() {
    try {
        // 读取JSON文件
        std::ifstream input_file("twitter.json");
        if (!input_file.is_open()) {
            std::cerr << "Error: Could not open twitter.json" << std::endl;
            return 1;
        }

        // 读取文件内容
        std::string json_str((std::istreambuf_iterator<char>(input_file)),
                             std::istreambuf_iterator<char>());
        input_file.close();

        // 解析JSON
        JsonValue json = JsonParser::parse(json_str);

        // 获取第一个status的用户信息
        if (!json["statuses"].isArray() || json["statuses"].get<JsonArray>().empty()) {
            throw JsonException("No statuses found in JSON");
        }

        // 将JSON解析到结构体
        User user = parse_user_from_json(json["statuses"][0]["user"]);

        // 打印原始结构体内容
        std::cout << "Original User Information:" << std::endl;
        std::cout << "----------------------------------------" << std::endl;
        std::cout << "ID: " << user.id << std::endl;
        std::cout << "Name: " << user.name << std::endl;
        std::cout << "Screen Name: " << user.screen_name << std::endl;
        std::cout << "Location: " << user.location << std::endl;
        std::cout << "Description: " << user.description << std::endl;
        std::cout << "Followers: " << user.followers_count << std::endl;
        std::cout << "Friends: " << user.friends_count << std::endl;
        std::cout << "Hashtags: ";
        for (const auto& hashtag : user.hashtags) {
            std::cout << "#" << hashtag << " ";
        }
        std::cout << std::endl;
        std::cout << "----------------------------------------" << std::endl;

        // 演示JSON修改
        demonstrate_json_modification(json);

        // 将修改后的JSON保存
        std::ofstream output_file("user_output.json");
        if (!output_file.is_open()) {
            std::cerr << "Error: Could not open user_output.json for writing" << std::endl;
            return 1;
        }
        output_file << JsonParser::stringify(json);
        output_file.close();

        std::cout << "\nSuccessfully wrote modified JSON to user_output.json" << std::endl;
        return 0;

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}