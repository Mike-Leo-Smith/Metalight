//
// Created by Mike Smith on 2020/2/14.
//

#pragma once

#include <core/viewport.h>
#include <core/mathematics.h>

namespace luisa::sampler::independent {

template<uint N>
LUISA_DEVICE_CALLABLE inline uint tea(uint v0, uint v1) {
    auto s0 = 0u;
    for (auto n = 0u; n < N; n++) {
        s0 += 0x9e3779b9u;
        v0 += ((v1 << 4u) + 0xa341316cu) ^ (v1 + s0) ^ ((v1 >> 5u) + 0xc8013ea4u);
        v1 += ((v0 << 4u) + 0xad90777du) ^ (v0 + s0) ^ ((v0 >> 5u) + 0x7e95761eu);
    }
    return v0;
}

LUISA_DEVICE_CALLABLE inline uint lcg(LUISA_THREAD_SPACE uint &prev) {
    constexpr auto LCG_A = 1664525u;
    constexpr auto LCG_C = 1013904223u;
    prev = (LCG_A * prev + LCG_C);
    return prev & 0x00ffffffu;
}

LUISA_DEVICE_CALLABLE inline float rnd(LUISA_THREAD_SPACE unsigned int &prev) {
    constexpr auto inv = 1.0f / static_cast<float>(0x01000000);
    return static_cast<float>(lcg(prev)) * inv;
}

using State = uint;

LUISA_DEVICE_CALLABLE inline void reset_states(
    Viewport film_viewport,
    LUISA_DEVICE_SPACE State *sampler_state_buffer,
    uint tid) {
    
    if (tid < film_viewport.size.x * film_viewport.size.y) {
        auto pixel_x = tid % film_viewport.size.x + film_viewport.origin.x;
        auto pixel_y = tid / film_viewport.size.x + film_viewport.origin.y;
        sampler_state_buffer[tid] = tea<4>(pixel_x, pixel_y);
    }
    
}

struct GenerateSamplesKernelUniforms {
    Viewport tile_viewport;
    Viewport film_viewport;
};

template<uint dimension>
LUISA_DEVICE_CALLABLE inline void generate_samples(
    LUISA_DEVICE_SPACE State *sampler_state_buffer,
    LUISA_DEVICE_SPACE const uint *ray_queue,
    uint ray_count,
    LUISA_DEVICE_SPACE float *sample_buffer,
    LUISA_UNIFORM_SPACE GenerateSamplesKernelUniforms &uniforms,
    uint tid) {
    
    if (tid < ray_count) {
        auto ray_index_in_tile = ray_queue[tid];
        auto ray_x = uniforms.tile_viewport.origin.x + ray_index_in_tile % uniforms.tile_viewport.size.x;
        auto ray_y = uniforms.tile_viewport.origin.y + ray_index_in_tile / uniforms.tile_viewport.size.x;
        auto ray_index = ray_y * uniforms.film_viewport.size.x + ray_x;
        auto state = sampler_state_buffer[ray_index];
        for (auto i = 0u; i < dimension; i++) {
            sample_buffer[tid * dimension + i] = rnd(state);
        }
        sampler_state_buffer[ray_index] = state;
    }
}

LUISA_DEVICE_CALLABLE inline void generate_camera_samples(
    LUISA_DEVICE_SPACE State *sampler_state_buffer,
    LUISA_DEVICE_SPACE float4 *sample_buffer,
    LUISA_UNIFORM_SPACE GenerateSamplesKernelUniforms &uniforms,
    uint tid) {
    
    if (tid < uniforms.tile_viewport.size.x * uniforms.tile_viewport.size.y) {
        auto ray_x = uniforms.tile_viewport.origin.x + tid % uniforms.tile_viewport.size.x;
        auto ray_y = uniforms.tile_viewport.origin.y + tid / uniforms.tile_viewport.size.x;
        auto ray_index = ray_y * uniforms.film_viewport.size.x + ray_x;
        auto state = sampler_state_buffer[ray_index];
        auto r0 = rnd(state);
        auto r1 = rnd(state);
        auto r2 = rnd(state);
        auto r3 = rnd(state);
        sample_buffer[tid] = make_float4(r0, r1, r2, r3);
        sampler_state_buffer[ray_index] = state;
    }
}

}

#ifndef LUISA_DEVICE_COMPATIBLE

#include <core/sampler.h>

namespace luisa {

class IndependentSampler : public Sampler {

protected:
    std::unique_ptr<Kernel> _reset_states_kernel;
    std::unique_ptr<Kernel> _generate_1d_samples_kernel;
    std::unique_ptr<Kernel> _generate_2d_samples_kernel;
    std::unique_ptr<Kernel> _generate_3d_samples_kernel;
    std::unique_ptr<Kernel> _generate_4d_samples_kernel;
    std::unique_ptr<Kernel> _generate_camera_samples_kernel;
    std::unique_ptr<Buffer<sampler::independent::State>> _state_buffer;
    
    void _generate_samples(KernelDispatcher &dispatch, BufferView<uint> ray_queue_buffer, BufferView<uint> ray_count_buffer, BufferView<float> sample_buffer) override;
    void _generate_samples(KernelDispatcher &dispatch, BufferView<uint> ray_queue_buffer, BufferView<uint> ray_count_buffer, BufferView<float2> sample_buffer) override;
    void _generate_samples(KernelDispatcher &dispatch, BufferView<uint> ray_queue_buffer, BufferView<uint> ray_count_buffer, BufferView<float3> sample_buffer) override;
    void _generate_samples(KernelDispatcher &dispatch, BufferView<uint> ray_queue_buffer, BufferView<uint> ray_count_buffer, BufferView<float4> sample_buffer) override;
    
    void _reset_states() override;
    
    void _start_next_frame(KernelDispatcher &dispatch[[maybe_unused]]) override {}
    void _prepare_for_tile(KernelDispatcher &dispatch[[maybe_unused]]) override {}

public:
    IndependentSampler(Device *device, const ParameterSet &parameter_set);
    BufferView<float4> generate_camera_samples(KernelDispatcher &dispatch) override;
};

}

#endif
