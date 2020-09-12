#include <random>
#include <compute/device.h>
#include <compute/dsl.h>

using namespace luisa;
using namespace luisa::compute;
using namespace luisa::compute::dsl;

int main(int argc, char *argv[]) {
    
    Context context{argc, argv};
    auto device = Device::create(&context);
    
    constexpr auto width = 1280u;
    constexpr auto height = 720u;
    
    auto texture = device->allocate_texture<uchar4>(width, height);
    auto kernel = device->compile_kernel([&] {
        
        auto image_size = immutable(make_uint2(width, height));
        auto txy = thread_xy();
        If (all(txy < image_size)) {
            
            Var xy_f = make_float2(txy);
            Var size_f = make_float2(image_size) - 1.0f;
            
            auto linear_to_srgb = [](Expr<float3> u) -> Expr<float3> {
                return select(u <= 0.0031308f, 12.92f * u, 1.055f * pow(u, 1.0f / 2.4f) - 0.055f);
            };
            Var color = make_float4(linear_to_srgb(make_float3(xy_f / size_f, 1.0f)), 1.0f);
            texture.write(txy, color);
        };
    });
    
    device->launch([&](Dispatcher &dispatch) {
        dispatch(kernel.parallelize(make_uint2(width, height)));
        dispatch(texture.save(context.working_path("test.jpg")));
    });
    device->synchronize();
}
