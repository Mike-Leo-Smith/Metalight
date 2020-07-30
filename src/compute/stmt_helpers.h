//
// Created by Mike Smith on 2020/7/30.
//

#pragma once

#include <compute/function.h>

namespace luisa::dsl {

template<typename True, typename False,
    std::enable_if_t<std::conjunction_v<std::is_invocable<True>, std::is_invocable<False>>, int> = 0>
inline void if_(Variable cond, True &&true_branch, False &&false_branch) {
    auto f = cond.function();
    f->add_statement(std::make_unique<IfStmt>(cond));
    f->block(std::forward<True>(true_branch));
    f->add_statement(std::make_unique<KeywordStmt>("else"));
    f->block(std::forward<False>(false_branch));
}

template<typename True,
    std::enable_if_t<std::is_invocable_v<True>, int> = 0>
inline void if_(Variable cond, True &&true_branch) {
    auto f = cond.function();
    f->add_statement(std::make_unique<IfStmt>(cond));
    f->block(std::forward<True>(true_branch));
}

template<typename Body,
    std::enable_if_t<std::is_invocable_v<Body>, int> = 0>
inline void while_(Variable cond, Body &&body) {
    auto f = cond.function();
    f->add_statement(std::make_unique<WhileStmt>(cond));
    f->block(std::forward<Body>(body));
}

template<typename Body,
    std::enable_if_t<std::is_invocable_v<Body>, int> = 0>
inline void do_while_(Body &&body, Variable cond) {
    auto f = cond.function();
    f->add_statement(std::make_unique<KeywordStmt>("do"));
    f->block(std::forward<Body>(body));
    f->add_statement(std::make_unique<WhileStmt>(cond));
    f->add_statement(std::make_unique<KeywordStmt>(";"));
}

inline void void_(Variable v) {
    auto f = v.function();
    f->add_statement(std::make_unique<ExprStmt>(v));
}

}
