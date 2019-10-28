//
// Created by Mike Smith on 2019/10/25.
//

#pragma once

#include "kernel.h"

struct Acceleration : Noncopyable {
    virtual ~Acceleration() = default;
    virtual void trace_any(KernelDispatcher &dispatch, Buffer &ray_buffer, Buffer &intersection_buffer, size_t ray_count) = 0;
    virtual void trace_nearest(KernelDispatcher &dispatch, Buffer &ray_buffer, Buffer &intersection_buffer, size_t ray_count) = 0;
};