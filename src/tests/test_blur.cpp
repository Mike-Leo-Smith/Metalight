#include <random>
#include <opencv2/opencv.hpp>
#include <compute/device.h>
#include <compute/dsl.h>

using namespace luisa;
using namespace luisa::compute;
using namespace luisa::compute::dsl;

void blur_x_or_y(TextureView input, TextureView output, int width, int height, int rx, int ry) noexcept {
    
    LUISA_ERROR_IF_NOT(rx * ry == 0, "At least one of rx and ry should be zero (got rx = ", rx, ", ry = ", ry, ").");
    
    Var tx = cast<int>(thread_xy().x());
    Var ty = cast<int>(thread_xy().y());
    If (tx < width && ty < height) {
        Var<float4> sum;
        for (auto dx = -rx; dx <= rx; dx++) {
            auto x = tx + dx;
            If (x >= 0 && x < width) {
                sum += make_float4(make_float3(input.read(make_uint2(x, ty))), 1.0f);
            };
        }
        for (auto dy = -ry; dy <= ry; dy++) {
            auto y = ty + dy;
            If (y >= 0 && y < height) {
                sum += make_float4(make_float3(input.read(make_uint2(tx, y))), 1.0f);
            };
        }
        output.write(thread_xy(), make_float4(make_float3(sum) / sum.w(), 1.0f));
    };
}

int main(int argc, char *argv[]) {
    
    Context context{argc, argv};
    auto device = Device::create(&context);
    
    auto image = cv::imread("data/images/luisa.png", cv::IMREAD_COLOR);
    if (image.type() == CV_8UC3) { cv::cvtColor(image, image, cv::COLOR_BGR2BGRA); }
    
    auto width = static_cast<uint32_t>(image.cols);
    auto height = static_cast<uint32_t>(image.rows);
    
    auto texture = device->allocate_texture<uchar4>(width, height);
    auto temp_texture = device->allocate_texture<uchar4>(width, height);
    
    device->launch([&](Dispatcher &dispatch) noexcept {
        dispatch(texture.copy_from(image.data));
    });
    
    constexpr auto rx = 5;
    constexpr auto ry = 10;
    auto blur_x = device->compile_kernel([&] { blur_x_or_y(texture, temp_texture, width, height, rx, 0); });
    auto blur_y = device->compile_kernel([&] { blur_x_or_y(texture, temp_texture, width, height, 0, ry); });
    
    LUISA_INFO("Processing...");
    device->launch([&](Dispatcher &dispatch) {
        dispatch(blur_x->parallelize(make_uint2(width, height)));
        dispatch(temp_texture.copy_to(texture));
        for (auto i = 0; i < 20; i++) {
            dispatch(blur_y->parallelize(make_uint2(width, height)));
            dispatch(temp_texture.copy_to(texture));
        }
        dispatch(temp_texture.copy_to(image.data));
    });
    device->synchronize();
    LUISA_INFO("Done.");
    
    cv::imshow("Result", image);
    cv::waitKey();
    
    LUISA_INFO("Saving image...");
    cv::imwrite("data/images/luisa-blur.png", image);
}
