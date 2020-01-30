//
// Created by Mike Smith on 2019/10/24.
//

#pragma once

#include <vector>
#include <util/noncopyable.h>

namespace luisa {

enum struct BufferStorageTag {
    DEVICE_PRIVATE,
    MANAGED
};

class Buffer : Noncopyable {

protected:
    size_t _capacity;
    BufferStorageTag _storage;

public:
    Buffer(size_t capacity, BufferStorageTag storage) noexcept : _capacity{capacity}, _storage{storage} {};
    virtual ~Buffer() noexcept = default;
    virtual void upload(const void *host_data, size_t size, size_t offset) = 0;
    virtual void upload(const void *host_data, size_t size) { upload(host_data, size, 0ul); }
    virtual void upload() { upload(nullptr, 0ul); }
    virtual void synchronize(struct KernelDispatcher &dispatch) = 0;
    [[nodiscard]] virtual const void *data() const { return const_cast<Buffer *>(this)->data(); }
    [[nodiscard]] virtual void *data() = 0;
    [[nodiscard]] virtual size_t capacity() const noexcept { return _capacity; }
};

}
