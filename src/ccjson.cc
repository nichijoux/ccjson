#pragma clang diagnostic push
#pragma ide diagnostic ignored "misc-no-recursion"
#include "ccjson.h"
#include <cmath>
#include <sstream>

namespace ccjson {

JsonValue::JsonValue(const JsonValue& other) : m_type(other.m_type) {
    switch (m_type) {
        case JsonType::Null:
        case JsonType::Boolean: m_value.boolean = other.m_value.boolean; break;
        case JsonType::Integer: m_value.iNumber = other.m_value.iNumber; break;
        case JsonType::Double: m_value.dNumber = other.m_value.dNumber; break;
        case JsonType::String: m_value.string = new std::string(*other.m_value.string); break;
        case JsonType::Array: m_value.array = new JsonArray(*other.m_value.array); break;
        case JsonType::Object: m_value.object = new JsonObject(*other.m_value.object); break;
    }
}

JsonValue::JsonValue(JsonValue&& other) noexcept : m_type(other.m_type) {
    m_value              = other.m_value;
    other.m_type         = JsonType::Null;
    other.m_value.object = nullptr;
}

JsonValue& JsonValue::operator=(const JsonValue& other) {
    if (this != &other) {
        destroyValue();
        m_type = other.m_type;
        switch (m_type) {
            case JsonType::Null:
            case JsonType::Boolean: m_value.boolean = other.m_value.boolean; break;
            case JsonType::Integer: m_value.iNumber = other.m_value.iNumber; break;
            case JsonType::Double: m_value.dNumber = other.m_value.dNumber; break;
            case JsonType::String: m_value.string = new std::string(*other.m_value.string); break;
            case JsonType::Array: m_value.array = new JsonArray(*other.m_value.array); break;
            case JsonType::Object: m_value.object = new JsonObject(*other.m_value.object); break;
        }
    }
    return *this;
}

JsonValue& JsonValue::operator=(JsonValue&& other) noexcept {
    if (this != &other) {
        destroyValue();
        m_type               = other.m_type;
        m_value              = other.m_value;
        other.m_type         = JsonType::Null;
        other.m_value.object = nullptr;
    }
    return *this;
}

JsonValue::~JsonValue() {
    destroyValue();
}

JsonValue& JsonValue::set(const std::string& key, const JsonValue& value) {
    if (m_type != JsonType::Object) {
        destroyValue();
        m_type         = JsonType::Object;
        m_value.object = new JsonObject();
    }
    (*m_value.object)[key] = value;
    return *this;
}

JsonValue& JsonValue::push_back(JsonValue value) {
    if (!isArray()) {
        destroyValue();
        m_type        = JsonType::Array;
        m_value.array = new JsonArray();
    }
    m_value.array->emplace_back(std::move(value));
    return *this;
}

JsonValue::operator bool() const {
    if (!isBoolean()) {
        throw JsonException("Cannot convert to bool");
    }
    return m_value.boolean;
}

JsonValue::operator int16_t() const {
    if (!isNumber()) {
        throw JsonException("Cannot convert to int");
    }
    if (m_type == JsonType::Integer) {
        return static_cast<int16_t>(m_value.iNumber);
    }
    return static_cast<int16_t>(m_value.dNumber);
}

JsonValue::operator int32_t() const {
    if (!isNumber()) {
        throw JsonException("Cannot convert to int");
    }
    if (m_type == JsonType::Integer) {
        return static_cast<int32_t>(m_value.iNumber);
    }
    return static_cast<int32_t>(m_value.dNumber);
}

JsonValue::operator int64_t() const {
    if (!isNumber()) {
        throw JsonException("Cannot convert to int");
    }
    if (m_type == JsonType::Integer) {
        return m_value.iNumber;
    }
    return static_cast<int64_t>(m_value.dNumber);
}

JsonValue::operator float() const {
    if (!isNumber()) {
        throw JsonException("Cannot convert to double");
    }
    if (m_type == JsonType::Double) {
        return static_cast<float>(m_value.dNumber);
    }
    return static_cast<float>(m_value.iNumber);
}

JsonValue::operator double() const {
    if (!isNumber()) {
        throw JsonException("Cannot convert to double");
    }
    if (m_type == JsonType::Double) {
        return m_value.dNumber;
    }
    return static_cast<double>(m_value.iNumber);
}

JsonValue::operator std::string() const {
    if (!isString()) {
        throw JsonException("Cannot convert to string");
    }
    return *m_value.string;
}

std::string JsonValue::toString(int indent) const {
    return JsonParser::stringify(*this, indent);
}

void JsonValue::destroyValue() noexcept {
    switch (m_type) {
        // 动态分配的内存
        case JsonType::String: delete m_value.string; break;
        case JsonType::Array: delete m_value.array; break;
        case JsonType::Object: delete m_value.object; break;
        default: break;
    }
}

#define SKIP_USELESS_CHAR(str, position)                                                           \
    do {                                                                                           \
        while (position < str.size() && std::isspace(str[position])) {                             \
            ++position;                                                                            \
        }                                                                                          \
    } while (false)

JsonValue JsonParser::parse(std::string_view json, uint8_t option) {
    size_t    position = 0;
    JsonValue result   = parseValue(json, position, option);
    SKIP_USELESS_CHAR(json, position);
    // 按道理现在应该不存在json数据了
    if (position != json.size()) {
        throw JsonParseException("Unexpected content after JSON value", position);
    }
    return result;
}

JsonValue JsonParser::parseValue(std::string_view json, size_t& position, uint8_t option) {
    // 跳过无用字符
    SKIP_USELESS_CHAR(json, position);
    // 范围检测
    if (position >= json.size()) {
        throw JsonParseException("Unexpected end of input", position);
    }
    // 根据第一个字符判断接来要解析什么
    char c = json[position];
    switch (c) {
        case 'n': return parseNull(json, position);
        case 't':
        case 'f': return parseBoolean(json, position);
        case '"': return parseString(json, position, option);
        case '[': return parseArray(json, position, option);
        case '{': return parseObject(json, position, option);
        case '-':
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9': return parseNumber(json, position);
        default: throw JsonParseException("Unexpected character: " + std::to_string(c), position);
    }
}

JsonValue JsonParser::parseNull(std::string_view json, size_t& position) {
    // 当前字符一定为n
    // json[pos] - json[pos + 3]
    if (json.substr(position, 4) != "null") {
        throw JsonParseException("Expected 'null'", position);
    }
    position += 4;
    return nullptr;
}

JsonValue JsonParser::parseBoolean(std::string_view json, size_t& position) {
    // 当前字符一定为t或f
    if (json.substr(position, 4) == "true") {
        position += 4;
        return true;
    } else if (json.substr(position, 5) == "false") {
        position += 5;
        return false;
    }
    throw JsonParseException("Expected 'true' or 'false'", position);
}

JsonValue JsonParser::parseNumber(std::string_view json, size_t& position) {
    // 解析数字,数字格式为-?(0|[1-9]\d*)(\.\d+)?([eE][+-]?\d+)?
    size_t start     = position;
    bool   isInteger = true;  // 假设是整数，除非发现小数点或指数

    // 处理负数
    position += json[start] == '-';
    bool isNegative = json[start] == '-';

    // 处理整数部分（不跳过前导零，仅检查合法性）
    if (json[position] == '0') {
        position++;
        // 禁止前导零后跟其他数字（如 "012"）
        if (position < json.size() && std::isdigit(json[position])) {
            throw JsonParseException("Leading zeros are not allowed", position);
        }
    } else {
        while (position < json.size() && std::isdigit(json[position])) {
            position++;
        }
    }
    // 处理小数
    if (position < json.size() && json[position] == '.') {
        isInteger = false;
        position++;
        // . 后跟至少一个数字（如 .5 或 3.14）
        if (position >= json.size() || !std::isdigit(json[position])) {
            throw JsonParseException("Invalid number format, decimal heed at least 1 number",
                                     position);
        }
        while (position < json.size() && std::isdigit(json[position])) {
            position++;
        }
    }
    // 处理指数
    if (position < json.size() && json[position] == 'e') {
        isInteger = false;
        // e 或 E 后跟可选符号（+/-）和至少一个数字（如 1e3 或 2E-5）
        // 处理符号e
        position++;
        // 处理符号+-
        if (json[position] == '+' || json[position] == '-') {
            position++;
        }
        // 处理数字
        if (position >= json.size() || !std::isdigit(json[position])) {
            throw JsonParseException("Invalid number format, exponent need at least 1 number",
                                     position);
        }
        while (position < json.size() && std::isdigit(json[position])) {
            position++;
        }
    }
    std::string numStr(json.substr(start, position - start));
    // 将数值转换为数字
    try {
        if (isInteger) {
            // 快速检查：如果字符串过长，直接按 double 处理
            if (numStr.length() > 20 ||
                (numStr.length() == 20 &&
                 numStr > (isNegative ? "-9223372036854775808" : "9223372036854775807"))) {
                return std::stod(numStr);
            }

            // 尝试解析为 int64_t
            int64_t intValue = std::stoll(numStr);
            return intValue;
        } else {
            // 直接解析为double
            return std::stod(numStr);
        }
    } catch (const std::out_of_range&) {
        // 如果整数解析溢出，转为double
        return std::stod(numStr);
    } catch (const std::invalid_argument& e) {
        throw JsonParseException("Invalid number format, because " + std::string(e.what()),
                                 position);
    }
}

JsonValue JsonParser::parseString(std::string_view json, size_t& position, uint8_t option) {
    // 当前字符串一定为"
    // 跳过开头的"
    position++;
    JsonString result;
    result.reserve(32);  // 初始预留空间，避免频繁扩容
    // 读取后面的字符串
    while (position < json.size()) {
        char c = json[position++];
        if (c == '"') {
            return std::move(result);
        } else if (c == '\\') {
            // 如果c为\,说明遇到了转移字符
            if (position >= json.size()) {
                throw JsonParseException("Unexpected end of string", position);
            }
            // 转义字符后的字符
            c = json[position++];
            switch (c) {
                case '"': result += '"'; break;
                case '\\': result += '\\'; break;
                case '/': result += '/'; break;
                case 'b': result += '\b'; break;
                case 'f': result += '\f'; break;
                case 'n': result += '\n'; break;
                case 'r': result += '\r'; break;
                case 't': result += '\t'; break;
                case 'u': {
                    // unicode字符\u后必须跟随4个字符（代表码点）
                    if (position + 4 >= json.size()) {
                        throw JsonParseException("Invalid unicode escape sequence", position);
                    }
                    auto hex = json.substr(position, 4);
                    position += 4;

                    // 解析unicode
                    try {
                        // 转换为 Unicode 码点
                        char32_t codePoint = std::stoul(hex.data(), nullptr, 16);
                        // 检查是否是代理对（Surrogate Pair）
                        if (codePoint >= 0xD800 && codePoint <= 0xDBFF) {
                            // 高代理（High Surrogate），需要低代理（Low Surrogate）
                            if (position + 6 > json.size() || json[position] != '\\' ||
                                json[position + 1] != 'u') {
                                throw JsonParseException("Missing low surrogate in \\u escape",
                                                         position);
                            }
                            position += 2;  // 跳过 \u

                            // 解析低代理
                            char32_t lowSurrogate =
                                std::stoul(json.substr(position, 4).data(), nullptr, 16);
                            position += 4;

                            if (lowSurrogate < 0xDC00 || lowSurrogate > 0xDFFF) {
                                throw JsonParseException("Invalid low surrogate in \\u escape",
                                                         position);
                            }

                            // 计算完整码点
                            codePoint =
                                0x10000 + ((codePoint - 0xD800) << 10) + (lowSurrogate - 0xDC00);
                        } else if (codePoint >= 0xDC00 && codePoint <= 0xDFFF) {
                            // 单独的低代理（非法）
                            throw JsonParseException("Unpaired low surrogate in \\u escape",
                                                     position);
                        }
                        // 根据 Unicode 码点范围，按 UTF-8 规则分段编码：
                        if (codePoint <= 0x007F) {
                            // 0x0000~0x007F (1字节)
                            result.push_back(static_cast<char>(codePoint));
                        } else if (codePoint <= 0x07FF) {
                            // 0x0080~0x07FF (2字节)
                            result.push_back(static_cast<char>(0xC0 | (codePoint >> 6)));
                            result.push_back(static_cast<char>(0x80 | (codePoint & 0x3F)));
                        } else if (codePoint <= 0xFFFF) {
                            // 0x0800~0xFFFF (3字节)
                            result.push_back(static_cast<char>(0xE0 | (codePoint >> 12)));
                            result.push_back(static_cast<char>(0x80 | ((codePoint >> 6) & 0x3F)));
                            result.push_back(static_cast<char>(0x80 | (codePoint & 0x3F)));
                        } else {
                            // 0x10000~0x10FFFF (4字节)
                            result.push_back(static_cast<char>(0xF0 | (codePoint >> 18)));
                            result.push_back(static_cast<char>(0x80 | ((codePoint >> 12) & 0x3F)));
                            result.push_back(static_cast<char>(0x80 | ((codePoint >> 6) & 0x3F)));
                            result.push_back(static_cast<char>(0x80 | (codePoint & 0x3F)));
                        }
                    } catch (...) {
                        throw JsonParseException("Invalid unicode escape sequence", position);
                    }
                    break;
                }
                case 'x': {
                    if (option & ENABLE_PARSE_X_ESCAPE_SEQUENCE) {
                        // 收集所有连续的 \xHH 字节
                        std::vector<uint8_t> utf8Bytes;
                        while (position + 2 < json.size()) {
                            auto hex = json.substr(position, 2);
                            position += 2;
                            try {
                                uint8_t byte =
                                    static_cast<uint8_t>(std::stoul(hex.data(), nullptr, 16));
                                utf8Bytes.push_back(byte);
                            } catch (...) {
                                throw JsonParseException("Invalid \\x escape sequence", position);
                            }
                            // 检查是否还有下一个 \x
                            if (position + 3 < json.size() && json[position] == '\\' &&
                                json[position + 1] == 'x') {
                                position += 2;  // 跳过 \x
                            } else {
                                break;
                            }
                        }

                        if (utf8Bytes.empty()) {
                            throw JsonParseException("Empty \\x escape sequence", position);
                        }

                        // 解码 UTF-8 → Unicode 码点
                        char32_t codePoint = 0;
                        size_t   i         = 0;
                        if (utf8Bytes[0] <= 0x7F) {
                            codePoint = utf8Bytes[0];
                            i         = 1;
                        } else if ((utf8Bytes[0] & 0xE0) == 0xC0) {
                            codePoint = ((utf8Bytes[0] & 0x1F) << 6) | (utf8Bytes[1] & 0x3F);
                            i         = 2;
                        } else if ((utf8Bytes[0] & 0xF0) == 0xE0) {
                            codePoint = ((utf8Bytes[0] & 0x0F) << 12) |
                                        ((utf8Bytes[1] & 0x3F) << 6) | (utf8Bytes[2] & 0x3F);
                            i = 3;
                        } else if ((utf8Bytes[0] & 0xF8) == 0xF0) {
                            codePoint = ((utf8Bytes[0] & 0x07) << 18) |
                                        ((utf8Bytes[1] & 0x3F) << 12) |
                                        ((utf8Bytes[2] & 0x3F) << 6) | (utf8Bytes[3] & 0x3F);
                            i = 4;
                        }

                        // 检查是否完全解码
                        if (i != utf8Bytes.size()) {
                            throw JsonParseException("Invalid UTF-8 sequence", position);
                        }

                        // 编码 Unicode 码点 → \uXXXX or \uXXXX\uXXXX (代理对)
                        if (codePoint <= 0xFFFF) {
                            // 基本平面（BMP）
                            char buffer[7];
                            snprintf(buffer, sizeof(buffer), "\\u%04X", codePoint);
                            result += buffer;
                        } else {
                            // 补充平面（Surrogate Pair）
                            codePoint -= 0x10000;
                            auto highSurrogate = static_cast<uint16_t>((codePoint >> 10) + 0xD800);
                            auto lowSurrogate = static_cast<uint16_t>((codePoint & 0x3FF) + 0xDC00);
                            char buffer[7];
                            snprintf(buffer, sizeof(buffer), "\\u%04X", highSurrogate);
                            result += buffer;
                            snprintf(buffer, sizeof(buffer), "\\u%04X", lowSurrogate);
                            result += buffer;
                        }
                    } else {
                        throw JsonParseException("not support parse x escape sequence", position);
                    }
                    break;
                }
                case '0': {
                    if (option & ENABLE_PARSE_0_ESCAPE_SEQUENCE) {
                        // \0 转为 \u0000
                        result += "\\u0000";
                    } else {
                        throw JsonParseException("Invalid \\0 escape sequence", position);
                    }
                    break;
                }
                default: throw JsonParseException("Invalid escape sequence", position);
            }
        } else {
            result.push_back(c);
        }
    }
    // 异常
    throw JsonParseException("Unexpected end of string", position);
}

JsonValue JsonParser::parseArray(std::string_view json, size_t& position, uint8_t option) {
    // 当前字符一定为[
    JsonArray result;
    // 跳过[
    position++;
    // 跳过无用字符串
    SKIP_USELESS_CHAR(json, position);
    if (position >= json.size()) {
        throw JsonParseException("Unexpected end of Array", position);
    }
    // 如果遇到了]
    if (json[position] == ']') {
        position++;
        return result;
    }
    // 预分配空间
    result.reserve(32);
    // 解析后面的 value
    while (true) {
        // 跳过无用字符
        SKIP_USELESS_CHAR(json, position);
        // 解析值
        result.emplace_back(parseValue(json, position, option));
        // 无用字符
        SKIP_USELESS_CHAR(json, position);
        if (position >= json.size()) {
            throw JsonParseException("Unexpected end of Array", position);
        }
        // 如果遇到了]
        if (json[position] == ']') {
            position++;
            return result;
        }
        // 如果不是]那就必须为
        if (json[position] != ',') {
            throw JsonParseException("Expected ',' or ']'", position);
        }
        position++;
    }
}

JsonValue JsonParser::parseObject(std::string_view json, size_t& position, uint8_t option) {
    // 当前字符一定为{
    JsonObject object;
    position++;
    // 跳过无用字符
    SKIP_USELESS_CHAR(json, position);
    // 范围检测
    if (position >= json.size()) {
        throw JsonParseException("Unexpected end of Object", position);
    }
    // 是否已经结束
    if (json[position] == '}') {
        position++;
        return object;
    }
    // 说明存在值
    while (true) {
        SKIP_USELESS_CHAR(json, position);
        // 解析key
        if (json[position] != '"') {
            throw JsonParseException("the key of obejct must be a string", position);
        }
        JsonValue key = parseString(json, position, option);
        SKIP_USELESS_CHAR(json, position);
        // 是否超范围,或者是否没有:
        if (position >= json.size() || json[position] != ':') {
            throw JsonParseException("Unexpected end of Object", position);
        }
        position++;
        // 准备解析值
        JsonValue value = parseValue(json, position, option);
        if (position >= json.size()) {
            throw JsonParseException("Unexpected end of Array", position);
        }
        object.emplace(key.get<std::string>(), value);
        SKIP_USELESS_CHAR(json, position);
        // 如果遇到了}
        if (json[position] == '}') {
            position++;
            return object;
        }
        // 如果不是]那就必须为
        if (json[position] != ',') {
            throw JsonParseException("Expected ',' or ']'", position);
        }
        position++;
    }
}

#undef SKIP_USELESS_CHAR

std::string JsonParser::stringify(const JsonValue& value, int indent) {
    std::ostringstream oss;
    stringifyValue(value, oss, indent, 0);
    return oss.str();
}

void JsonParser::stringifyValue(const JsonValue&    value,
                                std::ostringstream& oss,
                                int                 indent,
                                int                 level) {
    // 根据类型调用不同的stringify
#define STRINGIFY(type)                                                                            \
    case JsonType::type: return stringify##type(value, oss, indent, level)
    switch (value.type()) {
        STRINGIFY(Null);
        STRINGIFY(Boolean);
        STRINGIFY(Integer);
        STRINGIFY(Double);
        STRINGIFY(String);
        STRINGIFY(Array);
        STRINGIFY(Object);
    }
#undef STRINGIFY
}

void JsonParser::stringifyNull(const ccjson::JsonValue& value, std::ostringstream& oss, int, int) {
    oss << "null";
}

void JsonParser::stringifyBoolean(const ccjson::JsonValue& value,
                                  std::ostringstream&      oss,
                                  int,
                                  int) {
    oss << (value.get<bool>() ? "true" : "false");
}

void JsonParser::stringifyInteger(const ccjson::JsonValue& value,
                                  std::ostringstream&      oss,
                                  int,
                                  int) {
    auto num = value.get<int64_t>();
    if (std::isfinite(num)) {
        oss << num;
    } else {
        throw JsonException("Cannot stringify infinite or NaN number");
    }
}

void JsonParser::stringifyDouble(const ccjson::JsonValue& value,
                                 std::ostringstream&      oss,
                                 int,
                                 int) {
    auto num = value.get<double>();
    if (std::isfinite(num)) {
        oss << num;
    } else {
        throw JsonException("Cannot stringify infinite or NaN number");
    }
}

void JsonParser::stringifyString(const ccjson::JsonValue& value,
                                 std::ostringstream&      oss,
                                 int                      indent,
                                 int                      level) {
    stringifyString(value.get<std::string>(), oss);
}

void JsonParser::stringifyString(const std::string& value, std::ostringstream& oss) {
    oss << '"';
    for (char c : value) {
        switch (c) {
            case '"': oss << "\\\""; break;
            case '\\': oss << "\\\\"; break;
            case '\b': oss << "\\b"; break;
            case '\f': oss << "\\f"; break;
            case '\n': oss << "\\n"; break;  // 正确转义 \n
            case '\r': oss << "\\r"; break;  // 正确转义 \r
            case '\t': oss << "\\t"; break;
            default: oss << c; break;
        }
    }
    oss << '"';
}

void JsonParser::stringifyArray(const ccjson::JsonValue& value,
                                std::ostringstream&      oss,
                                int                      indent,
                                int                      level) {
    const auto& array = value.get<JsonArray>();
    if (array.empty()) {
        oss << "[]";
        return;
    }
    oss << "[";
    for (size_t i = 0; i < array.size(); i++) {
        if (i > 0) {
            oss << ",";
        }
        if (indent != 0) {
            oss << "\n";
        }
        oss << std::string((level + 1) * indent, ' ');
        stringifyValue(array[i], oss, indent, level + 1);
    }
    if (indent != 0) {
        oss << "\n" << std::string(level * indent, ' ');
    }
    oss << "]";
}

void JsonParser::stringifyObject(const ccjson::JsonValue& value,
                                 std::ostringstream&      oss,
                                 int                      indent,
                                 int                      level) {
    const auto& object = value.get<JsonObject>();
    if (object.empty()) {
        oss << "{}";
        return;
    }
    oss << "{";
    bool first = true;
    for (const auto& [key, val] : object) {
        if (!first) {
            oss << ',';
        }
        if (indent != 0) {
            oss << "\n";
        }
        oss << std::string((level + 1) * indent, ' ');
        first = false;
        stringifyString(key, oss);
        oss << ':';
        stringifyValue(val, oss, indent, level + 1);
    }
    if (indent != 0) {
        oss << "\n" << std::string(level * indent, ' ');
    }
    oss << '}';
}

JsonValue::ConstIterator::value_type& JsonValue::ConstIterator::operator*() const {
    return std::visit(
        [this](const auto& it) -> reference {
            using T = std::decay_t<decltype(it)>;
            if constexpr (std::is_same_v<T, JsonObject::const_iterator>) {
                return it->second;  // For objects, return the value part of the pair
            } else if constexpr (std::is_same_v<T, JsonArray::const_iterator>) {
                return *it;  // For arrays, return the element
            } else {
                return *m_host;  // For simple types, return the value itself
            }
        },
        m_iterator);
}

std::string JsonValue::ConstIterator::key() const {
    return std::visit(
        [](const auto& it) -> std::string {
            using T = std::decay_t<decltype(it)>;
            if constexpr (std::is_same_v<T, JsonObject ::const_iterator>) {
                return it->first;
            }
            throw JsonException("cannot use key() for non-Object iterators");
        },
        m_iterator);
}

}  // namespace ccjson
#pragma clang diagnostic pop