
#include <memory_manager.h>

template<> uint8_t *PagedAllocator<uint256_t>::pool = 0;
template<> uint8_t *PagedAllocator<uint256_t>::poolEnd = 0;
template<> std::vector<uint8_t*> PagedAllocator<uint256_t>::garbageCollection(0);

template<> uint8_t *PagedAllocator<uint160_t>::pool = 0;
template<> uint8_t *PagedAllocator<uint160_t>::poolEnd = 0;
template<> std::vector<uint8_t*> PagedAllocator<uint160_t>::garbageCollection(0);

#define PopularLength(x,y) template<> uint8_t *PagedAllocator<uint##x, sizeof(uint##x)*y>::pool = 0; \
template<> uint8_t *PagedAllocator<uint##x, sizeof(uint##x)*y>::poolEnd = 0; \
template<> std::vector<uint8_t*> PagedAllocator<uint##x, sizeof(uint##x)*y>::garbageCollection(0);

POPULAR_LENGTH_LIST

#undef PopularLength

#define PopularOutput(x,y) template<> uint8_t *PagedAllocator<ptrs##x, sizeof(ptrs##x)*y>::pool = 0; \
template<> uint8_t *PagedAllocator<ptrs##x, sizeof(ptrs##x)*y>::poolEnd = 0; \
template<> std::vector<uint8_t*> PagedAllocator<ptrs##x, sizeof(ptrs##x)*y>::garbageCollection(0);

POPULAR_OUTPUT_LIST

#undef PopularOutput