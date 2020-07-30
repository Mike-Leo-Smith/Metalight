//
// Created by Mike Smith on 2020/7/30.
//

#pragma once

#include <compute/function.h>

namespace luisa::dsl {

#define MAP_VARIABLE_NAME_TO_ARGUMENT_DEF(name) Variable name
#define MAP_VARIABLE_NAMES_TO_ARGUMENT_LIST(...) LUISA_MAP_MACRO_LIST(MAP_VARIABLE_NAME_TO_ARGUMENT_DEF, __VA_ARGS__)

#define MAKE_BUILTIN_FUNCTION_DEF(func, ...)                                       \
inline Variable func(MAP_VARIABLE_NAMES_TO_ARGUMENT_LIST(__VA_ARGS__)) {           \
    std::vector<Variable> args{__VA_ARGS__};                                       \
    auto f = args.front().function();                                              \
    return f->add_expression(std::make_unique<CallExpr>(#func, std::move(args)));  \
}                                                                                  \

MAKE_BUILTIN_FUNCTION_DEF(select, cond, tv, fv)
MAKE_BUILTIN_FUNCTION_DEF(sin, x)
MAKE_BUILTIN_FUNCTION_DEF(cos, x)
MAKE_BUILTIN_FUNCTION_DEF(tan, x)
MAKE_BUILTIN_FUNCTION_DEF(asin, x)
MAKE_BUILTIN_FUNCTION_DEF(acos, x)
MAKE_BUILTIN_FUNCTION_DEF(atan, x)
MAKE_BUILTIN_FUNCTION_DEF(atan2, y, x)
MAKE_BUILTIN_FUNCTION_DEF(ceil, x)
MAKE_BUILTIN_FUNCTION_DEF(floor, x)
MAKE_BUILTIN_FUNCTION_DEF(round, x)
MAKE_BUILTIN_FUNCTION_DEF(pow, x, y)
MAKE_BUILTIN_FUNCTION_DEF(exp, x)
MAKE_BUILTIN_FUNCTION_DEF(log, x)
MAKE_BUILTIN_FUNCTION_DEF(log2, x)
MAKE_BUILTIN_FUNCTION_DEF(log10, x)
MAKE_BUILTIN_FUNCTION_DEF(min, x, y)
MAKE_BUILTIN_FUNCTION_DEF(max, x, y)
MAKE_BUILTIN_FUNCTION_DEF(abs, x)
MAKE_BUILTIN_FUNCTION_DEF(clamp, x, a, b)
MAKE_BUILTIN_FUNCTION_DEF(lerp, a, b, t)
MAKE_BUILTIN_FUNCTION_DEF(radians, deg)
MAKE_BUILTIN_FUNCTION_DEF(degrees, rad)
MAKE_BUILTIN_FUNCTION_DEF(normalize, v)
MAKE_BUILTIN_FUNCTION_DEF(length, v)
MAKE_BUILTIN_FUNCTION_DEF(dot, u, v)
MAKE_BUILTIN_FUNCTION_DEF(cross, u, v)

MAKE_BUILTIN_FUNCTION_DEF(make_float3x3, val_or_mat4)
MAKE_BUILTIN_FUNCTION_DEF(make_float3x3, c0, c1, c2)
MAKE_BUILTIN_FUNCTION_DEF(make_float3x3, m00, m01, m02, m10, m11, m12, m20, m21, m22)

MAKE_BUILTIN_FUNCTION_DEF(make_float4x4, val_or_mat3)
MAKE_BUILTIN_FUNCTION_DEF(make_float4x4, c0, c1, c2, c3)
MAKE_BUILTIN_FUNCTION_DEF(make_float4x4, m00, m01, m02, m03, m10, m11, m12, m13, m20, m21, m22, m23, m30, m31, m32, m33)

// make_vec2
#define MAKE_BUILTIN_FUNCTION_DEF_MAKE_VEC2(T)         \
MAKE_BUILTIN_FUNCTION_DEF(make_##T##2, v)              \
MAKE_BUILTIN_FUNCTION_DEF(make_##T##2, x, y)           \

// make_vec3
#define MAKE_BUILTIN_FUNCTION_DEF_MAKE_VEC3(T)         \
MAKE_BUILTIN_FUNCTION_DEF(make_##T##3, v)              \
MAKE_BUILTIN_FUNCTION_DEF(make_##T##3, x, y)           \
MAKE_BUILTIN_FUNCTION_DEF(make_##T##3, x, y, z)        \

// make_vec4
#define MAKE_BUILTIN_FUNCTION_DEF_MAKE_VEC4(T)         \
MAKE_BUILTIN_FUNCTION_DEF(make_##T##4, v)              \
MAKE_BUILTIN_FUNCTION_DEF(make_##T##4, x, y)           \
MAKE_BUILTIN_FUNCTION_DEF(make_##T##4, x, y, z)        \
MAKE_BUILTIN_FUNCTION_DEF(make_##T##4, x, y, z, w)     \

// make_packed_vec3
#define MAKE_BUILTIN_FUNCTION_DEF_MAKE_PACKED_VEC3(T)  \
MAKE_BUILTIN_FUNCTION_DEF(make_packed_##T##3, v)       \

#define MAKE_BUILTIN_FUNCTION_DEF_MAKE_VEC(T)          \
MAKE_BUILTIN_FUNCTION_DEF_MAKE_VEC2(T)                 \
MAKE_BUILTIN_FUNCTION_DEF_MAKE_VEC3(T)                 \
MAKE_BUILTIN_FUNCTION_DEF_MAKE_VEC4(T)                 \
MAKE_BUILTIN_FUNCTION_DEF_MAKE_PACKED_VEC3(T)          \

MAKE_BUILTIN_FUNCTION_DEF_MAKE_VEC(bool)
MAKE_BUILTIN_FUNCTION_DEF_MAKE_VEC(float)
MAKE_BUILTIN_FUNCTION_DEF_MAKE_VEC(byte)
MAKE_BUILTIN_FUNCTION_DEF_MAKE_VEC(ubyte)
MAKE_BUILTIN_FUNCTION_DEF_MAKE_VEC(short)
MAKE_BUILTIN_FUNCTION_DEF_MAKE_VEC(ushort)
MAKE_BUILTIN_FUNCTION_DEF_MAKE_VEC(int)
MAKE_BUILTIN_FUNCTION_DEF_MAKE_VEC(uint)

#undef MAKE_BUILTIN_FUNCTION_DEF_MAKE_VEC
#undef MAKE_BUILTIN_FUNCTION_DEF_MAKE_VEC4
#undef MAKE_BUILTIN_FUNCTION_DEF_MAKE_VEC3
#undef MAKE_BUILTIN_FUNCTION_DEF_MAKE_VEC2

MAKE_BUILTIN_FUNCTION_DEF(inverse, INVERSE, m)
MAKE_BUILTIN_FUNCTION_DEF(transpose, TRANSPOSE, m)

#undef MAKE_BUILTIN_FUNCTION_DEF
#undef MAP_VARIABLE_NAMES_TO_ARGUMENT_LIST
#undef MAP_VARIABLE_NAME_TO_ARGUMENT_DEF

template<typename T>
[[nodiscard]] inline Variable static_cast_(Variable v) {
    return v.function()->add_expression(std::make_unique<CastExpr>(CastOp::STATIC, v, type_desc<T>));
}

template<typename T>
[[nodiscard]] inline Variable reinterpret_cast_(Variable v) {
    return v.function()->add_expression(std::make_unique<CastExpr>(CastOp::REINTERPRET, v, type_desc<T>));
}

template<typename T>
[[nodiscard]] inline Variable bitwise_cast_(Variable v) {
    return v.function()->add_expression(std::make_unique<CastExpr>(CastOp::BITWISE, v, type_desc<T>));
}

}
