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
#    include <variant>
#    include <vector>

namespace ccjson {

// 前向声明
class JsonValue;

// 辅助模板
/**
 * @struct is_vector
 * @brief 模板元编程工具，用于检查类型是否为 std::vector。
 *
 * 默认情况下，is_vector 继承自 std::false_type，表示类型 T 不是 std::vector。
 * @tparam T 要检查的类型。
 */
template <typename T>
struct is_vector : std::false_type {};

/**
 * @struct is_vector&lt;std::vector&lt;T&gt;&gt;
 * @brief is_vector 的特化版本，用于确认类型为 std::vector。
 *
 * 当类型 T 为 std::vector 时，继承自 std::true_type。
 * @tparam T 向量元素的类型。
 */
template <typename T>
struct is_vector<std::vector<T>> : std::true_type {};

/**
 * @struct is_string_map
 * @brief 模板元编程工具，用于检查类型是否为键为字符串的 std::map 或 std::unordered_map。
 *
 * 默认情况下，is_string_map 继承自 std::false_type，表示类型 T 不是键为字符串的映射。
 * @tparam T 要检查的类型。
 */
template <typename T>
struct is_string_map : std::false_type {};

/**
 * @struct is_string_map&lt;std::map&lt;std::string, T&gt;&gt;
 * @brief is_string_map 的特化版本，用于确认类型为 std::map&lt;std::string, T&gt;
 *
 * 当类型 T 为键为字符串的 std::map 时，继承自 std::true_type
 * @tparam T 映射值的类型。
 */
template <typename T>
struct is_string_map<std::map<std::string, T>> : std::true_type {};

/**
 * @struct is_string_map&lt;std::unordered_map&lt;std::string, T&gt;&gt;
 * @brief is_string_map 的特化版本，用于确认类型为 std::unordered_map&lt;std::string, T&gt;。
 *
 * 当类型 T 为键为字符串的 std::unordered_map 时，继承自 std::true_type。
 * @tparam T 映射值的类型。
 */
template <typename T>
struct is_string_map<std::unordered_map<std::string, T>> : std::true_type {};

/**
 * @enum JsonType
 * @brief 定义 JSON 数据支持的类型。
 *
 * 枚举了 JSON 值的所有可能类型，包括空值、布尔值、整数、浮点数、字符串、数组和对象。
 */
enum class JsonType {
    Null,     ///< 空值
    Boolean,  ///< 布尔值
    Integer,  ///< 整数
    Double,   ///< 浮点数
    String,   ///< 字符串
    Array,    ///< 数组
    Object    ///< 对象
};

/**
 * @class JsonException
 * @brief JSON 操作的通用异常类。
 *
 * 用于表示 JSON 处理过程中发生的错误，例如类型转换失败或无效值
 */
class JsonException : public std::runtime_error {
  public:
    /**
     * @brief 构造函数，创建带有错误消息的异常。
     * @param message 错误描述信息。
     */
    explicit JsonException(const std::string& message) : std::runtime_error(message) {}
};

/**
 * @class JsonParseException
 * @brief JSON 解析过程中的异常类。
 *
 * 用于表示 JSON 字符串解析失败的情况，包含错误信息和发生错误的字符位置。
 */
class JsonParseException : public std::runtime_error {
  public:
    /**
     * @brief 构造函数，创建带有错误消息和位置的异常。
     * @param message 错误描述信息。
     * @param position 解析失败的字符位置。
     */
    JsonParseException(const std::string& message, size_t position)
        : std::runtime_error(message + ", position: " + std::to_string(position)) {}
};

/**
 * @struct HasToJson
 * @brief 模板元编程工具，用于检查类型是否支持 toJson 序列化函数。
 *
 * 默认情况下，HasToJson 继承自 std::false_type，表示类型 T 不支持 toJson 函数。
 * @tparam T 要检查的类型。
 * @tparam void 辅助模板参数，用于 SFINAE。
 */
template <typename T, typename = void>
struct HasToJson : std::false_type {};

/**
 * @struct HasToJson&lt;T, std::void_t&lt;decltype(toJson(std::declval&lt;const T&&gt;()))&gt;&gt;
 * @brief HasToJson 的特化版本，检查类型是否具有有效的 toJson 函数。
 *
 * 如果类型 T 具有 toJson 函数且返回值类型为 JsonValue，则继承自 std::true_type。
 * @tparam T 要检查的类型。
 */
template <typename T>
struct HasToJson<T, std::void_t<decltype(toJson(std::declval<const T&>()))>>
    : std::is_same<decltype(toJson(std::declval<const T&>())), JsonValue> {};

/**
 * @struct HasFromJson
 * @brief 模板元编程工具，用于检查类型是否支持 fromJson 反序列化函数。
 *
 * 默认情况下，HasFromJson 继承自 std::false_type，表示类型 T 不支持 fromJson 函数。
 * @tparam T 要检查的类型。
 * @tparam void 辅助模板参数，用于 SFINAE。
 */
template <typename T, typename = void>
struct HasFromJson : std::false_type {};

/**
 * @struct HasFromJson&lt;T, std::void_t&lt;decltype(fromJson(std::declval&lt;const
 * JsonValue&&gt;(), std::declval&lt;T&>()))&gt;&gt;
 * @brief HasFromJson 的特化版本，检查类型是否具有有效的 fromJson 函数。
 *
 * 如果类型 T 具有 fromJson 函数且返回值类型为 void，则继承自 std::true_type。
 * @tparam T 要检查的类型。
 */
template <typename T>
struct HasFromJson<
    T,
    std::void_t<decltype(fromJson(std::declval<const JsonValue&>(), std::declval<T&>()))>>
    : std::is_same<decltype(fromJson(std::declval<const JsonValue&>(), std::declval<T&>())), void> {
};

/**
 * @brief JSON 字符串类型别名。
 *
 * 使用 std::string 作为 JSON 字符串的存储类型。
 */
using JsonString = std::string;
/**
 * @brief JSON 数组类型别名。
 *
 * 使用 std::vector<JsonValue> 作为 JSON 数组的存储类型。
 */
using JsonArray = std::vector<JsonValue>;
/**
 * @brief JSON 对象类型别名。
 *
 * 使用 std::map<std::string, JsonValue> 作为 JSON 对象的存储类型。
 */
using JsonObject = std::map<std::string, JsonValue>;

// 容器序列化支持
/**
 * @brief 将 std::vector 序列化为 JSON 数组。
 * @tparam T 向量元素的类型。
 * @param vec 要序列化的向量。
 * @return 表示 JSON 数组的 JsonValue 对象。
 * @note 如果元素类型支持 toJson 函数，则使用 toJson 序列化；否则直接构造 JsonValue。
 */
template <typename T>
JsonValue toJson(const std::vector<T>& vec);

/**
 * @brief 将键为字符串的映射序列化为 JSON 对象。
 * @tparam Map 映射类型（std::map 或 std::unordered_map）
 * @tparam T 映射值的类型。
 * @tparam S SFINAE 约束，确保键类型为 std::string 且 Map 为 std::map 或 std::unordered_map。
 * @param map 要序列化的映射。
 * @return 表示 JSON 对象的 JsonValue 对象。
 * @note 如果值类型支持 toJson 函数，则使用 toJson 序列化；否则直接构造 JsonValue。
 */
template <typename Map,
          typename T = typename Map::mapped_type,
          typename S = std::enable_if_t<std::is_same_v<typename Map::key_type, std::string> &&
                                        (std::is_same_v<Map, std::map<std::string, T>> ||
                                         std::is_same_v<Map, std::unordered_map<std::string, T>>)>>
JsonValue toJson(const Map& map);

/**
 * @class JsonValue
 * @brief 表示 JSON 数据的核心类，支持多种 JSON 数据类型。
 *
 * 该类封装了 JSON 的基本数据类型（空值、布尔值、整数、浮点数、字符串、数组、对象），
 * 提供构造、赋值、类型检查和数据访问等功能。支持动态类型转换和容器序列化。
 *
 * @note 该类使用联合体存储不同类型的值
 */
class JsonValue {
  public:
    /**
     * @brief 默认构造函数，初始化为空值（Null）
     */
    JsonValue() : m_type(JsonType::Null) {}

    /**
     * @brief 构造空值类型的 JSON 数据。
     * @param value 空值（std::nullptr_t）
     */
    JsonValue(std::nullptr_t) : m_type(JsonType::Null) {}

    /**
     * @brief 构造布尔类型的 JSON 数据。
     * @param value 布尔值（true 或 false）
     */
    JsonValue(bool value) : m_type(JsonType::Boolean) {
        m_value.boolean = value;
    }

    /**
     * @brief 构造整数类型的 JSON 数据。
     * @tparam T 整数类型（例如 int32_t、int64_t 等，非 bool）
     * @param value 整数值。
     */
    template <typename T,
              std::enable_if_t<std::is_integral_v<T> && !std::is_same_v<T, bool>, int> = 0>
    JsonValue(T value) : m_type(JsonType::Integer) {
        m_value.iNumber = static_cast<int64_t>(value);
    }

    /**
     * @brief 构造浮点数类型的 JSON 数据。
     * @tparam T 浮点数类型（例如 float、double）
     * @param value 浮点数值。
     */
    template <typename T, std::enable_if_t<std::is_floating_point_v<T>, int> = 0>
    JsonValue(T value) : m_type(JsonType::Double) {
        m_value.dNumber = static_cast<double>(value);
    }

    /**
     * @brief 构造字符串类型的 JSON 数据
     * @param value 字符串
     */
    /**
     * @brief 构造字符串类型的 JSON 数据 (高级 SFINAE 版本)
     * @param value 任何类似字符串的类型
     */
    template <typename T,
              // SFINAE 约束条件
              typename = std::enable_if_t<
                  // 1. 类型 T 可以转换为 std::string_view
                  std::is_convertible_v<const std::decay_t<T>&, std::string> &&
                  // 2. 并且，类型 T 不是 JsonValue (关键！避免与拷贝构造冲突)
                  !std::is_same_v<std::decay_t<T>, JsonValue>>>
    JsonValue(T&& value) : m_type(JsonType::String) {
        m_value.string = new JsonString(std::string(value));
    }

    // 同样，建议保留移动构造函数以优化性能
    JsonValue(std::string&& value) : m_type(JsonType::String) {
        m_value.string = new JsonString(std::move(value));
    }

    /**
     * @brief 构造数组类型的 JSON 数据。
     * @param value JSON 数组对象。
     */
    JsonValue(const JsonArray& value) : m_type(JsonType::Array) {
        m_value.array = new JsonArray(value);
    }

    /**
     * @brief 构造对象类型的 JSON 数据。
     * @param value JSON 对象（键值对映射）
     */
    JsonValue(const JsonObject& value) : m_type(JsonType::Object) {
        m_value.object = new JsonObject(value);
    }

    /**
     * @brief 构造数组类型的 JSON 数据（std::vector）
     * @tparam T 向量元素的类型。
     * @param vec 向量对象。
     */
    template <typename T>
    JsonValue(const std::vector<T>& vec) : m_type(JsonType::Array) {
        *this = toJson(vec);
    }

    /**
     * @brief 构造对象类型的 JSON 数据（std::map）
     * @tparam T 映射值的类型。
     * @param map 映射对象。
     */
    template <typename T>
    JsonValue(const std::map<std::string, T>& map) : m_type(JsonType::Object) {
        *this = toJson(map);
    }

    /**
     * @brief 构造对象类型的 JSON 数据（std::unordered_map）
     * @tparam T 映射值的类型。
     * @param map 无序映射对象。
     */
    template <typename T>
    JsonValue(const std::unordered_map<std::string, T>& map) : m_type(JsonType::Object) {
        *this = toJson(map);
    }

    // 通用模板构造函数，确保KeyType可以转换为std::string
    template <typename Key, std::enable_if_t<std::is_convertible_v<Key, std::string>, int> = 0>
    JsonValue(const std::pair<Key, JsonValue>& pair) : m_type(JsonType::Array) {
        m_value.array = new JsonArray;
        m_value.array->emplace_back(static_cast<std::string>(pair.first));
        m_value.array->emplace_back(pair.second);
    }

    /**
     * @brief 使用初始化列表构造对象类型的 JSON 数据。
     * @tparam T 值类型。
     * @param init 初始化列表，包含键值对（const char*, T）
     */
    template <typename T>
    JsonValue(std::initializer_list<std::pair<const char*, T>> init) : m_type(JsonType::Object) {
        m_value.object = new JsonObject();
        for (const auto& [k, v] : init) {
            (*m_value.object)[k] = JsonValue(v);
        }
    }

    /**
     * @brief 使用初始化列表构造对象类型的 JSON 数据（JsonValue 值）
     * @param init 初始化列表，包含键值对（const char*, JsonValue）
     */
    JsonValue(std::initializer_list<std::pair<const char*, JsonValue>> init)
        : m_type(JsonType::Object) {
        m_value.object = new JsonObject();
        for (const auto& [k, v] : init) {
            (*m_value.object)[k] = v;
        }
    }

    /**
     * @brief 使用初始化列表构造对象类型的 JSON 数据（通用类型，转换为键值对）
     * @tparam T 初始化列表元素类型（需可转换为 std::pair&lt;const char*, JsonValue&gt;）
     * @param init 初始化列表。
     */
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

    /**
     * @brief 使用初始化列表构造数组类型的 JSON 数据。
     * @param init 初始化列表，包含非键值对的元素。
     */
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

    /**
     * @brief 拷贝构造函数。
     */
    JsonValue(const JsonValue& other);

    /**
     * @brief 移动构造函数。
     * @param other JsonValue 对象（右值）
     */
    JsonValue(JsonValue&& other) noexcept;

    /**
     * @brief 拷贝赋值运算符。
     * @param other 要赋值的 JsonValue 对象。
     * @return 自身引用。
     */
    JsonValue& operator=(const JsonValue& other);

    /**
     * @brief 移动赋值运算符。
     * @param other JsonValue 对象（右值）
     * @return 自身引用。
     */
    JsonValue& operator=(JsonValue&& other) noexcept;

    /**
     * @brief 析构函数，释放内部资源。
     */
    ~JsonValue();

    /**
     * @brief 获取 JSON 数据类型。
     * @return 当前 JSON 值的类型（JsonType）
     */
    inline JsonType type() const noexcept {
        return m_type;
    }

    /**
     * @brief 检查是否为空值（Null）
     * @return 如果是空值，返回 true，否则返回 false。
     */
    inline bool isNull() const noexcept {
        return m_type == JsonType::Null;
    }

    /**
     * @brief 检查是否为布尔类型。
     * @return 如果是布尔值，返回 true，否则返回 false。
     */
    inline bool isBoolean() const noexcept {
        return m_type == JsonType::Boolean;
    }

    /**
     * @brief 检查是否为数值类型（整数或浮点数）
     * @return 如果是数值类型，返回 true，否则返回 false。
     */
    inline bool isNumber() const noexcept {
        return m_type == JsonType::Integer || m_type == JsonType::Double;
    }

    /**
     * @brief 检查是否为字符串类型。
     * @return 如果是字符串，返回 true，否则返回 false。
     */
    inline bool isString() const noexcept {
        return m_type == JsonType::String;
    }

    /**
     * @brief 检查是否为数组类型。
     * @return 如果是数组，返回 true，否则返回 false。
     */
    inline bool isArray() const noexcept {
        return m_type == JsonType::Array;
    }

    /**
     * @brief 检查是否为对象类型。
     * @return 如果是对象，返回 true，否则，返回 false。
     */
    inline bool isObject() const noexcept {
        return m_type == JsonType::Object;
    }

    /**
     * @brief 使用初始化列表为数组赋值。
     * @param init 初始化列表，包含 JsonValue 元素。
     * @return 自身引用。
     */
    JsonValue& operator=(std::initializer_list<JsonValue> init) {
        destroyValue();
        m_type        = JsonType::Array;
        m_value.array = new JsonArray(init);
        return *this;
    }

    /**
     * @brief 使用初始化列表为对象赋值（键值对）
     * @tparam T 初始化列表元素类型（需可转换为 std::pair&lt;const char*, JsonValue&gt;）
     * @param init 初始化列表，包含键值对。
     * @return 自身引用。
     */
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

    /**
     * @brief 使用初始化列表为数组赋值（非键值对）
     * @tparam T 初始化列表元素类型（非需不可转换为 std::pair&lt;const char*, JsonValue&gt;）
     * @param init 初始化列表，包含数组元素。
     * @return 自身引用。
     */
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

    /**
     * @brief 为向量赋值（std::vector）
     * @param vec T 向量对象。
     * @return 自身引用。
     */
    template <typename T>
    JsonValue& operator=(const std::vector<T>& vec) {
        *this = toJson(vec);
        return *this;
    }

    /**
     * @brief 为映射赋值（std::map）
     * @param T 值类型。
     * @param map 映射对象。
     * @return 自身引用。
     */
    template <typename T>
    JsonValue& operator=(const std::map<std::string, T>& map) {
        *this = toJson(map);
        return *this;
    }

    /**
     * @brief 为无序映射赋值（std::unordered_map）
     * @param T 值类型。
     * @param map 无序映射对象。
     * @return 自身引用。
     */
    template <typename T>
    JsonValue& operator=(const std::unordered_map<std::string, T>& map) {
        *this = toJson(map);
        return *this;
    }

    /**
     * @brief 为自定义类型赋值（支持 toJson）
     * @param T 类型（需实现 toJson 函数）
     * @param value 值。
     * @return 自身引用。
     */
    template <typename T, typename std::enable_if<HasToJson<T>::value, int>::type = 0>
    JsonValue(const T& value) : m_type() {
        *this = toJson(value);
    }

    /**
     * @brief 构造自定义类型的 JsonValue（支持 toJson）
     * @param T 类型（需实现 toJson 函数）
     * @param value 值。
     */
    template <typename T, typename std::enable_if<HasToJson<T>::value, int>::type = 0>
    JsonValue& operator=(const T& value) {
        *this = toJson(value);
        return *this;
    }

    /**
     * @brief 创建空对象。
     * @return JsonValue 表示空对象的 JsonValue。
     */
    static JsonValue Object() {
        return JsonObject();
    }

    /**
     * @brief 创建空数组。
     * @return 表示空数组的 JsonValue。
     */
    static JsonValue Array() {
        return JsonArray();
    }

    /**
     * @brief 设置对象的键值对。
     * @param key 键（字符串）
     * @param value 值（JsonValue）
     * @return 自身引用。
     */
    JsonValue& set(const std::string& key, const JsonValue& value);

    /**
     * @brief 向数组添加元素。
     * @param value 要添加的 JsonValue。
     * @return 自身引用。
     */
    JsonValue& push_back(JsonValue value);

    /**
     * @brief 转换为布尔值。
     * @exception JsonException 如果不是布尔类型，则抛出异常。
     * @return 布尔值。
     */
    operator bool() const;

    /**
     * @brief 转换为 16 位整数。
     * @exception JsonException 如果不是数值类型，则抛出异常。
     * @return 16 位整数值。
     */
    operator int16_t() const;

    /**
     * @brief 转换为 32 位整数。
     * @exception JsonException 如果不是数值类型，则抛出异常。
     * @return 32 位整数值。
     */
    operator int32_t() const;

    /**
     * @brief 转换为 64 位整数。
     * @exception JsonException 如果不是数值类型，则抛出异常。
     * @return 64 位整数值。
     */
    operator int64_t() const;

    /**
     * @brief 转换为浮点数（float）
     * @exception JsonException 如果不是数值类型，则抛出异常。
     * @return 浮点数值（float）
     */
    operator float() const;

    /**
     * @brief 转换为浮点数（double）
     * @exception JsonException 如果不是数值类型，则抛出异常。
     * @return 浮点数值（double）
     */
    operator double() const;

    /**
     * @brief 转换为字符串。
     * @exception JsonException 如果不是字符串类型，则抛出异常。
     * @return 字符串值。
     */
    operator std::string() const;

    /**
     * @brief 获取指定类型的值。
     * @param T 目标类型。
     * @return 指定类型 T 的值。
     * @exception JsonException 或 std::runtime_error 如果类型不匹配或转换失败。
     * @note 支持基本类型、JsonObject、JsonArray 和自定义序列化类型（须实现fromJson函数）
     */
    template <typename T>
    T get() const {
        if constexpr (std::is_same_v<T, JsonValue>) {
            return *this;
        } else if constexpr (std::is_same_v<T, JsonObject>) {
            if (!isObject()) {
                throw JsonException("Not an Object");
            }
            return *m_value.object;
        } else if constexpr (std::is_same_v<T, JsonArray>) {
            if (!isArray()) {
                throw JsonException("Not an Array");
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

    /**
     * @brief 访问数组元素（可读写）
     * @param T 数组索引（整数）
     * @param key 参数类型（整数）
     * @return 数组中对应索引的 JsonValue 引用。
     * @exception std::out_of_range 如果索引为负数。
     */
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
        throw JsonException("Negative Array index");
    }

    /**
     * @brief 访问对象属性（可读写）
     * @param T 键类型（字符串或可转换为字符串）
     * @param key 参数类型 T。
     * @return 对象中对应键的 JsonValue 引用。
     */
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

    /**
     * @brief 访问数组元素（只读读）
     * @param T 数组索引（整数类型）
     * @param key 参数类型 T。
     * @return 数组中对应索引的 JsonValue 常量引用值。
     * @exception std::out_of范围 如果索引超出范围。
     * @exception std::runtime_error 如果不是数组。
     */
    template <typename T, std::enable_if_t<std::is_integral_v<std::remove_reference_t<T>>, int> = 0>
    const JsonValue& operator[](T&& key) const {
        if (!isArray()) {
            throw JsonException("Not an Array");
        }
        auto& arr = *m_value.array;
        if (key >= 0 && static_cast<size_t>(key) < arr.size()) {
            return arr[key];
        }
        throw JsonException("Array index out of range");
    }

    /**
     * @brief 访问对象属性（只读）
     * @param key 参数类型 T 键类型（字符串或 C 风格字符串）
     * @return 对象中对应键的 JsonValue 常量引用值。
     * @exception std::out_of范围 如果键不存在。
     * @exception std::runtime_error 如果不是对象。
     */
    template <typename T,
              std::enable_if_t<
                  !std::is_integral_v<std::remove_reference_t<T>> &&
                      (std::is_convertible_v<T, std::string> ||
                       std::is_same_v<std::remove_cv_t<std::remove_reference_t<T>>, const char*>),
                  int> = 0>
    const JsonValue& operator[](T&& key) const {
        if (!isObject()) {
            throw JsonException("Not an Object");
        }
        auto it = m_value.object->find(std::forward<T>(key));
        if (it != m_value.object->end()) {
            return it->second;
        }
        throw JsonException("Key not found");
    }

    /**
     * @brief 将 JSON 值转换为字符串表示。
     * @param indent 缩进空格数（默认 0，表示无缩进）
     * @return JSON 值的字符串表示。
     */
    std::string toString(int indent = 0) const;

    /**
     * @brief 输出 JSON 值到流。
     * @param os 输出流。
     * @param value JSON 值。
     * @return 输出流引用。
     */
    friend std::ostream& operator<<(std::ostream& os, const JsonValue& value) {
        os << value.toString();
        return os;
    }

  private:
    class ConstIterator {
      public:
        using iterator_catalog = std::forward_iterator_tag;
        using value_type       = const JsonValue;
        using difference_type  = std::ptrdiff_t;
        using reference        = value_type&;
        using pointer          = value_type*;

        // Pre-increment operator
        inline ConstIterator& operator++() {
            std::visit([](auto& it) { ++it; }, m_iterator);
            return *this;
        }

        // Post-increment operator
        inline ConstIterator operator++(int) {
            auto temp = *this;
            ++(*this);
            return temp;
        }

        // Equality operator
        inline bool operator==(const ConstIterator& other) const {
            return m_host == other.m_host && m_iterator == other.m_iterator;
        }

        // Inequality operator
        inline bool operator!=(const ConstIterator& other) const {
            return !(*this == other);
        }

        // Dereference operator
        reference operator*() const;

        // Member access operator
        inline pointer operator->() const {
            return &this->operator*();
        }

        std::string key() const;

        inline reference value() const {
            return operator*();
        }

      private:
        using IteratorType =
            std::variant<JsonArray::const_iterator, JsonObject ::const_iterator, size_t>;

        ConstIterator(const JsonValue* host, IteratorType iterator)
            : m_host(host), m_iterator(iterator) {}

      private:
        const JsonValue* m_host;
        IteratorType     m_iterator;

        friend class JsonValue;
    };

  public:
    ConstIterator begin() const {
        switch (m_type) {
            case JsonType::Object: return {this, m_value.object->begin()};
            case JsonType::Array: return {this, m_value.array->begin()};
            default:
                // For simple types, begin is represented by index 0
                return ConstIterator(this, size_t{0});
        }
    }

    ConstIterator end() const {
        switch (m_type) {
            case JsonType::Object: return {this, m_value.object->end()};
            case JsonType::Array: return {this, m_value.array->end()};
            default:
                // For simple types, end is represented by index 1
                return ConstIterator(this, size_t{1});
        }
    }

  private:
    /**
     * @brief 释放内部存储的动态内存。
     */
    void destroyValue() noexcept;

  private:
    JsonType m_type;  ///< JSON 数据类型
    union
    {
        bool                              boolean;  ///< 布尔值
        int64_t                           iNumber;  ///< 整数值
        double                            dNumber;  ///< 浮点值
        std::string*                      string;   ///< 字符串指针
        std::vector<JsonValue>*           array;    ///< 数组指针
        std::map<std::string, JsonValue>* object;   ///< 对象指针
    } m_value{};                                    ///< 存储值的联合体
};  // namespace ccjson

// JSON解析器类
class JsonParser {
  public:
    /**
     * @enum ParserOption
     * @brief JSON 解析选项枚举。
     *
     * 定义解析 JSON 时的可选配置项，用于控制扩展功能。
     */
    enum ParserOption {
        DISABLE_EXTENSION              = 0,      ///< 禁用所有扩展
        ENABLE_PARSE_X_ESCAPE_SEQUENCE = 1,      ///< 启用 \x 转义序列解析
        ENABLE_PARSE_0_ESCAPE_SEQUENCE = 1 << 1  ///< 启用 \0 转义序列解析
    };

  public:
    /**
     * @brief 从 JSON 字符串解析为 JsonValue。
     * @param json JSON 输入字符串。
     * @param option 解析选项（默认禁用扩展）
     * @return 解析结果的 JsonValue 对象。
     * @exception JsonParseException 如果解析失败，抛出异常，包含错误信息和位置。
     */
    static JsonValue parse(std::string_view json, uint8_t option = DISABLE_EXTENSION);

    /**
     * @brief 将 JsonValue 序列化为 JSON 字符串。
     * @param value 要序列化的 JSON 值。
     * @param indent 缩进空格数（默认 0，表示无缩进）
     * @return 序列化的 JSON 字符串。
     * @exception JsonException 如果序列化失败（如数值无效），抛出异常。
     */
    static std::string stringify(const JsonValue& value, int indent = 0);

  private:
    /**
     * @brief 解析 JSON 值的辅助函数。
     * @param json 输入 JSON 字符串。
     * @param position 当前解析位置（输入输出参数）
     * @param option 解析选项。
     * @return 解析后的 JSON 值。
     * @exception JsonParseException 如果解析失败，抛异常出异常。
     */
    static JsonValue parseValue(std::string_view json, size_t& position, uint8_t option);

    /**
     * @brief 解析空值（null）
     * @param json 输入 JSON 字符串。
     * @param position 当前解析位置（输入输出参数）
     * @return 表示空值的 JsonValue。
     * @exception JsonParseException 如果解析失败，抛出异常。
     */
    static JsonValue parseNull(std::string_view json, size_t& position);

    /**
     * @brief 解析布尔值（true 或 false）
     * @param json 输入 JSON 字符串。
     * @param position 当前解析位置（输入输出参数）
     * @return 表示布尔值的 JsonValue。
     * @exception JsonParseException 如果解析失败，抛出异常。
     */
    static JsonValue parseBoolean(std::string_view json, size_t& position);

    /**
     * @brief 解析数值（整数或浮点数）
     * @param json 输入 JSON 字符串。
     * @param position 当前解析位置（输入输出参数）
     * @return 表示数值的 JsonValue。
     * @exception JsonParseException 如果数值格式无效，抛出异常。
     */
    static JsonValue parseNumber(std::string_view json, size_t& position);

    /**
     * @brief 解析字符串。
     * @param json 输入 JSON 字符串。
     * @param position 当前解析位置（输入输出参数）
     * @param option 解析选项。
     * @return 表示字符串的 JsonValue。
     * @exception JsonParseException 如果字符串格式无效，抛出异常。
     */
    static JsonValue parseString(std::string_view json, size_t& position, uint8_t option);

    /**
     * @brief 解析数组。
     * @param json 输入 JSON 字符串。
     * @param position 当前解析位置（输入输出参数）
     * @param option 解析选项。
     * @return 表示数组的 JsonValue。
     * @exception JsonParseException 如果数组格式无效，抛出异常。
     */
    static JsonValue parseArray(std::string_view json, size_t& position, uint8_t option);

    /**
     * @brief 解析对象。
     * @param json 输入 JSON 字符串。
     * @param position 当前解析位置（输入输出参数）
     * @param option 解析选项。
     * @return 表示对象的 JsonValue。
     * @exception JsonParseException 如果对象格式无效，抛出异常。
     */
    static JsonValue parseObject(std::string_view json, size_t& position, uint8_t option);

  private:
    /**
     * @brief 序列化 JSON 值到输出流。
     * @param value 要序列化的 JSON 值。
     * @param oss 输出字符串流。
     * @param indent 缩进空格数。
     * @param level 当前缩进层级。
     */
    static void
    stringifyValue(const JsonValue& value, std::ostringstream& oss, int indent, int level);

    /**
     * @brief 序列化空值到输出流。
     * @param value JSON 值（空值）
     * @param oss 输出字符串流。
     * @param indent 缩进空格数。
     * @param level 当前缩进层级。
     */
    inline static void
    stringifyNull(const JsonValue& value, std::ostringstream& oss, int indent, int level);

    /**
     * @brief 序列化布尔值到输出流。
     * @param value JSON 值（布尔值）
     * @param oss 输出字符串流。
     * @param indent 缩进空格数。
     * @param level 当前缩进层级。
     */
    inline static void
    stringifyBoolean(const JsonValue& value, std::ostringstream& oss, int indent, int level);

    /**
     * @brief 序列化整数值到输出流。
     * @param value JSON 值（整数）
     * @param oss 输出字符串流。
     * @param indent 缩进空格数。
     * @param level 当前缩进层级。
     * @exception JsonException 如果数值无效（如无穷大或 NaN），抛出异常。
     */
    inline static void
    stringifyInteger(const JsonValue& value, std::ostringstream& oss, int indent, int level);

    /**
     * @brief 序列化浮点数值到输出流。
     * @param value JSON 值（浮点数）
     * @param oss 输出字符串流。
     * @param indent 缩进空格数。
     * @param level 当前缩进层级。
     * @exception JsonException 如果数值无效（如无穷大或 NaN），抛出异常。
     */
    inline static void
    stringifyDouble(const JsonValue& value, std::ostringstream& oss, int indent, int level);

    /**
     * @brief 序列化字符串值到输出流。
     * @param value JSON 值（字符串）
     * @param oss 输出字符串流。
     * @param indent 缩进空格数。
     * @param level 当前缩进层级。
     */
    inline static void
    stringifyString(const JsonValue& value, std::ostringstream& oss, int indent, int level);

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
    static void
    stringifyArray(const JsonValue& value, std::ostringstream& oss, int indent, int level);

    /**
     * @brief 序列化对象到输出流。
     * @param value JSON 值（对象）
     * @param oss 输出字符串流。
     * @param indent 缩进空格数。
     * @param level 当前缩进层级。
     */
    static void
    stringifyObject(const JsonValue& value, std::ostringstream& oss, int indent, int level);
};

// 容器序列化支持

/**
 * @brief 将 std::vector 序列化为 JSON 数组。
 * @tparam T 向量元素类型。
 * @param vec 要序列化的向量。
 * @return 表示 JSON 数组的 JsonValue。
 */
template <typename T>
JsonValue toJson(const std::vector<T>& vec) {
    std::vector<JsonValue> result;
    result.reserve(vec.size());
    for (const auto& item : vec) {
        if constexpr (HasToJson<T>::value) {
            result.emplace_back(toJson(item));
        } else {
            result.emplace_back(item);
        }
    }
    return result;
}

/**
 * @brief 将键为字符串的映射序列化为 JSON 对象。
 * @tparam Map 映射类型（std::map 或 std::unordered_map）
 * @tparam T 映射值类型。
 * @param map 要序列化的映射。
 * @return 表示 JSON 对象的 JsonValue。
 */
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

/**
 * @brief 将字符串字面量转为JsonValue
 * @param data 字符串指针
 * @param length 字符串长度
 * @return 解析后的JsonValue
 * @note 解析调用JsonParser::parse且不支持任何扩展
 */
inline JsonValue operator""_json(const char* data, size_t length) {
    return JsonParser::parse({data, length});
}
}  // namespace ccjson

#endif
#pragma clang diagnostic pop
