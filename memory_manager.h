#ifndef __MEMORY_MANAGER_H__
    #define __MEMORY_MANAGER_H__

    #include <rmd160port.h>
    #include <sha256port.h>
    #include <common.h>
    #include <string>
    #include <vector>
    #include <string.h>

    typedef const uint8_t *Hash160;
    typedef const uint8_t *TypedHash160;
    typedef const uint8_t *Hash256;
    struct uint160_t { uint8_t v[  kRIPEMD160ByteSize]; };
    struct uint168_t { uint8_t v[1+kRIPEMD160ByteSize]; };
    struct uint256_t { uint8_t v[     kSHA256ByteSize]; };

    template<
        typename T,
        size_t   kPageSize = 16384
    >
    struct PagedAllocator {

        static std::vector<uint8_t*> garbageCollection;
        static uint8_t *pool;
        static uint8_t *poolEnd;
        enum { kPageByteSize = sizeof(T)*kPageSize };

        static uint8_t *alloc() {
            uint8_t *result = NULL;
            if(likely(garbageCollection.empty()))
            {
                if(unlikely(poolEnd<=pool)) {
                    pool = (uint8_t*)malloc(kPageByteSize);
                    memset(pool, 0, kPageByteSize);
                    poolEnd = pool + kPageByteSize;
                }
                result = pool;
                pool += sizeof(T);
            }
            else
            {
                result = garbageCollection.back();
                garbageCollection.pop_back();
            }
            return result;
        }
        static void free(T* obj) { garbageCollection.push_back((uint8_t*)obj); }
        static void free(const uint8_t* obj) { garbageCollection.push_back((uint8_t*)obj); }
    };

    static inline uint8_t *allocHash256() { return PagedAllocator<uint256_t>::alloc(); }
    static inline uint8_t *allocHash160() { return PagedAllocator<uint160_t>::alloc(); }

    #define POPULAR_LENGTH_LIST  \
        PopularLength( 10,   30) \
        PopularLength( 14,   30) \
        PopularLength( 15,  200) \
        PopularLength( 32, 1000) \
        PopularLength( 34, 2000) \
        PopularLength( 42,  200) \
        PopularLength( 44, 1000) \
        PopularLength( 46,  200) \
        PopularLength( 51,  100) \
        PopularLength( 76, 2000) \
        PopularLength( 80,  200) \
        PopularLength(114,   30) \
        PopularLength(144,  300) \
        PopularLength(210, 1000)

    #define PopularLength(x,y) Length##x,
        enum PopularTXLengths {
            POPULAR_LENGTH_LIST
            LengthOther
        };
    #undef PopularLength

    #define PopularLength(x,y) x,
        const int EnumToLength[] = {
            POPULAR_LENGTH_LIST
            -1
        };
    #undef PopularLength

    #define PopularLength(x,y) struct uint##x { uint8_t v[x+1]; };
    POPULAR_LENGTH_LIST
    #undef PopularLength

    #define PopularLength(x,y) static inline uint8_t *allocNBytes##x() { return PagedAllocator<uint##x, y>::alloc(); }
    POPULAR_LENGTH_LIST
    #undef PopularLength

    typedef uint8_t * (*AllocBytesFunction)();

    #define PopularLength(x,y) &allocNBytes##x,
    const AllocBytesFunction allocBytesFuncs[] = {
        POPULAR_LENGTH_LIST
        0
    };
    #undef PopularLength

    #define PopularLength(x,y) static inline void freeNBytes##x(const uint8_t* obj) { return PagedAllocator<uint##x, y>::free(obj); }
    POPULAR_LENGTH_LIST
    #undef PopularLength

    typedef void (*FreeBytesFunction)(const uint8_t*);
    #define PopularLength(x,y) &freeNBytes##x,
    const FreeBytesFunction freeBytesFuncs[] = {
        POPULAR_LENGTH_LIST
        0
    };
    #undef PopularLength

    static inline uint8_t *allocTX(int _size)
    {
        for(int i = 0; i < LengthOther; ++i)
        {
            if(EnumToLength[i] == _size)
            {
                uint8_t* result = allocBytesFuncs[i]();
                result[0] = i;
                return result + 1;
            }
        }
        uint8_t* result = (uint8_t*)malloc(_size + 1);
        result[0] = LengthOther;
        return result + 1;
    }
    static inline void freeTX(uint8_t* tx)
    {
        int lengthIndex = *(tx-1);
        if(lengthIndex >=0 && lengthIndex < LengthOther)
        {
            freeBytesFuncs[lengthIndex](tx-1);
            return;
        }
        free(tx-1);
    }

    #define POPULAR_OUTPUT_LIST \
        PopularOutput( 1, 1000) \
        PopularOutput( 2, 1000) \
        PopularOutput( 3, 1000) \
        PopularOutput( 4, 1000) \
        PopularOutput( 5, 1000) \
        PopularOutput( 6, 1000) \
        PopularOutput( 7, 1000) \
        PopularOutput( 8, 1000) \
        PopularOutput( 9, 1000) \
        PopularOutput(10, 1000) \
        PopularOutput(11, 1000) \
        PopularOutput(12, 1000)

    #define PopularOutput(x,y) OutputNumber##x,
    enum PopularOutputNumbers {
        POPULAR_OUTPUT_LIST
        OutputNumberCount
    };
    #undef PopularOutput

    #define PopularOutput(x,y) struct ptrs##x { uint8_t* v[x+1]; };
    POPULAR_OUTPUT_LIST
    #undef PopularOutput

    #define PopularOutput(x,y) static inline uint8_t** allocNPtrs##x() { return (uint8_t**)PagedAllocator<ptrs##x, y>::alloc(); }
    POPULAR_OUTPUT_LIST
    #undef PopularOutput

    typedef uint8_t** (*AllocPtrsFunction)();
    #define PopularOutput(x,y) &allocNPtrs##x,
    const AllocPtrsFunction allocPtrsFuncs[] = {
        POPULAR_OUTPUT_LIST
        0
    };
    #undef PopularOutput

    #define PopularOutput(x,y) static inline void freeNPtrs##x(uint8_t** obj) { return PagedAllocator<ptrs##x, y>::free((ptrs##x *)obj); }
    POPULAR_OUTPUT_LIST
    #undef PopularOutput

    typedef void (*FreePtrsFunction)(uint8_t**);
    #define PopularOutput(x,y) &freeNPtrs##x,
    const FreePtrsFunction freePtrsFuncs[] = {
        POPULAR_OUTPUT_LIST
        0
    };
    #undef PopularOutput

    static inline uint8_t** allocPtrs(int _size)
    {
        if(_size <= OutputNumberCount)
        {
            // zero based: allocPtrsFuncs[0] is the allocator for 1
            uint8_t** result = allocPtrsFuncs[_size-1]();
            result[0] = reinterpret_cast<uint8_t*>(_size);
            return result + 1;
        }
        uint8_t** result = (uint8_t**)malloc(sizeof(uint8_t*) * (_size + 1));
        result[0] = reinterpret_cast<uint8_t*>(_size);
        return result + 1;
    }
    static inline void freePtrs(uint8_t** ptrs)
    {
        int length = reinterpret_cast<intptr_t>(*(ptrs - 1));
        if(length >=0 && length <= OutputNumberCount)
        {
            freePtrsFuncs[length-1](ptrs-1);
            return;
        }
        free(ptrs-1);
    }

    static inline int countNonNullPtrs(uint8_t** ptrs)
    {
        int length = reinterpret_cast<intptr_t>(*(ptrs - 1));
        int counter = 0;
        for(int i = 0; i < length; ++i)
        {
            if(ptrs[i] != 0)
            {
                ++counter;
            }
        }
        return counter;
    }

#endif // __MEMORY_MANAGER_H__
