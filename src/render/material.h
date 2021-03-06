//
// Created by Mike Smith on 2020/9/5.
//

#pragma once

#include <algorithm>
#include <numeric>

#include <compute/type_desc.h>
#include <compute/buffer.h>

#include <render/plugin.h>
#include <render/surface.h>

namespace luisa::render {

struct MaterialHandle {
    uint shader_offset;
    uint shader_count;
};

}

LUISA_STRUCT(luisa::render::MaterialHandle, shader_offset, shader_count)

namespace luisa::render {

using compute::dsl::Var;
using compute::dsl::Expr;

class Material : public Plugin {

public:
    struct Lobe {
        std::shared_ptr<SurfaceShader> shader;
        float weight;
        Lobe(std::shared_ptr<SurfaceShader> shader, float weight) noexcept
            : shader{std::move(shader)}, weight{weight} {}
    };

protected:
    std::vector<Lobe> _lobes;

public:
    Material(Device *device, const ParameterSet &params) : Plugin{device, params} {}
    
    [[nodiscard]] uint required_data_block_count() const noexcept {
        return std::accumulate(_lobes.cbegin(), _lobes.cend(), 0u, [](uint sum, const auto &lobe) noexcept {
            return sum + lobe.shader->required_data_block_count();
        });
    }
    
    [[nodiscard]] uint required_emission_data_block_count() const noexcept {
        return std::accumulate(_lobes.cbegin(), _lobes.cend(), 0u, [](uint sum, const auto &lobe) noexcept {
            return sum + (lobe.shader->is_emissive() ? lobe.shader->required_data_block_count() : 0u);
        });
    }
    
    [[nodiscard]] float sum_weight() const noexcept {
        return std::accumulate(_lobes.cbegin(), _lobes.cend(), 0.0f, [](float sum, const auto &lobe) noexcept {
            return sum + lobe.weight;
        });
    }
    
    [[nodiscard]] float sum_emission_weight() const noexcept {
        return std::accumulate(_lobes.cbegin(), _lobes.cend(), 0.0f, [](float sum, const auto &lobe) noexcept {
            return sum + (lobe.shader->is_emissive() ? lobe.weight : 0.0f);
        });
    }
    
    [[nodiscard]] bool is_emissive() const noexcept {
        return std::any_of(_lobes.cbegin(), _lobes.cend(), [](const auto &lobe) noexcept {
            return lobe.shader->is_emissive();
        });
    }
    
    [[nodiscard]] const std::vector<Lobe> &lobes() const noexcept { return _lobes; }
    [[nodiscard]] uint lobe_count() const noexcept { return _lobes.size(); }
    [[nodiscard]] uint emissive_lobe_count() const noexcept {
        return std::count_if(_lobes.cbegin(), _lobes.cend(), [](const Lobe &lobe) noexcept { return lobe.shader->is_emissive(); });
    }
};

}
