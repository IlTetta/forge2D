#pragma once
#include <cstdint>

namespace forge {

class VertexBuffer {
public:
    explicit VertexBuffer(uint32_t size);
    ~VertexBuffer();

    VertexBuffer(const VertexBuffer&) = delete;
    VertexBuffer& operator=(const VertexBuffer&) = delete;

    void bind() const;
    void unbind() const;
    void setData(const void* data, uint32_t size);

private:
    uint32_t m_id = 0;

};
    
} // namespace forge