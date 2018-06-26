#include <IndexManager/BPlusTree.h>

namespace IndexManager {

namespace BPlusTree {

namespace BM = BufferManager;

void Node::initFromBlock(BM::PtrBlock blk,
                         const std::pair<ValueType, size_t> &info) {
    Ptr ptr;
    Value value;
    uint32_t numPtrs, numVals;

    children.clear();
    values.clear();

    blk->resetPos(0);
    blk->read(reinterpret_cast<char *>(&numPtrs), sizeof(uint32_t));
    blk->read(reinterpret_cast<char *>(&numVals), sizeof(uint32_t));
    blk->read(reinterpret_cast<char *>(&parent), sizeof(Ptr));
    for (auto i = 0; i < numPtrs; i++) {
        blk->read(reinterpret_cast<char *>(&ptr), sizeof(Ptr));
        children.push_back(ptr);
    }
    for (auto i = 0; i < numVals; i++) {
        value.type = info.first;
        value.charCnt = info.second;
        blk->read(value.val(), value.size());
    }
}

} // namespace BPlusTree

} // namespace IndexManager