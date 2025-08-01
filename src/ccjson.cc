#pragma clang diagnostic push
#pragma ide diagnostic ignored "misc-no-recursion"
#include "ccjson.h"
#include <charconv>
#include <cmath>
#include <iomanip>
#include <sstream>

namespace ccjson {

JsonValue::JsonValue(const JsonValue& other) : m_type(other.m_type) {
    switch (m_type) {
        case JsonType::Null: break;
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
            case JsonType::Null: break;
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
        throw JsonException("Cannot convert to int16_t");
    }
    if (m_type == JsonType::Integer) {
        return static_cast<int16_t>(m_value.iNumber);
    }
    return static_cast<int16_t>(m_value.dNumber);
}

JsonValue::operator int32_t() const {
    if (!isNumber()) {
        throw JsonException("Cannot convert to int32_t");
    }
    if (m_type == JsonType::Integer) {
        return static_cast<int32_t>(m_value.iNumber);
    }
    return static_cast<int32_t>(m_value.dNumber);
}

JsonValue::operator int64_t() const {
    if (!isNumber()) {
        throw JsonException("Cannot convert to int64_t");
    }
    if (m_type == JsonType::Integer) {
        return m_value.iNumber;
    }
    return static_cast<int64_t>(m_value.dNumber);
}

JsonValue::operator float() const {
    if (!isNumber()) {
        throw JsonException("Cannot convert to float");
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
    return parser::stringify(*this, indent);
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

/**
 * @brief 跳过json的无用字符串。
 * @param str 输入 JSON 字符串。
 * @param position 当前解析位置（输入输出参数）
 * @note 不可使用std::isspace,json规范中定义的ws只可为空格,\\t,\\n,\\r
 */
#define SKIP_USELESS_CHAR(str, position)                                                           \
    do {                                                                                           \
        while (position < str.size() && ((str[position]) == ' ' || (str[position]) == '\t' ||      \
                                         (str[position]) == '\n' || (str[position]) == '\r')) {    \
            ++position;                                                                            \
        }                                                                                          \
    } while (false)

/**
 * @brief 解析 JSON 值的辅助函数。
 * @param json 输入 JSON 字符串。
 * @param position 当前解析位置（输入输出参数）
 * @param option 解析选项。
 * @return 解析后的 JSON 值。
 * @exception JsonParseException 如果解析失败，抛异常出异常。
 */
static JsonValue parseValue(const std::string_view& json, size_t& position, uint8_t option);

/**
 * @brief 解析空值（null）
 * @param json 输入 JSON 字符串。
 * @param position 当前解析位置（输入输出参数）
 * @return 表示空值的 JsonValue。
 * @exception JsonParseException 如果解析失败，抛出异常。
 */
static JsonValue parseNull(const std::string_view& json, size_t& position);

/**
 * @brief 解析布尔值（true 或 false）
 * @param json 输入 JSON 字符串。
 * @param position 当前解析位置（输入输出参数）
 * @return 表示布尔值的 JsonValue。
 * @exception JsonParseException 如果解析失败，抛出异常。
 */
static JsonValue parseBoolean(const std::string_view& json, size_t& position);

/**
 * @brief 解析数值（整数或浮点数）
 * @param json 输入 JSON 字符串。
 * @param position 当前解析位置（输入输出参数）
 * @return 表示数值的 JsonValue。
 * @exception JsonParseException 如果数值格式无效，抛出异常。
 */
static JsonValue parseNumber(const std::string_view& json, size_t& position);

/**
 * @brief 将hex字符串转为char32_t
 * @param hex 待转换的字符串
 * @return {unicode码点,是否转换成功}
 */
static std::pair<char32_t, bool> hexToChar32t(const std::string_view& hex);

/**
 * @brief 将Unicode码点转换为UTF-8字符串。
 * @param codePoint unicode码点。
 * @param position 当前解析到的json字符串位置。
 * @return 解析后的unicode字符串
 * @throw JsonParseException 如果码点大于0x10FFFF，抛出异常。
 */
static std::string parseUnicodeString(char32_t codePoint, size_t position);

/**
 * @brief 解析字符串。
 * @param json 输入 JSON 字符串。
 * @param position 当前解析位置（输入输出参数）
 * @param option 解析选项。
 * @return 表示字符串的 JsonValue。
 * @throw JsonParseException 如果字符串格式无效，抛出异常。
 */
static JsonValue parseString(const std::string_view& json, size_t& position, uint8_t option);

/**
 * @brief 解析数组。
 * @param json 输入 JSON 字符串。
 * @param position 当前解析位置（输入输出参数）
 * @param option 解析选项。
 * @return 表示数组的 JsonValue。
 * @exception JsonParseException 如果数组格式无效，抛出异常。
 */
static JsonValue parseArray(const std::string_view& json, size_t& position, uint8_t option);

/**
 * @brief 解析对象。
 * @param json 输入 JSON 字符串。
 * @param position 当前解析位置（输入输出参数）
 * @param option 解析选项。
 * @return 表示对象的 JsonValue。
 * @exception JsonParseException 如果对象格式无效，抛出异常。
 */
static JsonValue parseObject(const std::string_view& json, size_t& position, uint8_t option);

JsonValue parseValue(const std::string_view& json, size_t& position, uint8_t option) {
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
        default: throw JsonParseException("Unexpected character: " + std::string(c, 1), position);
    }
}

JsonValue parseNull(const std::string_view& json, size_t& position) {
    // 当前字符一定为n
    // json[pos] - json[pos + 3]
    if (json.substr(position, 4) != "null") {
        throw JsonParseException("Expected 'null'", position);
    }
    position += 4;
    return nullptr;
}

JsonValue parseBoolean(const std::string_view& json, size_t& position) {
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

JsonValue parseNumber(const std::string_view& json, size_t& position) {
    // 解析数字,数字格式为-?(0|[1-9]\d*)(\.\d+)?([eE][+-]?\d+)?
    size_t start = position;
    // 假设是整数，除非发现小数点或指数
    bool isInteger = true;

    // 处理正负号
    if (json[position] == '+' || json[position] == '-') {
        ++position;
        if (position < json.size() && json[position] == '.') {
            throw JsonParseException(
                "Invalid numeric format: sign ('+'/'-') cannot be immediately followed by '.'",
                position);
        }
    }
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
        // 跳过数字
        while (position < json.size() && std::isdigit(json[position])) {
            position++;
        }
    }
    // 处理指数
    if (position < json.size() && (json[position] == 'e' || json[position] == 'E')) {
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
    std::string raw(json.substr(start, position - start));
    // 将数值转换为数字
    if (isInteger) {
        int64_t result = 0;
        auto [ptr, ec] = std::from_chars(raw.data(), raw.data() + raw.size(), result);
        if (ec != std::errc() || ptr != raw.data() + raw.size()) {
            if (ec == std::errc::result_out_of_range) {
                goto parseDouble;
            }
            throw JsonParseException("Invalid argument: The input is not a valid integer number.",
                                     start);
        }
        return result;
    } else {
    parseDouble:
        double result  = 0;
        auto [ptr, ec] = std::from_chars(raw.data(), raw.data() + raw.size(), result);
        if (ec != std::errc() || ptr != raw.data() + raw.size()) {
            if (ec == std::errc::result_out_of_range) {
                throw JsonParseException(
                    "Result out of range: The parsed value is too large or too small.", start);
            }
            throw JsonParseException("Invalid argument: The input is not a valid float number.",
                                     start);
        }
        return result;
    }
}

std::pair<char32_t, bool> hexToChar32t(const std::string_view& hex) {
    char32_t result = 0;
    for (char c : hex) {
        result = (result << 4);
        if ('0' <= c && c <= '9') {
            result += c - '0';
        } else if ('A' <= c && c <= 'F') {
            result += c - 'A' + 10;
        } else if ('a' <= c && c <= 'f') {
            result += c - 'a' + 10;
        } else {
            return {0, false};
        }
    }
    return {result, true};
}

std::string parseUnicodeString(char32_t codePoint, size_t position) {
    std::string result;
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
    } else if (codePoint <= 0x10FFFF) {
        // 0x10000~0x10FFFF (4字节)
        result.push_back(static_cast<char>(0xF0 | (codePoint >> 18)));
        result.push_back(static_cast<char>(0x80 | ((codePoint >> 12) & 0x3F)));
        result.push_back(static_cast<char>(0x80 | ((codePoint >> 6) & 0x3F)));
        result.push_back(static_cast<char>(0x80 | (codePoint & 0x3F)));
    } else {
        throw JsonParseException("Invalid Unicode code point", position);
    }

    return result;
}

JsonValue parseString(const std::string_view& json, size_t& position, uint8_t option) {
    // 当前字符串一定为"
    // 跳过开头的"
    position++;
    JsonString result;
    // 读取后面的字符串
    while (position < json.size()) {
        char c = json[position++];
        if (c == '"') {
            return result;
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
                    if (auto [codePoint, success] = hexToChar32t(hex); success) {
                        // 检查是否是代理对（Surrogate Pair）
                        if (0xD800 <= codePoint && codePoint <= 0xDBFF) {
                            // 验证是否存在第二个代理
                            if (position + 6 > json.size() || json[position] != '\\' ||
                                json[position + 1] != 'u') {
                                throw JsonParseException("Missing low surrogate in UTF-16 pair",
                                                         position);
                            }
                            // 跳过\u
                            position += 2;
                            // 计算低代理
                            auto lowHex = json.substr(position, 4);
                            position += 4;
                            if (auto [lowCode, ok] = hexToChar32t(lowHex); ok) {
                                if (lowCode < 0xDC00 || lowCode > 0xDFFF) {
                                    throw JsonParseException("Invalid low surrogate in \\u escape",
                                                             position);
                                }
                                // 计算实际码点
                                codePoint =
                                    0x10000 + ((codePoint - 0xD800) << 10) + (lowCode - 0xDC00);
                            } else {
                                throw JsonParseException("Invalid unicode escape sequence",
                                                         position);
                            }
                        }
                        // 解析unicode编码
                        result += parseUnicodeString(codePoint, position);
                    } else {
                        throw JsonParseException("Invalid unicode escape sequence", position);
                    }
                    break;
                }
                case 'x': {
                    if (option & parser::ENABLE_PARSE_X_ESCAPE_SEQUENCE) {
                        // 收集所有连续的 \xHH 字节
                        std::vector<uint8_t> utf8Bytes;
                        while (position + 2 < json.size()) {
                            auto hex = json.substr(position, 2);
                            position += 2;
                            if (auto [byte, success] = hexToChar32t(hex); success) {
                                utf8Bytes.push_back(static_cast<uint8_t>(byte));
                            } else {
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
                        // \x必须有内容
                        if (utf8Bytes.empty() || utf8Bytes.size() > 4) {
                            throw JsonParseException("Invalid \\x escape sequence size", position);
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
                    if (option & parser::ENABLE_PARSE_0_ESCAPE_SEQUENCE) {
                        // \0 转为 \u0000
                        result += "\\u0000";
                    } else {
                        throw JsonParseException("Invalid \\0 escape sequence", position);
                    }
                    break;
                }
                default: throw JsonParseException("Invalid escape sequence", position);
            }
        } else if (static_cast<unsigned char>(c) < 0x20) {
            throw JsonParseException("Control character not allowed in JSON string", position);
        } else {
            result.push_back(c);
        }
    }
    // 异常
    throw JsonParseException("Unexpected end of string", position);
}

JsonValue parseArray(const std::string_view& json, size_t& position, uint8_t option) {
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
    // 解析后面的 value
    while (position < json.size()) {
        // 跳过无用字符
        SKIP_USELESS_CHAR(json, position);
        // 解析值
        result.emplace_back(parseValue(json, position, option));
        // 无用字符
        SKIP_USELESS_CHAR(json, position);
        if (position >= json.size()) {
            break;
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
    throw JsonParseException("Unexpected end of Array", position);
}

JsonValue parseObject(const std::string_view& json, size_t& position, uint8_t option) {
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
    while (position < json.size()) {
        SKIP_USELESS_CHAR(json, position);
        // 解析key
        if (position < json.size() && json[position] != '"') {
            throw JsonParseException("the key of object must be a string", position);
        }
        JsonValue key = parseString(json, position, option);
        SKIP_USELESS_CHAR(json, position);
        // 是否超范围,或者是否没有:
        if (position >= json.size() || json[position] != ':') {
            break;
        }
        position++;
        // 准备解析值
        JsonValue value = parseValue(json, position, option);
        if (position >= json.size()) {
            break;
        }
        object.emplace(key.asString(), value);
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
    throw JsonParseException("Unexpected end of Object", position);
}

namespace parser {
    JsonValue parse(std::string_view json, ParserOption option) {
        size_t    position = 0;
        JsonValue result   = parseValue(json, position, option);
        SKIP_USELESS_CHAR(json, position);
        // 按道理现在应该不存在json数据了
        if (position != json.size()) {
            throw JsonParseException("Unexpected content after JSON value", position);
        }
        return result;
    }
}  // namespace parser

#undef SKIP_USELESS_CHAR

/**
 * @brief 序列化 JSON 值到输出流。
 * @param value 要序列化的 JSON 值。
 * @param oss 输出字符串流。
 * @param indent 缩进空格数。
 * @param level 当前缩进层级。
 */
static void stringifyValue(const JsonValue& value, std::ostringstream& oss, int indent, int level);

/**
 * @brief 序列化空值到输出流。
 * @param oss 输出字符串流。
 */
inline static void stringifyNull(std::ostringstream& oss);

/**
 * @brief 序列化布尔值到输出流。
 * @param value JSON 值（布尔值）
 * @param oss 输出字符串流。
 */
inline static void stringifyBoolean(const JsonValue& value, std::ostringstream& oss);

/**
 * @brief 序列化整数值到输出流。
 * @param value JSON 值（整数）
 * @param oss 输出字符串流。
 * @exception JsonException 如果数值无效（如无穷大或 NaN），抛出异常。
 */
inline static void stringifyInteger(const JsonValue& value, std::ostringstream& oss);

/**
 * @brief 序列化浮点数值到输出流。
 * @param value JSON 值（浮点数）
 * @param oss 输出字符串流。
 * @exception JsonException 如果数值无效（如无穷大或 NaN），抛出异常。
 */
inline static void stringifyDouble(const JsonValue& value, std::ostringstream& oss);

/**
 * @brief 序列化字符串到输出流（处理转义字符）
 * @param value 字符串值。
 * @param oss 输出字符串流。
 */
static void stringifyString(const std::string& value, std::ostringstream& oss);

/**
 * @brief 序列化数组到输出流。
 * @param value JSON 值（数组）
 * @param oss 输出字符串流。
 * @param indent 缩进空格数。
 * @param level 当前缩进层级。
 */
static void stringifyArray(const JsonValue& value, std::ostringstream& oss, int indent, int level);

/**
 * @brief 序列化对象到输出流。
 * @param value JSON 值（对象）
 * @param oss 输出字符串流。
 * @param indent 缩进空格数。
 * @param level 当前缩进层级。
 */
static void stringifyObject(const JsonValue& value, std::ostringstream& oss, int indent, int level);

void stringifyValue(const JsonValue& value, std::ostringstream& oss, int indent, int level) {
    // 根据类型调用不同的stringify
    switch (value.type()) {
        case JsonType::Null: return stringifyNull(oss);
        case JsonType::Boolean: return stringifyBoolean(value, oss);
        case JsonType::Integer: return stringifyInteger(value, oss);
        case JsonType::Double: return stringifyDouble(value, oss);
        case JsonType::String: return stringifyString(value.asString(), oss);
        case JsonType::Array: return stringifyArray(value, oss, indent, level);
        case JsonType::Object: return stringifyObject(value, oss, indent, level);
    }
}

void stringifyNull(std::ostringstream& oss) {
    oss << "null";
}

void stringifyBoolean(const JsonValue& value, std::ostringstream& oss) {
    oss << (value.get<bool>() ? "true" : "false");
}

void stringifyInteger(const JsonValue& value, std::ostringstream& oss) {
    oss << value.get<int64_t>();
}

void stringifyDouble(const JsonValue& value, std::ostringstream& oss) {
    auto num = value.get<double>();
    if (std::isfinite(num)) {
        std::ostringstream vss;
        // 确保使用点号作为小数点
        vss.imbue(std::locale::classic());
        // 检查是否为整数值但需要表示为浮点数
        bool isIntegerValue =
            (num == std::floor(num)) && (std::abs(num) < 1e14);  // 避免大整数精度问题
        const double absValue = std::abs(num);
        // 决定使用常规表示法还是科学计数法
        bool useScientific = (absValue >= 1e6) || (absValue > 0 && absValue < 1e-4);
        if (useScientific) {
            // 科学计数法表示 - 先获取原始科学计数法字符串
            vss << std::scientific << std::setprecision(15) << num;
            std::string str = vss.str();
            // 解析科学计数法的各个部分
            size_t ePos = str.find_first_of("eE");
            if (ePos == std::string::npos) {
                // 不是科学计数法格式，直接返回
                oss << str;
                return;
            }
            // 分解为尾数和指数部分
            std::string mantissa = str.substr(0, ePos);
            std::string exponent = str.substr(ePos + 1);
            // 处理尾数部分

            // 移除尾数部分无意义的零
            size_t lastNonZero = mantissa.find_last_not_of('0');
            if (lastNonZero != std::string::npos) {
                if (mantissa[lastNonZero] == '.') {
                    mantissa.erase(lastNonZero);
                }
            }
            // 处理指数部分
            int expValue = std::stoi(exponent);

            // 重新构建科学计数法字符串
            std::ostringstream result;
            // 直接输出指数，不带+号和前导零
            result << mantissa << 'e' << expValue;

            oss << result.str();
            return;
        } else if (isIntegerValue) {
            // 对于整数值的浮点数，强制添加 .0
            vss << std::fixed << std::setprecision(1) << num;
            oss << vss.str();
            return;
        } else {
            // 常规表示法
            vss << std::setprecision(std::numeric_limits<double>::max_digits10) << num;
            oss << vss.str();
            return;
        }
    } else {
        throw JsonException("Cannot stringify infinite or NaN number");
    }
}

void stringifyString(const std::string& value, std::ostringstream& oss) {
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

void stringifyArray(const JsonValue& value, std::ostringstream& oss, int indent, int level) {
    const auto& array = value.asArray();
    if (array.empty()) {
        oss << "[]";
        return;
    }
    oss << '[';
    for (size_t i = 0; i < array.size(); i++) {
        if (i > 0) {
            oss << ',';
        }
        if (indent != 0) {
            oss << '\n';
        }
        oss << std::string((level + 1) * indent, ' ');
        stringifyValue(array[i], oss, indent, level + 1);
    }
    if (indent != 0) {
        oss << '\n' << std::string(level * indent, ' ');
    }
    oss << ']';
}

void stringifyObject(const JsonValue& value, std::ostringstream& oss, int indent, int level) {
    const auto& object = value.asObject();
    if (object.empty()) {
        oss << "{}";
        return;
    }
    oss << '{';
    bool first = true;
    for (const auto& [k, v] : object) {
        if (!first) {
            oss << ',';
        }
        if (indent != 0) {
            oss << '\n';
        }
        oss << std::string((level + 1) * indent, ' ');
        first = false;
        stringifyString(k, oss);
        oss << ':';
        stringifyValue(v, oss, indent, level + 1);
    }
    if (indent != 0) {
        oss << '\n' << std::string(level * indent, ' ');
    }
    oss << '}';
}

namespace parser {
    std::string stringify(const JsonValue& value, int indent) {
        std::ostringstream oss;
        stringifyValue(value, oss, indent, 0);
        return oss.str();
    }
}  // namespace parser

}  // namespace ccjson
#pragma clang diagnostic pop