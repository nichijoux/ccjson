#pragma clang diagnostic push
#pragma ide diagnostic ignored "modernize-use-nodiscard"
#pragma ide diagnostic ignored "google-explicit-constructor"

#ifndef CCJSON_JSON_H

#    define CCJSON_JSON_H

#    include <map>
#    include <memory>
#    include <optional>
#    include <ostream>
#    include <stdexcept>
#    include <string>
#    include <type_traits>
#    include <unordered_map>
#    include <vector>

namespace ccjson {

// 前向声明
class JsonValue;

// 辅助模板
template <typename T>
struct is_vector : std::false_type {};

template <typename T>
struct is_vector<std::vector<T>> : std::true_type {};

template <typename T>
struct is_string_map : std::false_type {};

template <typename T>
struct is_string_map<std::map<std::string, T>> : std::true_type {};

template <typename T>
struct is_string_map<std::unordered_map<std::string, T>> : std::true_type {};

// JSON值类型
enum class JsonType { Null, Boolean, Integer, Double, String, Array, Object };

// JSON解析错误异常
class JsonException : public std::runtime_error {
  public:
    explicit JsonException(const std::string& message) : std::runtime_error(message) {}
};

template <typename T, typename = void>
struct HasToJson : std::false_type {};

// 先检查是否有toJson，然后检测toJson的返回值是否为JsonValue
template <typename T>
struct HasToJson<T, std::void_t<decltype(toJson(std::declval<const T&>()))>>
    : std::is_same<decltype(toJson(std::declval<const T&>())), JsonValue> {};

template <typename T, typename = void>
struct HasFromJson : std::false_type {};

// 先检查是否有fromJson，然后检测fromJson的返回值是否为void
template <typename T>
struct HasFromJson<
    T,
    std::void_t<decltype(fromJson(std::declval<const JsonValue&>(), std::declval<T&>()))>>
    : std::is_same<decltype(fromJson(std::declval<const JsonValue&>(), std::declval<T&>())), void> {
};

class JsonParseException : public std::runtime_error {
  public:
    JsonParseException(const std::string& message, size_t position)
        : std::runtime_error(message + ", position: " + std::to_string(position)) {}
};

using JsonString = std::string;
using JsonArray  = std::vector<JsonValue>;
using JsonObject = std::map<std::string, JsonValue>;

// 容器序列化支持
template <typename T>
JsonValue toJson(const std::vector<T>& vec);

template <typename Map,
          typename T = typename Map::mapped_type,
          typename S = std::enable_if_t<std::is_same_v<typename Map::key_type, std::string> &&
                                        (std::is_same_v<Map, std::map<std::string, T>> ||
                                         std::is_same_v<Map, std::unordered_map<std::string, T>>)>>
JsonValue toJson(const Map& map);

// JSON值类
class JsonValue {
  public:
    // 构造函数
    JsonValue() : m_type(JsonType::Null) {}

    JsonValue(std::nullptr_t) : m_type(JsonType::Null) {}

    JsonValue(bool value) : m_type(JsonType::Boolean) {
        m_value.boolean = value;
    }

    template <typename T,
              std::enable_if_t<std::is_integral_v<T> && !std::is_same_v<T, bool>, int> = 0>
    JsonValue(T value) : m_type(JsonType::Integer) {
        m_value.iNumber = static_cast<int64_t>(value);
    }

    template <typename T, std::enable_if_t<std::is_floating_point_v<T>, int> = 0>
    JsonValue(T value) : m_type(JsonType::Double) {
        m_value.dNumber = static_cast<double>(value);
    }

    JsonValue(const char* value) : m_type(JsonType::String) {
        m_value.string = new JsonString(value);
    }

    JsonValue(const std::string& value) : m_type(JsonType::String) {
        m_value.string = new JsonString(value);
    }

    JsonValue(std::string&& value) : m_type(JsonType::String) {
        m_value.string = new JsonString(std::move(value));
    }

    JsonValue(const JsonArray& value) : m_type(JsonType::Array) {
        m_value.array = new JsonArray(value);
    }

    JsonValue(JsonArray&& value) : m_type(JsonType::Array) {
        m_value.array = new JsonArray(std::move(value));
    }

    JsonValue(const JsonObject& value) : m_type(JsonType::Object) {
        m_value.object = new JsonObject(value);
    }

    JsonValue(JsonObject&& value) : m_type(JsonType::Object) {
        m_value.object = new JsonObject(std::move(value));
    }

    JsonValue(const std::unordered_map<std::string, JsonValue>& value) : m_type(JsonType::Object) {
        m_value.object = new JsonObject(value.begin(), value.end());
    }

    JsonValue(std::unordered_map<std::string, JsonValue>&& value) : m_type(JsonType::Object) {
        m_value.object = new JsonObject(value.begin(), value.end());
    }

    // 支持容器类型的构造函数
    template <typename T>
    JsonValue(const std::vector<T>& vec) : m_type(JsonType::Array) {
        *this = toJson(vec);
    }

    template <typename T>
    JsonValue(const std::map<std::string, T>& map) : m_type(JsonType::Object) {
        *this = toJson(map);
    }

    template <typename T>
    JsonValue(const std::unordered_map<std::string, T>& map) : m_type(JsonType::Object) {
        *this = toJson(map);
    }

    // 支持pair的初始化列表（用于对象初始化）
    template <typename T>
    JsonValue(std::initializer_list<std::pair<const char*, T>> init) : m_type(JsonType::Object) {
        m_value.object = new JsonObject();
        for (const auto& [k, v] : init) {
            (*m_value.object)[k] = JsonValue(v);
        }
    }

    // 支持混合类型的pair初始化列表
    JsonValue(std::initializer_list<std::pair<const char*, JsonValue>> init)
        : m_type(JsonType::Object) {
        m_value.object = new JsonObject();
        for (const auto& [k, v] : init) {
            (*m_value.object)[k] = v;
        }
    }

    template <
        typename T,
        std::enable_if_t<std::is_convertible_v<T, std::pair<const char*, JsonValue>>, int> = 0>
    JsonValue(std::initializer_list<T> init) {
        // 对象初始化
        m_type         = JsonType::Object;
        m_value.object = new JsonObject();
        for (const auto& item : init) {
            auto pair                     = static_cast<std::pair<const char*, JsonValue>>(item);
            (*m_value.object)[pair.first] = pair.second;
        }
    }

    template <
        typename T,
        std::enable_if_t<!std::is_convertible_v<T, std::pair<const char*, JsonValue>>, int> = 0>
    JsonValue(std::initializer_list<T> init) {
        // 数组初始化
        m_type        = JsonType::Array;
        m_value.array = new JsonArray();
        m_value.array->reserve(init.size());
        for (const auto& item : init) {
            m_value.array->emplace_back(JsonValue(item));
        }
    }

    // 拷贝构造函数
    JsonValue(const JsonValue& other);

    // 移动构造函数
    JsonValue(JsonValue&& other) noexcept;

    // 拷贝赋值运算符
    JsonValue& operator=(const JsonValue& other);

    // 移动赋值运算符
    JsonValue& operator=(JsonValue&& other) noexcept;

    // 析构函数
    ~JsonValue() {
        destroyValue();
    }

    // 类型检查
    JsonType type() const noexcept {
        return m_type;
    }

    bool isNull() const noexcept {
        return m_type == JsonType::Null;
    }

    bool isBoolean() const noexcept {
        return m_type == JsonType::Boolean;
    }

    bool isNumber() const noexcept {
        return m_type == JsonType::Integer || m_type == JsonType::Double;
    }

    bool isString() const noexcept {
        return m_type == JsonType::String;
    }

    bool isArray() const noexcept {
        return m_type == JsonType::Array;
    }

    bool isObject() const noexcept {
        return m_type == JsonType::Object;
    }

    // 数组赋值运算符
    JsonValue& operator=(std::initializer_list<JsonValue> init) {
        destroyValue();
        m_type        = JsonType::Array;
        m_value.array = new JsonArray(init);
        return *this;
    }

    // 模板数组赋值运算符
    template <
        typename T,
        std::enable_if_t<std::is_convertible_v<T, std::pair<const char*, JsonValue>>, int> = 0>
    JsonValue& operator=(std::initializer_list<T> init) {
        // 释放之前的数据
        destroyValue();
        // 对象赋值
        m_type         = JsonType::Object;
        m_value.object = new JsonObject();
        for (const auto& [k, v] : init) {
            (*m_value.object)[k] = v;
        }
        return *this;
    }

    template <
        typename T,
        std::enable_if_t<!std::is_convertible_v<T, std::pair<const char*, JsonValue>>, int> = 0>
    JsonValue& operator=(std::initializer_list<T> init) {
        // 释放之前的数据
        destroyValue();
        // 数组赋值
        m_type        = JsonType::Array;
        m_value.array = new JsonArray();
        m_value.array->reserve(init.size());
        for (const auto& item : init) {
            m_value.array->emplace_back(JsonValue(item));
        }
        return *this;
    }

    // 支持容器类型的赋值运算符
    template <typename T>
    JsonValue& operator=(const std::vector<T>& vec) {
        *this = toJson(vec);
        return *this;
    }

    template <typename T>
    JsonValue& operator=(const std::map<std::string, T>& map) {
        *this = toJson(map);
        return *this;
    }

    template <typename T>
    JsonValue& operator=(const std::unordered_map<std::string, T>& map) {
        *this = toJson(map);
        return *this;
    }

    // 支持自定义类型的构造函数和赋值运算符
    template <typename T, typename std::enable_if<HasToJson<T>::value, int>::type = 0>
    JsonValue(const T& value) : m_type() {
        *this = toJson(value);
    }

    template <typename T, typename std::enable_if<HasToJson<T>::value, int>::type = 0>
    JsonValue& operator=(const T& value) {
        *this = toJson(value);
        return *this;
    }

    // 创建对象的静态方法
    static JsonValue Object() {
        return JsonObject();
    }

    static JsonValue Array() {
        return JsonArray();
    }

    // 设置键值对
    JsonValue& set(const std::string& key, const JsonValue& value) {
        if (m_type != JsonType::Object) {
            destroyValue();
            m_type         = JsonType::Object;
            m_value.object = new JsonObject();
        }
        (*m_value.object)[key] = value;
        return *this;
    }

    // 添加数组元素
    JsonValue& push_back(JsonValue value) {
        if (!isArray()) {
            destroyValue();
            m_type        = JsonType::Array;
            m_value.array = new JsonArray();
        }
        m_value.array->emplace_back(std::move(value));
        return *this;
    }

    // Implicit conversion operators
    operator bool() const {
        if (!isBoolean()) {
            throw JsonException("Cannot convert to bool");
        }
        return m_value.boolean;
    }

    operator int16_t() const {
        if (!isNumber()) {
            throw JsonException("Cannot convert to int");
        }
        if (m_type == JsonType::Integer) {
            return static_cast<int16_t>(m_value.iNumber);
        }
        return static_cast<int16_t>(m_value.dNumber);
    }

    operator int32_t() const {
        if (!isNumber()) {
            throw JsonException("Cannot convert to int");
        }
        if (m_type == JsonType::Integer) {
            return static_cast<int32_t>(m_value.iNumber);
        }
        return static_cast<int32_t>(m_value.dNumber);
    }

    operator int64_t() const {
        if (!isNumber()) {
            throw JsonException("Cannot convert to int");
        }
        if (m_type == JsonType::Integer) {
            return m_value.iNumber;
        }
        return static_cast<int64_t>(m_value.dNumber);
    }

    operator float() const {
        if (!isNumber()) {
            throw JsonException("Cannot convert to double");
        }
        if (m_type == JsonType::Double) {
            return static_cast<float>(m_value.dNumber);
        }
        return static_cast<float>(m_value.iNumber);
    }

    operator double() const {
        if (!isNumber()) {
            throw JsonException("Cannot convert to double");
        }
        if (m_type == JsonType::Double) {
            return m_value.dNumber;
        }
        return static_cast<double>(m_value.iNumber);
    }

    operator std::string() const {
        if (!isString()) {
            throw JsonException("Cannot convert to string");
        }
        return *m_value.string;
    }

    // 类型转换和访问
    template <typename T>
    T get() const {
        if constexpr (std::is_same_v<T, JsonValue>) {
            return *this;
        } else if constexpr (std::is_same_v<T, JsonObject>) {
            if (!isObject()) {
                throw std::runtime_error("Not an Object");
            }
            return *m_value.object;
        } else if constexpr (std::is_same_v<T, JsonArray>) {
            if (!isArray()) {
                throw std::runtime_error("Not an Array");
            }
            return *m_value.array;
        } else if constexpr (std::is_arithmetic_v<T>) {
            // 支持所有数值类型之间的转换
            if (!(isNumber() || isBoolean())) {
                throw JsonException("Cannot convert to numeric type");
            }
            if (m_type == JsonType::Integer) {
                return static_cast<T>(m_value.iNumber);
            } else if (m_type == JsonType::Boolean) {
                return m_value.boolean;
            } else {
                return static_cast<T>(m_value.dNumber);
            }
        } else if constexpr (std::is_same_v<T, std::string>) {
            return operator std::string();
        } else if constexpr (is_vector<T>::value) {
            // 处理 std::vector<T> 类型
            if (!isArray()) {
                throw JsonException("Not an Array");
            }
            using ValueType = typename T::value_type;
            T result;
            result.reserve(m_value.array->size());
            for (const auto& item : *m_value.array) {
                if constexpr (HasFromJson<ValueType>::value) {
                    ValueType value;
                    fromJson(item, value);
                    result.emplace_back(std::move(value));
                } else {
                    result.emplace_back(item.template get<ValueType>());
                }
            }
            return result;
        } else if constexpr (is_string_map<T>::value) {
            if (!isObject()) {
                throw JsonException("Not a Object");
            }
            using ValueType = typename T::mapped_type;
            T result;
            for (const auto& [key, item] : *m_value.object) {
                if constexpr (HasToJson<ValueType>::value) {
                    ValueType value;
                    fromJson(item, value);
                    result[key] = value;
                } else {
                    // 没有定义fromJson函数
                    result[key] = item.template get<ValueType>();
                }
            }
            return result;
        } else if constexpr (HasFromJson<T>::value) {
            // 处理自定义类型
            T result;
            fromJson(*this, result);
            return result;
        } else {
            // 这行永远不会执行，只是为了避免编译器警告
            static_assert(std::is_void_v<T>, "Unsupported type for get<T>()");
            return T{};
        }
    }

    template <typename T, std::enable_if_t<std::is_integral_v<std::remove_reference_t<T>>, int> = 0>
    JsonValue& operator[](T&& key) {
        // key为数字
        if (!isArray()) {
            destroyValue();
            m_type        = JsonType::Array;
            m_value.array = new JsonArray();
        }
        auto& arr = *m_value.array;
        if (key >= 0) {
            if (static_cast<size_t>(key) >= arr.size()) {
                arr.resize(key + 1);
            }
            return arr[key];
        }
        throw std::out_of_range("Negative Array index");
    }

    template <typename T,
              std::enable_if_t<
                  !std::is_integral_v<std::remove_reference_t<T>> &&
                      (std::is_convertible_v<T, std::string> ||
                       std::is_same_v<std::remove_cv_t<std::remove_reference_t<T>>, const char*>),
                  int> = 0>
    JsonValue& operator[](T&& key) {
        if (!isObject()) {
            destroyValue();
            m_type         = JsonType::Object;
            m_value.object = new JsonObject;
        }
        return (*m_value.object)[std::forward<T>(key)];
    }

    template <typename T, std::enable_if_t<std::is_integral_v<std::remove_reference_t<T>>, int> = 0>
    const JsonValue& operator[](T&& key) const {
        if (!isArray()) {
            throw std::runtime_error("Not an Array");
        }
        auto& arr = *m_value.array;
        if (key >= 0 && static_cast<size_t>(key) < arr.size()) {
            return arr[key];
        }
        throw std::out_of_range("Array index out of range");
    }

    template <typename T,
              std::enable_if_t<
                  !std::is_integral_v<std::remove_reference_t<T>> &&
                      (std::is_convertible_v<T, std::string> ||
                       std::is_same_v<std::remove_cv_t<std::remove_reference_t<T>>, const char*>),
                  int> = 0>
    const JsonValue& operator[](T&& key) const {
        if (!isObject()) {
            throw std::runtime_error("Not an Object");
        }
        auto it = m_value.object->find(std::forward<T>(key));
        if (it != m_value.object->end()) {
            return it->second;
        }
        throw std::out_of_range("Key not found");
    }

    std::string toString(int indent = 0) const;

    friend std::ostream& operator<<(std::ostream& os, const JsonValue& value) {
        os << value.toString();
        return os;
    }

  private:
    void destroyValue();

  private:
    JsonType m_type;
    union
    {
        bool                              boolean;
        int64_t                           iNumber;
        double                            dNumber;
        std::string*                      string;
        std::vector<JsonValue>*           array;
        std::map<std::string, JsonValue>* object;
    } m_value{};
};

// JSON解析器类
class JsonParser {
  public:
    enum ParserOption {
        DISABLE_EXTENSION              = 0,
        ENABLE_PARSE_X_ESCAPE_SEQUENCE = 1,
        ENABLE_PARSE_0_ESCAPE_SEQUENCE = 1 << 1
    };

  public:
    // 从字符串解析JSON
    static JsonValue parse(std::string_view json, uint8_t option = DISABLE_EXTENSION);

    // 将JSON值转换为字符串
    static std::string stringify(const JsonValue& value, int indent = 0);

  private:
    // 解析辅助函数
    static JsonValue parseValue(std::string_view json, size_t& position, uint8_t option);
    static JsonValue parseNull(std::string_view json, size_t& position);
    static JsonValue parseBoolean(std::string_view json, size_t& position);
    static JsonValue parseNumber(std::string_view json, size_t& position);
    static JsonValue parseString(std::string_view json, size_t& position, uint8_t option);
    static JsonValue parseArray(std::string_view json, size_t& position, uint8_t option);
    static JsonValue parseObject(std::string_view json, size_t& position, uint8_t option);

    // 字符串化辅助函数
    static void
    stringifyValue(const JsonValue& value, std::ostringstream& oss, int indent, int level);
    inline static void
    stringifyNull(const JsonValue& value, std::ostringstream& oss, int indent, int level);
    inline static void
    stringifyBoolean(const JsonValue& value, std::ostringstream& oss, int indent, int level);
    static void
    stringifyInteger(const JsonValue& value, std::ostringstream& oss, int indent, int level);
    static void
    stringifyDouble(const JsonValue& value, std::ostringstream& oss, int indent, int level);
    inline static void
    stringifyString(const JsonValue& value, std::ostringstream& oss, int indent, int level);
    static void stringifyString(const std::string& value, std::ostringstream& oss);
    static void
    stringifyArray(const JsonValue& value, std::ostringstream& oss, int indent, int level);
    static void
    stringifyObject(const JsonValue& value, std::ostringstream& oss, int indent, int level);
};

// 容器序列化支持
template <typename T>
JsonValue toJson(const std::vector<T>& vec) {
    std::vector<JsonValue> result;
    result.reserve(vec.size());
    for (const auto& item : vec) {
        if constexpr (HasToJson<T>::value) {
            result.push_back(toJson(item));
        } else {
            result.push_back(JsonValue(item));
        }
    }
    return result;
}

template <typename Map, typename T, typename>
JsonValue toJson(const Map& map) {
    std::map<std::string, JsonValue> result;
    for (const auto& [key, value] : map) {
        if constexpr (HasToJson<T>::value) {
            result[key] = toJson(value);
        } else {
            result[key] = JsonValue(value);
        }
    }
    return result;
}
}  // namespace ccjson

#endif
#pragma clang diagnostic pop
