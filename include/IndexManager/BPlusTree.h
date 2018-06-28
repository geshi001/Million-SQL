#pragma once
#include <BufferManager/Block.h>
#include <DataType.h>
#include <cstdint>
#include <vector>

namespace IM {

namespace BPlusTree {

using Ptr = uint32_t;
constexpr Ptr NullPtr = 0U;

struct Node {
    Ptr parent;
    std::vector<Ptr> children;
    std::vector<Ptr> values;
    Node(int fanout)
        : values(fanout - 1), parent(NullPtr), children(fanout, NullPtr) {}
    void initFromBlock(BM::PtrBlock, const std::pair<ValueType, size_t> &);
};

struct Tree {
    Node root;
    Tree(int fanout) : root(fanout) {}
};

} // namespace BPlusTree

} // namespace IM