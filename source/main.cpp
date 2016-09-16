#include <cassert>
#include <cstdint>
#include <cstring>
#include <memory>
#include <map>
#include <bitset>

#include "../distorm/include/distorm.h"

typedef size_t addr_t;

struct cpu_t {
    uint16_t ip_;
    union { uint16_t ax_; struct { uint8_t al_, ah_; }; };
    union { uint16_t bx_; struct { uint8_t bl_, bh_; }; };
    union { uint16_t cx_; struct { uint8_t cl_, ch_; }; };
    union { uint16_t dx_; struct { uint8_t dl_, dh_; }; };
    uint16_t si_, di_, bp_, sp_;
    uint16_t cs_, ds_, es_, ss_;
    uint16_t status_;
};

struct memory_t {

    memory_t() {
        memory_.fill(0xCC);
    }

    void memset(addr_t dst, uint8_t data, addr_t size) {
        assert(dst + size < SIZE_);
        ::memset(memory_.data() + dst, data, size);
    }

    void memcpy(addr_t dst, void * src, addr_t size) {
        assert(dst + size < SIZE_);
        ::memcpy(memory_.data() + dst, src, size);
    }

    template <typename type_t = uint8_t>
    type_t * get(size_t offset = 0) {
        assert(offset + sizeof(type_t) < SIZE_);
        return reinterpret_cast<type_t*>(memory_.data() + offset);
    }

    template <typename type_t = uint8_t>
    const type_t * get(addr_t offset = 0) const {
        assert(offset + sizeof(type_t) < SIZE_);
        return reinterpret_cast<const type_t*>(memory_.data() + offset);
    }

    void write(addr_t addr, uint8_t value) {
        if (code_map_[addr]) {
            assert(map_);
            map_->invalidate(addr);
        }
        memory_[addr] = value;
        code_map_.set(addr, false);
    }

    size_t size() const {
        return SIZE_;
    }

    // pointer to the blocks map structure
    block_map_t * map_;

protected:

    static const size_t SIZE_ = 1024 * 64;

    // this structure marks which areas are covered by blocks so we know
    // that writes to these areas should invalidate them.
    std::bitset<SIZE_> code_map_;

    // this is the raw memory bytes
    std::array<uint8_t, SIZE_> memory_;
};

struct block_t {
    size_t addr_;
    size_t size_;

    // this points to the previously taken branch target for this block.
    block_t * next_;
};

struct block_generator_t {

    block_t * generate(addr_t addr) {
        _CodeInfo info;
        info.code = memory_->get(addr);
        info.dt = Decode16Bits;
        info.codeLen = 8;
        info.features = DF_MAXIMUM_ADDR16 | DF_STOP_ON_FLOW_CONTROL;
        info.nextOffset = 0;
        info.codeOffset = 0;
        uint32_t count = 0;
        distorm_decompose(&info, inst_.data(), inst_.size(), &count);
        return nullptr;
    }

    std::array<_DInst, 512> inst_;
    memory_t * memory_;
};

struct block_map_t {

    block_map_t()
        : memory_(nullptr)
        , generator_(nullptr)
    {
    }

    void invalidate(addr_t addr) {
        // invalidate and remove block
        auto itt = map_.upper_bound(addr);
        // blah blah
    }

    block_t * lookup(addr_t addr) {
        auto itt = map_.find(addr);
        if (itt == map_.end()) {
            assert(generator_);
            block_t * block = generator_->generate(addr);
            assert(block);
            map_[addr] = block;
            return block;
        }
        else {
            return itt->second;
        }
    }

    block_generator_t * generator_;
    memory_t * memory_;

protected:
    std::map<size_t, block_t*> map_;
};

int main(const int argc, const char * args []) {

    extern std::array<uint8_t, 512> bootsector;

    memory_t mem;
    block_generator_t generator;
    block_map_t map;

    mem.memcpy(0x7c00, bootsector.data(), bootsector.size());
    mem.map_ = &map;

    map.memory_ = &mem;
    map.generator_ = &generator;
    map.lookup(0x7c00);

    return 0;
}
