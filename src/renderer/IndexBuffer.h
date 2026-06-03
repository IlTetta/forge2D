#pragma once
#include <cstdint>

namespace forge {

class IndexBuffer {
public:
    IndexBuffer(const uint32_t* indices, uint32_t count);
    ~IndexBuffer();

    IndexBuffer(const IndexBuffer&) = delete;
    IndexBuffer& operator=(const IndexBuffer&) = delete;

    void bind() const;
    void unbind() const;
    uint32_t getCount() const { return m_count;}

private:
    uint32_t m_id = 0;
    uint32_t m_count = 0;
}  ;

} // namespace forge