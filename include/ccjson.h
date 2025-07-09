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
    using std::runtime_error::runtime_error;
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
 * @brief JSON 对象反序列化为 std::vector
 * @tparam T 向量元素的类型
 * @param root 要反序列化的 JSON 对象
 * @param vec 反序列化后的 std::vector 对象
 * @note 如果 T 支持 fromJson函数，则使用 fromJson 进行反序列化；否则直接构造 JsonValue
 */
template <typename T>
void fromJson(const JsonValue& root, std::vector<T>& vec);

/**
 * @brief 将 JSON 对象反序列化为键为字符串的映射
 * @tparam Map 映射类型（std::map 或 std::unordered_map）
 * @tparam T 映射值的类型
 * @tparam S SFINAE 约束，确保键类型为 std::string 且 Map 为 std::map 或 std::unordered_map
 * @param root 要反序列化的 JSON 对象
 * @param map 反序列化后的映射对象
 * @note 如果值类型支持 fromJson 函数，则使用 fromJson 反序列化；否则直接构造 JsonValue
 */
template <typename Map,
          typename T = typename Map::mapped_type,
          typename S = std::enable_if_t<std::is_same_v<typename Map::key_type, std::string> &&
                                        (std::is_same_v<Map, std::map<std::string, T>> ||
                                         std::is_same_v<Map, std::unordered_map<std::string, T>>)>>
void fromJson(const JsonValue& root, Map& map);

/**
 * @brief 将 std::vector 序列化为 JSON 对象
 * @tparam T 向量元素的类型
 * @param vec 要序列化的向量
 * @return 表示 JSON 数组的 JsonValue 对象
 * @note 如果元素类型支持 toJson 函数，则使用 toJson 序列化；否则直接构造 JsonValue
 */
template <typename T>
JsonValue toJson(const std::vector<T>& vec);

/**
 * @brief 将键为字符串的映射序列化为 JSON 对象
 * @tparam Map 映射类型（std::map 或 std::unordered_map）
 * @tparam T 映射值的类型
 * @tparam S SFINAE 约束，确保键类型为 std::string 且 Map 为 std::map 或 std::unordered_map
 * @param map 要序列化的映射
 * @return 表示 JSON 对象的 JsonValue 对象
 * @note 如果值类型支持 toJson 函数，则使用 toJson 序列化；否则直接构造 JsonValue
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
    JsonValue() noexcept : m_type(JsonType::Null) {}

    /**
     * @brief 构造空值类型的 JSON 数据。
     * @param value 空值（std::nullptr_t）
     */
    JsonValue(std::nullptr_t) noexcept : m_type(JsonType::Null) {}

    /**
     * @brief 构造布尔类型的 JSON 数据。
     * @param value 布尔值（true 或 false）
     */
    JsonValue(bool value) noexcept : m_type(JsonType::Boolean) {
        m_value.boolean = value;
    }

    /**
     * @brief 构造整数类型的 JSON 数据。
     * @tparam T 整数类型（例如 int32_t、int64_t 等，非 bool）
     * @param value 整数值。
     */
    template <typename T,
              std::enable_if_t<std::is_integral_v<T> && !std::is_same_v<T, bool>, int> = 0>
    JsonValue(T value) noexcept : m_type(JsonType::Integer) {
        m_value.iNumber = static_cast<int64_t>(value);
    }

    /**
     * @brief 构造浮点数类型的 JSON 数据。
     * @tparam T 浮点数类型（例如 float、double）
     * @param value 浮点数值。
     */
    template <typename T, std::enable_if_t<std::is_floating_point_v<T>, int> = 0>
    JsonValue(T value) noexcept : m_type(JsonType::Double) {
        m_value.dNumber = static_cast<double>(value);
    }

    /**
     * @brief 构造字符串类型的 JSON 数据 (高级 SFINAE 版本)
     * @param value 任何类似字符串的类型
     */
    template <typename T,
              // SFINAE 约束条件
              typename = std::enable_if_t<
                  // 1. 类型 T 可以转换为 std::string_view
                  std::is_convertible_v<const std::decay_t<T>&, std::string_view> &&
                  // 2. 并且，类型 T 不是 JsonValue
                  !std::is_same_v<std::decay_t<T>, JsonValue>>>
    JsonValue(T&& value) noexcept : m_type(JsonType::String) {
        m_value.string = new JsonString(value);
    }

    JsonValue(std::string&& value) noexcept : m_type(JsonType::String) {
        m_value.string = new JsonString(std::move(value));
    }

    /**
     * @brief 构造数组类型的 JSON 数据。
     * @param value JSON 数组对象。
     */
    JsonValue(const JsonArray& value) noexcept : m_type(JsonType::Array) {
        m_value.array = new JsonArray(value);
    }

    /**
     * @brief 构造对象类型的 JSON 数据。
     * @param value JSON 对象（键值对映射）
     */
    JsonValue(const JsonObject& value) noexcept : m_type(JsonType::Object) {
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
     * @brief 检查是否为对象类型
     * @return 如果是对象，返回 true，否则，返回 false
     */
    inline bool isObject() const noexcept {
        return m_type == JsonType::Object;
    }

    /**
     * @brief 获取字符串值的引用
     * @return 字符串值的常量引用
     * @exception JsonException 如果当前类型不是字符串，抛出异常
     */
    inline const JsonString& asString() const {
        if (m_type != JsonType::String) {
            throw JsonException("not a string");
        }
        return *m_value.string;
    }

    /**
     * @brief 获取数组值的引用
     * @return 数组值的常量引用
     * @exception JsonException 如果当前类型不是数组，抛出异常
     */
    inline const JsonArray& asArray() const {
        if (m_type != JsonType::Array) {
            throw JsonException("not an array");
        }
        return *m_value.array;
    }

    /**
     * @brief 获取对象值的引用
     * @return 对象值的常量引用
     * @exception JsonException 如果当前类型不是对象，抛出异常
     */
    inline const JsonObject& asObject() const {
        if (m_type != JsonType::Object) {
            throw JsonException("not an object");
        }
        return *m_value.object;
    }

    /**
     * @brief 使用初始化列表为数组赋值
     * @param init 初始化列表，包含 JsonValue 元素
     * @return 自身引用
     * @note 释放现有数据并构造新的 JSON 数组
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
     * @tparam T 向量元素的类型
     * @param vec 向量对象
     * @return 自身引用
     * @note 内部调用 toJson 将向量序列化为 JSON 数组
     */
    template <typename T>
    JsonValue& operator=(const std::vector<T>& vec) {
        *this = toJson(vec);
        return *this;
    }

    /**
     * @brief 为映射赋值（std::map）
     * @tparam T 映射值的类型
     * @param map 映射对象
     * @return 自身引用
     * @note 内部调用 toJson 将映射序列化为 JSON 对象
     */
    template <typename T>
    JsonValue& operator=(const std::map<std::string, T>& map) {
        *this = toJson(map);
        return *this;
    }

    /**
     * @brief 为无序映射赋值（std::unordered_map）
     * @tparam T 映射值的类型
     * @param map 无序映射对象
     * @return 自身引用
     * @note 内部调用 toJson 将无序映射序列化为 JSON 对象
     */
    template <typename T>
    JsonValue& operator=(const std::unordered_map<std::string, T>& map) {
        *this = toJson(map);
        return *this;
    }

    /**
     * @brief 构造自定义类型的 JSON 数据（支持 toJson）
     * @tparam T 类型（需实现 toJson 函数）
     * @param value 值
     * @note 使用 SFINAE 确保 T 支持 toJson 函数
     */
    template <typename T, typename std::enable_if<HasToJson<T>::value, int>::type = 0>
    JsonValue(const T& value) : m_type() {
        *this = toJson(value);
    }

    /**
     * @brief 为自定义类型赋值（支持 toJson）
     * @tparam T 类型（需实现 toJson 函数）
     * @param value 值
     * @return 自身引用
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
     * @brief 设置对象的键值对
     * @param key 键（字符串）
     * @param value 值（JsonValue）
     * @return 自身引用
     * @note 如果当前对象不是 JSON 对象，会自动转换为对象类型
     */
    JsonValue& set(const std::string& key, const JsonValue& value);

    /**
     * @brief 向数组添加元素
     * @param value 要添加的 JsonValue
     * @return 自身引用
     * @note 如果当前对象不是 JSON 数组，会自动转换为数组类型
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
        } else if constexpr (std::is_same_v<T, std::string> ||
                             std::is_same_v<T, std::string_view>) {
            return operator std::string();
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
    /**
     * @class ConstIterator
     * @brief JsonValue 的常量迭代器，用于遍历数组或对象元素。
     *
     * 支持前向迭代，提供对 JSON 数组或对象的只读访问。
     * @note 使用 std::variant 存储不同类型的迭代器（数组、对象或简单类型）。
     */
    class ConstIterator {
      public:
        using iterator_category = std::forward_iterator_tag;  ///< 迭代器类别
        using value_type        = const JsonValue;            ///< 迭代器值类型
        using difference_type   = std::ptrdiff_t;             ///< 差值类型
        using reference         = value_type&;                ///< 引用类型
        using pointer           = value_type*;                ///< 指针类型

        /**
         * @brief 前置递增运算符
         * @return 自身引用
         * @note 递增内部迭代器（数组、对象或简单类型的计数）
         */
        inline ConstIterator& operator++() {
            std::visit([](auto& it) { ++it; }, m_iterator);
            return *this;
        }

        /**
         * @brief 后置递增运算符
         * @return 递增前的迭代器副本
         */
        inline ConstIterator operator++(int) {
            auto temp = *this;
            ++(*this);
            return temp;
        }

        /**
         * @brief 相等比较运算符
         * @param other 另一个迭代器
         * @return 如果迭代器相等，返回 true；否则返回 false
         */
        inline bool operator==(const ConstIterator& other) const {
            return m_host == other.m_host && m_iterator == other.m_iterator;
        }

        /**
         * @brief 不等比较运算符
         * @param other 另一个迭代器
         * @return 如果迭代器不相等，返回 true；否则返回 false
         */
        inline bool operator!=(const ConstIterator& other) const {
            return !(*this == other);
        }

        /**
         * @brief 解引用运算符
         * @return 当前迭代器指向的 JsonValue 引用
         */
        reference operator*() const;

        /**
         * @brief 指针访问运算符
         * @return 当前迭代器指向的 JsonValue 指针
         */
        inline pointer operator->() const {
            return &this->operator*();
        }

        /**
         * @brief 获取当前对象的键（仅对对象类型有效）
         * @return 当前对象的键（字符串）
         * @exception JsonException 如果当前类型不是对象，抛出异常
         */
        std::string key() const;

        /**
         * @brief 获取当前迭代器指向的值
         * @return 当前迭代器指向的 JsonValue 引用
         */
        inline reference value() const {
            return operator*();
        }

      private:
        using IteratorType = std::variant<JsonArray::const_iterator,
                                          JsonObject ::const_iterator,
                                          size_t>;  ///< 迭代器类型

        /**
         * @brief 构造函数，初始化迭代器
         * @param host 所属的 JsonValue 对象
         * @param iterator 内部迭代器（数组、对象或简单类型计数）
         */
        ConstIterator(const JsonValue* host, IteratorType iterator)
            : m_host(host), m_iterator(iterator) {}

      private:
        const JsonValue* m_host;      ///< 所属 JsonValue 对象
        IteratorType     m_iterator;  ///< 内部迭代器

        friend class JsonValue;
    };

  public:
    /**
     * @brief 获取迭代器起始位置
     * @return 指向数组、对象或简单类型起始位置的常量迭代器
     * @note 对于简单类型，begin 返回计数 0
     */
    ConstIterator begin() const noexcept {
        switch (m_type) {
            case JsonType::Object: return {this, m_value.object->begin()};
            case JsonType::Array: return {this, m_value.array->begin()};
            default:
                // 对于简单类型，begin即为0
                return ConstIterator(this, size_t{0});
        }
    }

    /**
     * @brief 获取迭代器结束位置
     * @return 指向数组、对象或简单类型结束位置的常量迭代器
     * @note 对于简单类型，end 返回计数 1
     */
    ConstIterator end() const noexcept {
        switch (m_type) {
            case JsonType::Object: return {this, m_value.object->end()};
            case JsonType::Array: return {this, m_value.array->end()};
            default:
                // 对于简单类型，end即为1
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

/**
 * @class JsonParser
 * @brief JSON 字符串解析和序列化的工具类。
 *
 * 提供静态方法用于将 JSON 字符串解析为 JsonValue 对象，以及将 JsonValue 对象序列化为 JSON 字符串。
 * 支持扩展解析选项（如 \\x 和 \\0 转义序列）。
 */
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
     * @brief 解析字符串。
     * @param json 输入 JSON 字符串。
     * @param position 当前解析位置（输入输出参数）
     * @param option 解析选项。
     * @return 表示字符串的 JsonValue。
     * @exception JsonParseException 如果字符串格式无效，抛出异常。
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

template <typename T>
void fromJson(const JsonValue& root, std::vector<T>& vec) {
    if (!root.isArray()) {
        throw JsonException("Not an Array");
    }
    using ValueType = typename std::vector<T>::value_type;
    vec.clear();
    vec.reserve(root.asArray().size());
    for (const auto& item : root.asArray()) {
        if constexpr (HasFromJson<ValueType>::value) {
            ValueType value;
            fromJson(item, value);
            vec.emplace_back(std::move(value));
        } else {
            vec.emplace_back(item.template get<ValueType>());
        }
    }
}

template <typename Map, typename T, typename>
void fromJson(const JsonValue& root, Map& map) {
    if (!root.isObject()) {
        throw JsonException("Not a Object");
    }
    using ValueType = typename Map::mapped_type;
    map.clear();
    for (const auto& [key, item] : root.asObject()) {
        if constexpr (HasToJson<ValueType>::value) {
            ValueType value;
            fromJson(item, value);
            map[key] = value;
        } else {
            // 没有定义fromJson函数
            map[key] = item.template get<ValueType>();
        }
    }
}

template <typename T>
JsonValue toJson(const std::vector<T>& vec) {
    JsonArray result;
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

template <typename Map, typename T, typename>
JsonValue toJson(const Map& map) {
    JsonObject result;
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
