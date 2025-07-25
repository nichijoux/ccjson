#ifndef CCJSON_JSON_REFLECT_H
#define CCJSON_JSON_REFLECT_H

#include "ccjson.h"
#include <string>

namespace ccjson::reflect {

// BEGIN GENERATED BY foreachgen.py

/**
 * @defgroup REFLECT_PP_FOREACH 反射宏定义组
 * @brief 用于在编译期对可变参数列表进行迭代处理的宏。
 *
 * 这些宏由 `foreachgen.py` 自动生成，支持从 1 到 32 个参数的迭代处理。
 * 每个宏将给定的函数 `f` 应用于参数列表中的每个参数。
 *
 * @note 这些宏用于支持反射机制，简化结构体成员的序列化与反序列化。
 */
#define REFLECT_PP_FOREACH_1(f, _1) f(_1)
#define REFLECT_PP_FOREACH_2(f, _1, _2) f(_1) f(_2)
#define REFLECT_PP_FOREACH_3(f, _1, _2, _3) f(_1) f(_2) f(_3)
#define REFLECT_PP_FOREACH_4(f, _1, _2, _3, _4) f(_1) f(_2) f(_3) f(_4)
#define REFLECT_PP_FOREACH_5(f, _1, _2, _3, _4, _5) f(_1) f(_2) f(_3) f(_4) f(_5)
#define REFLECT_PP_FOREACH_6(f, _1, _2, _3, _4, _5, _6) f(_1) f(_2) f(_3) f(_4) f(_5) f(_6)
#define REFLECT_PP_FOREACH_7(f, _1, _2, _3, _4, _5, _6, _7)                                        \
    f(_1) f(_2) f(_3) f(_4) f(_5) f(_6) f(_7)
#define REFLECT_PP_FOREACH_8(f, _1, _2, _3, _4, _5, _6, _7, _8)                                    \
    f(_1) f(_2) f(_3) f(_4) f(_5) f(_6) f(_7) f(_8)
#define REFLECT_PP_FOREACH_9(f, _1, _2, _3, _4, _5, _6, _7, _8, _9)                                \
    f(_1) f(_2) f(_3) f(_4) f(_5) f(_6) f(_7) f(_8) f(_9)
#define REFLECT_PP_FOREACH_10(f, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10)                          \
    f(_1) f(_2) f(_3) f(_4) f(_5) f(_6) f(_7) f(_8) f(_9) f(_10)
#define REFLECT_PP_FOREACH_11(f, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11)                     \
    f(_1) f(_2) f(_3) f(_4) f(_5) f(_6) f(_7) f(_8) f(_9) f(_10) f(_11)
#define REFLECT_PP_FOREACH_12(f, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12)                \
    f(_1) f(_2) f(_3) f(_4) f(_5) f(_6) f(_7) f(_8) f(_9) f(_10) f(_11) f(_12)
#define REFLECT_PP_FOREACH_13(f, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13)           \
    f(_1) f(_2) f(_3) f(_4) f(_5) f(_6) f(_7) f(_8) f(_9) f(_10) f(_11) f(_12) f(_13)
#define REFLECT_PP_FOREACH_14(f, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14)      \
    f(_1) f(_2) f(_3) f(_4) f(_5) f(_6) f(_7) f(_8) f(_9) f(_10) f(_11) f(_12) f(_13) f(_14)
#define REFLECT_PP_FOREACH_15(f, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15) \
    f(_1) f(_2) f(_3) f(_4) f(_5) f(_6) f(_7) f(_8) f(_9) f(_10) f(_11) f(_12) f(_13) f(_14) f(_15)
#define REFLECT_PP_FOREACH_16(f, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, \
                              _16)                                                                 \
    f(_1) f(_2) f(_3) f(_4) f(_5) f(_6) f(_7) f(_8) f(_9) f(_10) f(_11) f(_12) f(_13) f(_14)       \
        f(_15) f(_16)
#define REFLECT_PP_FOREACH_17(f, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, \
                              _16, _17)                                                            \
    f(_1) f(_2) f(_3) f(_4) f(_5) f(_6) f(_7) f(_8) f(_9) f(_10) f(_11) f(_12) f(_13) f(_14)       \
        f(_15) f(_16) f(_17)
#define REFLECT_PP_FOREACH_18(f, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, \
                              _16, _17, _18)                                                       \
    f(_1) f(_2) f(_3) f(_4) f(_5) f(_6) f(_7) f(_8) f(_9) f(_10) f(_11) f(_12) f(_13) f(_14)       \
        f(_15) f(_16) f(_17) f(_18)
#define REFLECT_PP_FOREACH_19(f, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, \
                              _16, _17, _18, _19)                                                  \
    f(_1) f(_2) f(_3) f(_4) f(_5) f(_6) f(_7) f(_8) f(_9) f(_10) f(_11) f(_12) f(_13) f(_14)       \
        f(_15) f(_16) f(_17) f(_18) f(_19)
#define REFLECT_PP_FOREACH_20(f, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, \
                              _16, _17, _18, _19, _20)                                             \
    f(_1) f(_2) f(_3) f(_4) f(_5) f(_6) f(_7) f(_8) f(_9) f(_10) f(_11) f(_12) f(_13) f(_14)       \
        f(_15) f(_16) f(_17) f(_18) f(_19) f(_20)
#define REFLECT_PP_FOREACH_21(f, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, \
                              _16, _17, _18, _19, _20, _21)                                        \
    f(_1) f(_2) f(_3) f(_4) f(_5) f(_6) f(_7) f(_8) f(_9) f(_10) f(_11) f(_12) f(_13) f(_14)       \
        f(_15) f(_16) f(_17) f(_18) f(_19) f(_20) f(_21)
#define REFLECT_PP_FOREACH_22(f, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, \
                              _16, _17, _18, _19, _20, _21, _22)                                   \
    f(_1) f(_2) f(_3) f(_4) f(_5) f(_6) f(_7) f(_8) f(_9) f(_10) f(_11) f(_12) f(_13) f(_14)       \
        f(_15) f(_16) f(_17) f(_18) f(_19) f(_20) f(_21) f(_22)
#define REFLECT_PP_FOREACH_23(f, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, \
                              _16, _17, _18, _19, _20, _21, _22, _23)                              \
    f(_1) f(_2) f(_3) f(_4) f(_5) f(_6) f(_7) f(_8) f(_9) f(_10) f(_11) f(_12) f(_13) f(_14)       \
        f(_15) f(_16) f(_17) f(_18) f(_19) f(_20) f(_21) f(_22) f(_23)
#define REFLECT_PP_FOREACH_24(f, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, \
                              _16, _17, _18, _19, _20, _21, _22, _23, _24)                         \
    f(_1) f(_2) f(_3) f(_4) f(_5) f(_6) f(_7) f(_8) f(_9) f(_10) f(_11) f(_12) f(_13) f(_14)       \
        f(_15) f(_16) f(_17) f(_18) f(_19) f(_20) f(_21) f(_22) f(_23) f(_24)
#define REFLECT_PP_FOREACH_25(f, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, \
                              _16, _17, _18, _19, _20, _21, _22, _23, _24, _25)                    \
    f(_1) f(_2) f(_3) f(_4) f(_5) f(_6) f(_7) f(_8) f(_9) f(_10) f(_11) f(_12) f(_13) f(_14)       \
        f(_15) f(_16) f(_17) f(_18) f(_19) f(_20) f(_21) f(_22) f(_23) f(_24) f(_25)
#define REFLECT_PP_FOREACH_26(f, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, \
                              _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26)               \
    f(_1) f(_2) f(_3) f(_4) f(_5) f(_6) f(_7) f(_8) f(_9) f(_10) f(_11) f(_12) f(_13) f(_14)       \
        f(_15) f(_16) f(_17) f(_18) f(_19) f(_20) f(_21) f(_22) f(_23) f(_24) f(_25) f(_26)
#define REFLECT_PP_FOREACH_27(f, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, \
                              _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27)          \
    f(_1) f(_2) f(_3) f(_4) f(_5) f(_6) f(_7) f(_8) f(_9) f(_10) f(_11) f(_12) f(_13) f(_14)       \
        f(_15) f(_16) f(_17) f(_18) f(_19) f(_20) f(_21) f(_22) f(_23) f(_24) f(_25) f(_26) f(_27)
#define REFLECT_PP_FOREACH_28(f, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, \
                              _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28)     \
    f(_1) f(_2) f(_3) f(_4) f(_5) f(_6) f(_7) f(_8) f(_9) f(_10) f(_11) f(_12) f(_13) f(_14)       \
        f(_15) f(_16) f(_17) f(_18) f(_19) f(_20) f(_21) f(_22) f(_23) f(_24) f(_25) f(_26) f(_27) \
            f(_28)
#define REFLECT_PP_FOREACH_29(f, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, \
                              _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28,     \
                              _29)                                                                 \
    f(_1) f(_2) f(_3) f(_4) f(_5) f(_6) f(_7) f(_8) f(_9) f(_10) f(_11) f(_12) f(_13) f(_14)       \
        f(_15) f(_16) f(_17) f(_18) f(_19) f(_20) f(_21) f(_22) f(_23) f(_24) f(_25) f(_26) f(_27) \
            f(_28) f(_29)
#define REFLECT_PP_FOREACH_30(f, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, \
                              _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28,     \
                              _29, _30)                                                            \
    f(_1) f(_2) f(_3) f(_4) f(_5) f(_6) f(_7) f(_8) f(_9) f(_10) f(_11) f(_12) f(_13) f(_14)       \
        f(_15) f(_16) f(_17) f(_18) f(_19) f(_20) f(_21) f(_22) f(_23) f(_24) f(_25) f(_26) f(_27) \
            f(_28) f(_29) f(_30)
#define REFLECT_PP_FOREACH_31(f, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, \
                              _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28,     \
                              _29, _30, _31)                                                       \
    f(_1) f(_2) f(_3) f(_4) f(_5) f(_6) f(_7) f(_8) f(_9) f(_10) f(_11) f(_12) f(_13) f(_14)       \
        f(_15) f(_16) f(_17) f(_18) f(_19) f(_20) f(_21) f(_22) f(_23) f(_24) f(_25) f(_26) f(_27) \
            f(_28) f(_29) f(_30) f(_31)
#define REFLECT_PP_FOREACH_32(f, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, \
                              _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28,     \
                              _29, _30, _31, _32)                                                  \
    f(_1) f(_2) f(_3) f(_4) f(_5) f(_6) f(_7) f(_8) f(_9) f(_10) f(_11) f(_12) f(_13) f(_14)       \
        f(_15) f(_16) f(_17) f(_18) f(_19) f(_20) f(_21) f(_22) f(_23) f(_24) f(_25) f(_26) f(_27) \
            f(_28) f(_29) f(_30) f(_31) f(_32)

/**
 * @brief 计算可变参数数量的辅助宏。
 *
 * 通过参数计数实现，从 32 到 1，确定传入参数的数量。
 */
#define REFLECT_PP_NARGS_IMPL(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15,    \
                              _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28,     \
                              _29, _30, _31, _32, N, ...)                                          \
    N
/**
 * @brief 计算可变参数数量的宏。
 *
 * 调用 REFLECT_PP_NARGS_IMPL 确定传入参数的数量。
 */
#define REFLECT_PP_NARGS(...)                                                                      \
    REFLECT_PP_NARGS_IMPL(__VA_ARGS__, 32, 31, 30, 29, 28, 27, 26, 25, 24, 23, 22, 21, 20, 19, 18, \
                          17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1)
// END GENERATED BY foreachgen.py

/**
 * @brief 展开宏参数。
 *
 * 用于确保宏参数在预处理器中正确展开。
 */
#define REFLECT_EXPAND(...) __VA_ARGS__

/**
 * @brief 连接两个标识符的辅助宏。
 */
#define REFLECT_CONCAT_2(x, y) x##y

/**
 * @brief 连接两个标识符。
 *
 * 调用 REFLECT_CONCAT_2 完成标识符连接。
 */
#define REFLECT_CONCAT(x, y) REFLECT_CONCAT_2(x, y)

/**
 * @brief 对可变参数列表应用指定函数。
 *
 * 根据参数数量选择合适的 REFLECT_PP_FOREACH_N 宏，依次对每个参数应用函数 f。
 * @param function 要应用的函数。
 * @param ... 可变参数列表。
 */
#define REFLECT_PP_FOREACH(function, ...)                                                          \
    REFLECT_EXPAND(REFLECT_CONCAT(REFLECT_PP_FOREACH_, REFLECT_PP_NARGS(__VA_ARGS__)))             \
    (function, __VA_ARGS__)

/**
 * @enum MemberType
 * @brief 定义类成员的类型。
 *
 * 枚举了类成员的可能类型，包括成员变量、成员函数、静态变量和静态函数。
 */
enum class MemberType {
    MEMBER_VARIABLE,  ///< 成员变量
    STATIC_VARIABLE,  ///< 静态变量
    MEMBER_FUNCTION,  ///< 成员函数
    STATIC_FUNCTION   ///< 静态函数
};

/**
 * @struct GetMemberType
 * @brief 模板元编程工具，用于确定类成员的类型。
 *
 * 默认模板，用于非特化情况，推导成员类型。
 * @tparam T 要检查的成员类型。
 */
template <typename T>
struct GetMemberType;

/**
 * @struct GetMemberType<T C::*>
 * @brief GetMemberType 特化，用于成员变量指针。
 *
 * 确定成员变量指针的类型为 MEMBER_VARIABLE。
 * @tparam T 成员变量的类型。
 * @tparam C 所属类的类型。
 */
template <typename T, typename C>
struct GetMemberType<T C::*> {
    static constexpr MemberType value = MemberType::MEMBER_VARIABLE;
};

/**
 * @struct GetMemberType<T*>
 * @brief GetMemberType 特化，用于静态变量指针。
 *
 * 确定静态变量指针的类型为 STATIC_VARIABLE。
 * @tparam T 静态变量的类型。
 */
template <typename T>
struct GetMemberType<T*> {
    static constexpr MemberType value = MemberType::STATIC_VARIABLE;
};

/**
 * @struct GetMemberType<T (C::*)(Ts...)>
 * @brief GetMemberType 特化，用于非 const 成员函数指针。
 *
 * 确定成员函数指针的类型为 MEMBER_FUNCTION。
 * @tparam T 返回类型。
 * @tparam C 所属类的类型。
 * @tparam Ts 函数参数类型。
 */
template <typename T, typename C, typename... Ts>
struct GetMemberType<T (C::*)(Ts...)> {
    static constexpr MemberType value = MemberType::MEMBER_FUNCTION;
};

/**
 * @struct GetMemberType<T (C::*)(Ts...) const>
 * @brief GetMemberType 特化，用于 const 成员函数指针。
 *
 * 确定 const 成员函数指针的类型为 MEMBER_FUNCTION。
 * @tparam T 返回类型。
 * @tparam C 所属类的类型。
 * @tparam Ts 函数参数类型。
 */
template <typename T, typename C, typename... Ts>
struct GetMemberType<T (C::*)(Ts...) const> {
    static constexpr MemberType value = MemberType::MEMBER_FUNCTION;
};

/**
 * @struct GetMemberType<T (C::*)(Ts...)&>
 * @brief GetMemberType 特化，用于左值引用成员函数指针。
 *
 * 确定左值引用成员函数指针的类型为 MEMBER_FUNCTION。
 * @tparam T 返回类型。
 * @tparam C 所属类的类型。
 * @tparam Ts 函数参数类型。
 */
template <typename T, typename C, typename... Ts>
struct GetMemberType<T (C::*)(Ts...)&> {
    static constexpr MemberType value = MemberType::MEMBER_FUNCTION;
};

/**
 * @struct GetMemberType<T (C::*)(Ts...) const&>
 * @brief GetMemberType 特化，用于 const 左值引用成员函数指针。
 *
 * 确定 const 左值引用成员函数指针的类型为 MEMBER_FUNCTION。
 * @tparam T 返回类型。
 * @tparam C 所属类的类型。
 * @tparam Ts 函数参数类型。
 */
template <typename T, typename C, typename... Ts>
struct GetMemberType<T (C::*)(Ts...) const&> {
    static constexpr MemberType value = MemberType::MEMBER_FUNCTION;
};

/**
 * @struct GetMemberType<T (C::*)(Ts...)&&>
 * @brief GetMemberType 特化，用于右值引用成员函数指针。
 *
 * 确定右值引用成员函数指针的类型为 MEMBER_FUNCTION。
 * @tparam T 返回类型。
 * @tparam C 所属类的类型。
 * @tparam Ts 函数参数类型。
 */
template <typename T, typename C, typename... Ts>
struct GetMemberType<T (C::*)(Ts...) &&> {
    static constexpr MemberType value = MemberType::MEMBER_FUNCTION;
};

/**
 * @struct GetMemberType<T (C::*)(Ts...) const&&>
 * @brief GetMemberType 特化，用于 const 右值引用成员函数指针。
 *
 * 确定 const 右值引用成员函数指针的类型为 MEMBER_FUNCTION。
 * @tparam T 返回类型。
 * @tparam C 所属类的类型。
 * @tparam Ts 函数参数类型。
 */
template <typename T, typename C, typename... Ts>
struct GetMemberType<T (C::*)(Ts...) const&&> {
    static constexpr MemberType value = MemberType::MEMBER_FUNCTION;
};

#if __cpp_noexcept_function_type
/**
 * @struct GetMemberType<T (C::*)(Ts...) noexcept>
 * @brief GetMemberType 特化，用于 noexcept 成员函数指针。
 *
 * 确定 noexcept 成员函数指针的类型为 MEMBER_FUNCTION。
 * @tparam T 返回类型。
 * @tparam C 所属类的类型。
 * @tparam Ts 函数参数类型。
 */
template <typename T, typename C, typename... Ts>
struct GetMemberType<T (C::*)(Ts...) noexcept> {
    static constexpr MemberType value = MemberType::MEMBER_FUNCTION;
};

/**
 * @struct GetMemberType<T (C::*)(Ts...) const noexcept>
 * @brief GetMemberType 特化，用于 const noexcept 成员函数指针。
 *
 * 确定 const noexcept 成员函数指针的类型为 MEMBER_FUNCTION。
 * @tparam T 返回类型。
 * @tparam C 所属类的类型。
 * @tparam Ts 函数参数类型。
 */
template <typename T, typename C, typename... Ts>
struct GetMemberType<T (C::*)(Ts...) const noexcept> {
    static constexpr MemberType value = MemberType::MEMBER_FUNCTION;
};

/**
 * @struct GetMemberType<T (C::*)(Ts...) & noexcept>
 * @brief GetMemberType 特化，用于左值引用 noexcept 成员函数指针。
 *
 * 确定左值引用 noexcept 成员函数指针的类型为 MEMBER_FUNCTION。
 * @tparam T 返回类型。
 * @tparam C 所属类的类型。
 * @tparam Ts 函数参数类型。
 */
template <typename T, typename C, typename... Ts>
struct GetMemberType<T (C::*)(Ts...) & noexcept> {
    static constexpr MemberType value = MemberType::MEMBER_FUNCTION;
};

/**
 * @struct GetMemberType<T (C::*)(Ts...) const & noexcept>
 * @brief GetMemberType 特化，用于 const 左值引用 noexcept 成员函数指针。
 *
 * 确定 const 左值引用 noexcept 成员函数指针的类型为 MEMBER_FUNCTION。
 * @tparam T 返回类型。
 * @tparam C 所属类的类型。
 * @tparam Ts 函数参数类型。
 */
template <typename T, typename C, typename... Ts>
struct GetMemberType<T (C::*)(Ts...) const & noexcept> {
    static constexpr MemberType value = MemberType::MEMBER_FUNCTION;
};

/**
 * @struct GetMemberType<T (C::*)(Ts...) && noexcept>
 * @brief GetMemberType 特化，用于右值引用 noexcept 成员函数指针。
 *
 * 确定右值引用 noexcept 成员函数指针的类型为 MEMBER_FUNCTION。
 * @tparam T 返回类型。
 * @tparam C 所属类的类型。
 * @tparam Ts 函数参数类型。
 */
template <typename T, typename C, typename... Ts>
struct GetMemberType<T (C::*)(Ts...) && noexcept> {
    static constexpr MemberType value = MemberType::MEMBER_FUNCTION;
};

/**
 * @struct GetMemberType<T (C::*)(Ts...) const && noexcept>
 * @brief GetMemberType 特化，用于 const 右值引用 noexcept 成员函数指针。
 *
 * 确定 const 右值引用 noexcept 成员函数指针的类型为 MEMBER_FUNCTION。
 * @tparam T 返回类型。
 * @tparam C 所属类的类型。
 * @tparam Ts 函数参数类型。
 */
template <typename T, typename C, typename... Ts>
struct GetMemberType<T (C::*)(Ts...) const && noexcept> {
    static constexpr MemberType value = MemberType::MEMBER_FUNCTION;
};
#endif

/**
 * @struct GetMemberType<T (*)(Ts...)>
 * @brief GetMemberType 特化，用于静态函数指针。
 *
 * 确定静态函数指针的类型为 STATIC_FUNCTION。
 * @tparam T 返回类型。
 * @tparam Ts 函数参数类型。
 */
template <typename T, typename... Ts>
struct GetMemberType<T (*)(Ts...)> {
    static constexpr MemberType value = MemberType::STATIC_FUNCTION;
};

#if __cpp_noexcept_function_type
/**
 * @struct GetMemberType<T (*)(Ts...) noexcept>
 * @brief GetMemberType 特化，用于 noexcept 静态函数指针。
 *
 * 确定 noexcept 静态函数指针的类型为 STATIC_FUNCTION。
 * @tparam T 返回类型。
 * @tparam Ts 函数参数类型。
 */
template <class T, class... Ts>
struct GetMemberType<T (*)(Ts...) noexcept> {
    static constexpr MemberType value = MemberType::STATIC_FUNCTION;
};
#endif

/**
 * @brief 检查类型是否定义了 forEachMemberPtr 成员函数。
 *
 * 使用 SFINAE 检测类型 T 是否具有静态成员函数 forEachMemberPtr。
 * @tparam T 要检查的类型。
 * @param int 优先级参数，用于 SFINAE。
 * @return 如果类型 T 定义了 forEachMemberPtr，则返回 true，否则返回 false。
 */
template <typename T>
inline constexpr auto HasMemberTest(int) -> decltype(T::template forEachMemberPtr<T>(nullptr),
                                                     true) {
    return true;
}

/**
 * @brief 默认情况下的 HasMemberTest，处理类型未定义 forEachMemberPtr 的情况。
 *
 * @tparam T 要检查的类型。
 * @return 始终返回 false。
 */
template <typename T>
inline constexpr bool HasMemberTest(...) {
    return false;
}

/**
 * @struct ReflectTrait
 * @brief 反射特性模板，用于管理类型的反射元信息。
 *
 * 提供静态方法，用于检查和遍历类型的成员指针。
 * @tparam T 要反射的类型。
 */
template <typename T>
struct ReflectTrait {
    /**
     * @brief 检查类型是否支持成员指针遍历。
     * @return 如果类型 T 定义了 forEachMemberPtr，则返回 true，否则返回 false。
     */
    static constexpr bool hasForEachMemberPtr() {
        return ccjson::reflect::HasMemberTest<T>(0);
    }

    /**
     * @brief 遍历类型的成员指针。
     * @tparam Function 可调用的函数对象，接受成员名称和成员指针。
     * @param function 要应用的函数。
     */
    template <typename Function>
    static constexpr void forEachMemberPtr(Function&& function) {
        T::template forEachMemberPtr<T>(function);
    }
};

/**
 * @brief 处理单个成员指针的宏。
 *
 * 将成员名称和成员指针传递给函数。
 * @param x 成员名称。
 */
#define REFLECT_TYPE_ONE_MEMBER_PTR(x) function(#x, &This::x);

/**
 * @brief 为非模板类型定义反射元信息。
 *
 * 使用 REFLECT_PP_FOREACH 宏遍历成员列表，生成 forEachMemberPtr 方法。
 * @param Object 要反射的类型。
 * @param ... 成员名称列表。
 */
#define REFLECT_TYPE(Object, ...)                                                                  \
    template <>                                                                                    \
    struct ccjson::reflect::ReflectTrait<Object> {                                                 \
        using This = Object;                                                                       \
                                                                                                   \
        static constexpr bool hasForEachMemberPtr() {                                              \
            return true;                                                                           \
        }                                                                                          \
                                                                                                   \
        template <typename Function>                                                               \
        static constexpr void forEachMemberPtr(Function function) {                                \
            REFLECT_PP_FOREACH(REFLECT_TYPE_ONE_MEMBER_PTR, __VA_ARGS__)                           \
        }                                                                                          \
    };

/**
 * @brief 提取模板类型的第一个参数。
 *
 * 用于处理模板类型的宏展开。
 * @param x 模板类型定义。
 * @param ... 剩余参数。
 */
#define REFLECT_TYPE_TEMPLATED_FIRST(x, ...) REFLECT_EXPAND x

/**
 * @brief 提取模板类型的剩余参数。
 *
 * 用于处理模板类型的宏展开。
 * @param x 模板类型定义。
 * @param ... 剩余参数。
 */
#define REFLECT_TYPE_TEMPLATED_REST(x, ...) __VA_ARGS__

/**
 * @brief 为模板类型定义反射元信息。
 *
 * 使用 REFLECT_PP_FOREACH 宏遍历成员列表，生成 forEachMemberPtr 方法，支持模板类型。
 * @param Type 模板类型定义。
 * @param ... 成员名称列表。
 */
#define REFLECT_TYPE_TEMPLATED(Type, ...)                                                          \
    template <REFLECT_EXPAND(REFLECT_TYPE_TEMPLATED_REST Type)>                                    \
    struct ccjson::reflect::ReflectTrait<REFLECT_EXPAND(REFLECT_TYPE_TEMPLATED_FIRST Type)> {      \
        using This = REFLECT_EXPAND(REFLECT_TYPE_TEMPLATED_FIRST Type);                            \
                                                                                                   \
        static constexpr bool hasForEachMemberPtr() {                                              \
            return true;                                                                           \
        }                                                                                          \
                                                                                                   \
        template <typename Function>                                                               \
        static constexpr void forEachMemberPtr(Function function) {                                \
            REFLECT_PP_FOREACH(REFLECT_TYPE_ONE_MEMBER_PTR, __VA_ARGS__)                           \
        }                                                                                          \
    };

/**
 * @brief 处理单个对象成员的宏。
 *
 * 将成员名称和成员指针传递给函数。
 * @param name 成员名称。
 */
#define REFLECT_ONE_OBJECT_MEMBER(name) function(#name, &This::name);

/**
 * @brief 为类定义反射元信息。
 *
 * 在类内部定义 forEachMemberPtr 方法，遍历指定的成员列表。
 * @param ... 成员名称列表。
 */
#define REFLECT(...)                                                                               \
    template <typename This, typename Function>                                                    \
    static constexpr void forEachMemberPtr(Function&& function) {                                  \
        REFLECT_PP_FOREACH(REFLECT_ONE_OBJECT_MEMBER, __VA_ARGS__)                                 \
    }

/**
 * @brief 遍历类型的成员指针。
 *
 * 委托给 ReflectTrait 的 forEachMemberPtr 方法。
 * @tparam T 要反射的类型。
 * @tparam Function 可调用的函数对象，接受成员名称和成员指针。
 * @param function 要应用的函数。
 */
template <class T, class Function>
constexpr void forEachMemberPtr(Function&& function) {
    ReflectTrait<std::decay_t<T>>::forEachMemberPtr(function);
}

/**
 * @brief 遍历对象的成员变量。
 *
 * 过滤出成员变量并应用指定函数。
 * @tparam T 对象的类型。
 * @tparam Function 可调用的函数对象，接受成员名称和成员值。
 * @param object 要遍历的对象。
 * @param function 要应用的函数。
 */
template <class T, class Function>
constexpr void forEachMember(T&& object, Function&& function) {
    ReflectTrait<std::decay_t<T>>::forEachMemberPtr([&](const char* name, auto member) {
        if constexpr (GetMemberType<std::decay_t<decltype(member)>>::value ==
                      MemberType::MEMBER_VARIABLE) {
            function(name, object.*member);
        }
    });
}

/**
 * @brief 序列化不支持反射的对象。
 *
 * 如果类型 T 不支持 forEachMemberPtr，则假定对象已实现 toJson 函数。
 * @tparam T 要序列化的类型。
 * @param object 要序列化的对象。
 * @return 序列化后的 JsonValue 对象。
 * @note 需要类型 T 实现 toJson 函数，否则直接返回对象（可能引发编译错误）。
 */
template <typename T,
          typename std::enable_if_t<!reflect::ReflectTrait<T>::hasForEachMemberPtr(), int> = 0>
ccjson::JsonValue serialize(const T& object) {
    return object;
}

/**
 * @brief 序列化支持反射的对象。
 *
 * 遍历对象的成员变量并递归序列化为 JSON。
 * @tparam T 要序列化的类型（需支持 forEachMemberPtr）。
 * @param object 要序列化的对象。
 * @return 序列化后的 JsonValue 对象。
 */
template <typename T,
          typename std::enable_if_t<reflect::ReflectTrait<T>::hasForEachMemberPtr(), int> = 0>
ccjson::JsonValue serialize(const T& object) {
    ccjson::JsonValue root;
    ccjson::reflect::forEachMember(
        object, [&](const char* key, auto& value) { root[key] = serialize(value); });
    return root;
}

/**
 * @brief 反序列化不支持反射的类型。
 *
 * 如果类型 T 不支持 forEachMemberPtr，则使用 JsonValue 的 get 方法进行转换。
 * @tparam T 要反序列化的类型。
 * @param root JSON 值。
 * @return 反序列化后的 T 类型对象。
 * @exception JsonException 如果类型转换失败，抛出异常。
 * @note 需要类型 T 实现 fromJson 函数或支持 JsonValue::get。
 */
template <typename T,
          typename std::enable_if_t<!reflect::ReflectTrait<T>::hasForEachMemberPtr(), int> = 0>
T deserialize(const ccjson::JsonValue& root) {
    return root.get<T>();
}

/**
 * @brief 反序列化支持反射的类型。
 *
 * 遍历类型的成员变量并递归从 JSON 反序列化。
 * @tparam T 要反序列化的类型（需支持 forEachMemberPtr）。
 * @param root JSON 值。
 * @return 反序列化后的 T 类型对象。
 */
template <typename T,
          typename std::enable_if_t<reflect::ReflectTrait<T>::hasForEachMemberPtr(), int> = 0>
T deserialize(const ccjson::JsonValue& root) {
    T object;
    ccjson::reflect::forEachMember(object, [&](const char* key, auto& value) {
        value = deserialize<std::decay_t<decltype(value)>>(root[key]);
    });
    return object;
}

/**
 * @brief 从 JSON 字符串反序列化为指定类型。
 *
 * 解析 JSON 字符串并反序列化为类型 T 的对象。
 * @tparam T 要反序列化的类型。
 * @param json JSON 字符串。
 * @return 反序列化后的 T 类型对象。
 * @exception JsonParseException 如果 JSON 字符串格式无效，抛出异常。
 * @exception JsonException 如果类型转换失败，抛出异常。
 */
template <typename T>
T deserialize(const std::string& json) {
    return deserialize<T>(ccjson::JsonParser::parse(json));
}
}  // namespace ccjson::reflect
#endif