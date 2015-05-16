#ifndef __UTIL_H__
    #define __UTIL_H__

    #include <algorithm>
    #include <errlog.h>
#ifdef WIN32
    #include <uint128_t.h>
    #include <io.h>
    #include <time_port.h>
#else
    #include <unistd.h>
#endif // WIN32
    #include <memory_manager.h>

#ifndef WIN32
    typedef signed int int128_t __attribute__((mode(TI)));
    typedef unsigned int uint128_t __attribute__((mode(TI)));
#endif // WIN32

    struct Hash160Hasher { size_t operator()(const Hash160 &hash160) const { uintptr_t i = reinterpret_cast<uintptr_t>(hash160); const size_t *p = reinterpret_cast<const size_t*>(i); return p[0]; } };
    struct TypedHash160Hasher { size_t operator()(const TypedHash160 &hash160) const { uintptr_t i = reinterpret_cast<uintptr_t>(hash160); const size_t *p = reinterpret_cast<const size_t*>(i); return p[0]; } };
    struct Hash256Hasher { size_t operator()(const Hash256 &hash256) const { uintptr_t i = reinterpret_cast<uintptr_t>(hash256); const size_t *p = reinterpret_cast<const size_t*>(i); return p[0]; } };

    struct Hash160Equal {
        bool operator()(
            const Hash160 &ha,
            const Hash160 &hb
        ) const {
            uintptr_t ia = reinterpret_cast<uintptr_t>(ha);
            uintptr_t ib = reinterpret_cast<uintptr_t>(hb);
            const uint64_t *a0 = reinterpret_cast<const uint64_t *>(ia);
            const uint64_t *b0 = reinterpret_cast<const uint64_t *>(ib);
            if(unlikely(a0[0]!=b0[0])) return false;
            if(unlikely(a0[1]!=b0[1])) return false;

            const uint32_t *a1 = reinterpret_cast<const uint32_t *>(ia);
            const uint32_t *b1 = reinterpret_cast<const uint32_t *>(ib);
            if(unlikely(a1[4]!=b1[4])) return false;
            return true;
        }
    };

    struct TypedHash160Equal {
        bool operator()(
            const TypedHash160 &ha,
            const TypedHash160 &hb
        ) const {
            return memcmp(ha, hb, 1 + kRIPEMD160ByteSize) == 0;
        }
    };

    struct Hash256Equal {
        bool operator()(
            const Hash256 &ha,
            const Hash256 &hb
        ) const {
            uintptr_t ia = reinterpret_cast<uintptr_t>(ha);
            uintptr_t ib = reinterpret_cast<uintptr_t>(hb);
            const uint64_t *a = reinterpret_cast<const uint64_t *>(ia);
            const uint64_t *b = reinterpret_cast<const uint64_t *>(ib);
            if(unlikely(a[0]!=b[0])) return false;
            if(unlikely(a[1]!=b[1])) return false;
            if(unlikely(a[2]!=b[2])) return false;
            if(unlikely(a[3]!=b[3])) return false;
            return true;
        }
    };

    struct CacheableMap {
        CacheableMap(){}
        int mFd;
        size_t mSize;
        std::string mName;
        static CacheableMap* OneLoadedMap;
        size_t mOffset = 0;

        int mapRead(uint8_t *_buf, int _size)
        {
            int length = std::min((int)_size, (int)mSize - (int)mOffset);
            if (length > 0)
            {
                memcpy(_buf, getData(mOffset), length);
                mOffset += length;
            }
            return length;
        }

        size_t mapSeek(size_t _offset, int _type)
        {
            if (_type == SEEK_SET)
            {
                mOffset = _offset;
            }
            else if (_type == SEEK_CUR)
            {
                mOffset += _offset;
            }
            else
            {
                warning("unknown seek type: %d", _type);
            }
            if (mOffset <= mSize)
            {
                return mOffset;
            }
            return std::numeric_limits<std::size_t>::max();
        }

        const uint8_t *getData(size_t _offset) {
            if (likely(0 == mData)) {
                #ifdef WIN32
                auto where = _lseeki64(mFd, 0, SEEK_SET);
                #else
                auto where = lseek64(mFd, 0, SEEK_SET);
                #endif // WIN32
                if (where != 0) {
                    sysErrFatal(
                        "failed to seek into block chain file %s",
                        mName.c_str()
                        );
                }
                mData = (uint8_t*)malloc(mSize);

                if(mData == 0)
                {
                    sysErrFatal(
                        "can't allocate memory for map %s",
                       mName.c_str()
                    );
                }

                auto sz = read(mFd, mData, mSize);
                if (sz != (signed)mSize) {
                    sysErrFatal(
                        "can't map block %s",
                        mName.c_str()
                        );
                }
            }
            if (OneLoadedMap != this)
            {
                if (OneLoadedMap != NULL)
                {
                    OneLoadedMap->releaseData();
                }
                OneLoadedMap = this;
            }
            return &mData[_offset];
        }
        void releaseData() {
            if (OneLoadedMap == this)
            {
                OneLoadedMap = NULL;
            }
            free(mData);
            mData = 0;
        }

    private: // dont copy
        CacheableMap(const CacheableMap&);
        CacheableMap& operator=(const CacheableMap&);
        uint8_t *mData = 0;
    };

    struct Chunk {
    private:
        CacheableMap *map;
        size_t size;
        size_t offset;
        mutable uint8_t *data;

    public:
        void init(
            CacheableMap *_map,
            size_t _size,
            size_t _offset
        ) {
            data = 0;
            map = _map;
            size = _size;
            offset = _offset;
        }

        void lazyInit() const
        {
            auto where = map->mapSeek(offset, SEEK_SET);
            if (where != offset) {
                sysErrFatal(
                    "failed to seek into block chain file %s",
                    map->mName.c_str()
                    );
            }
            data = (uint8_t*)malloc(size);

            auto sz = map->mapRead(data, size);
            if (sz != (signed)size) {
                //fatal("can't map block");
            }
        }

        const uint8_t *getData() const {
            if (likely(0 == data)) {
                lazyInit();
            }
            return data;
        }

        void releaseData() const {
            free(data);
            data = 0;
        }

        size_t getSize() const    { return size;   }
        size_t getOffset() const  { return offset; }
        CacheableMap *getMap()    { return map; }

        static Chunk *alloc() {
            return (Chunk*)PagedAllocator<Chunk>::alloc();
        }
    };
    
    struct TXChunk {
    private: // dont copy
        TXChunk(const TXChunk&);
        TXChunk& operator=(const TXChunk&);
    public:
        TXChunk() {}
        uint8_t** mRawData;
        
        static TXChunk *alloc() {
            return (TXChunk*)PagedAllocator<TXChunk>::alloc();
        }
    };

    struct Block {

        Chunk         *chunk;
        const uint8_t *hash;
        int64_t       height;
        Block         *prev;
        Block         *next;

        void init(
            const uint8_t *_hash,
            CacheableMap *_map,
            size_t         _size,
            Block         *_prev,
            uint64_t       _offset      
        ) {
            chunk = Chunk::alloc();
            chunk->init(_map, _size, (size_t)_offset);

            hash = _hash;
            height = -1;
            prev = _prev;
            next = 0;
        }

        static Block *alloc() {
            return (Block*)PagedAllocator<Block>::alloc();
        }
    };

    #if defined DENSE_HASH

        // Faster, uses more RAM
        #include <google/dense_hash_map>

        template<
            typename Key,
            typename Value,
            typename Hasher,
            typename Equal
        >
        struct HashMap {

            typedef google::dense_hash_map<
                Key,
                Value,
                Hasher,
                Equal
            > MapBase;

            struct Map:public MapBase {
                void setEmptyKey(
                    const Key &empty
                )
                {
                    this->set_empty_key(empty);
                }
            };
        };

    #elif defined SPARSE_HASH

        // Slower, uses less RAM
        #include <google/sparse_hash_map>

        template<
            typename Key,
            typename Value,
            typename Hasher,
            typename Equal
        >
        struct HashMap {

            typedef google::sparse_hash_map<
                Key,
                Value,
                Hasher,
                Equal
            > MapBase;

            struct Map:public MapBase {

                void setEmptyKey(
                    const Key &empty
                ) {
                }
            };
        };

    #else
    
        #include <unordered_map>
        template<
            typename Key,
            typename Value,
            typename Hasher,
            typename Equal
        >
        struct HashMap {

            typedef std::unordered_map<
                Key,
                Value,
                Hasher,
                Equal
            > MapBase;

            struct Map:public MapBase {
                void setEmptyKey(const Key &){}
                void set_deleted_key(const Key &){}
                void resize(uint64_t) {}
            };
        };

    #endif

    #define SKIP(type, var, p)       \
        p += sizeof(type)            \

    #define LOAD(type, var, p)       \
        type var = *(type*)p;        \
        p += sizeof(type)            \

    #define LOAD_VARINT(var, p)      \
        uint64_t var = loadVarInt(p) \

    static inline uint64_t loadVarInt(
        const uint8_t *&p
    ) {
        uint64_t r = *(p++);
        if(likely(r<0xFD))  {                       return r; }
        if(likely(0xFD==r)) { LOAD(uint16_t, v, p); return v; }
        if(likely(0xFE==r)) { LOAD(uint32_t, v, p); return v; }
                              LOAD(uint64_t, v, p); return v;
    }

    double usecs();

    void toHex(
              uint8_t *dst,
        const uint8_t *src,
        size_t        size = kSHA256ByteSize,
        bool          rev = true
    );

    void showHex(
        const uint8_t *src,
        size_t        size = kSHA256ByteSize,
        bool          rev = true
    );

    uint8_t fromHexDigit(
        uint8_t h,
        bool abortOnErr = true
    );

    bool fromHex(
              uint8_t *dst,
        const uint8_t *src,
        size_t        dstSize = kSHA256ByteSize,
        bool          rev = true,
        bool          abortOnErr = true
    );

    void showScript(
        const uint8_t *p,
        size_t        scriptSize,
        const char    *header = 0,
        const char    *indent = 0
    );

    bool compressPublicKey(
              uint8_t *result,
        const uint8_t *decompressedKey
    );

    bool decompressPublicKey(
              uint8_t *result,
        const uint8_t *compressedKey
    );

    int solveOutputScript(
              uint8_t *pubKeyHash,
        const uint8_t *script,
        uint64_t      scriptSize,
        uint8_t       *type
    );

    #if defined(DARKCOIN)
        void h9(
                  uint8_t *h9r,
            const uint8_t *buf,
            uint64_t      size
        );
    #endif

    #if defined(CLAM) || defined(JUMBUCKS)
        void scrypt(
                  uint8_t *h9r,
            const uint8_t *buf,
            uint64_t      size
        );
    #endif

    static inline void sha256Twice(
              uint8_t *sha,
        const uint8_t *buf,
        uint64_t      size
    ) {
        sha256(sha, buf, (size_t)size);
        sha256(sha, sha, kSHA256ByteSize);
    }

    extern const uint8_t hexDigits[];
    extern const uint8_t b58Digits[];

    uint8_t fromB58Digit(
        uint8_t digit,
        bool abortOnErr = true
    );

    void hash160ToAddr(
              uint8_t *addr,
        const uint8_t *hash160,

        #if defined(PROTOSHARES)
            uint8_t type = 56
        #endif

        #if defined(DARKCOIN)
            uint8_t type = 48 + 28
        #endif

        #if defined(LITECOIN)
            uint8_t type = 48
        #endif

        #if defined(BITCOIN)
            uint8_t type = 0
        #endif
        
        #if defined(FEDORACOIN)
            uint8_t type = 33
        #endif

        #if defined(PEERCOIN)
            uint8_t type = 48 + 7
        #endif

        #if defined(CLAM)
            uint8_t type = 137
        #endif

        #if defined(JUMBUCKS)
            uint8_t type = 43
        #endif

        #if defined(DOGECOIN)
            uint8_t type = 30
        #endif

        #if defined(MYRIADCOIN)
            uint8_t type = 50
        #endif

        #if defined(UNOBTANIUM)
            uint8_t type = 130
        #endif
    );

    static inline double satoshisToNormaForm(
        uint64_t satoshis
    ) {
        return satoshis*
            #if defined(PEERCOIN)
                1e-6
            #else
                1e-8
            #endif
        ;
    }

    bool addrToHash160(
              uint8_t *hash160,
        const uint8_t *addr,
              uint8_t &type,
                 bool checkHash = false
    );

    bool guessHash160(
              uint8_t *hash160,
        const uint8_t *addr
    );

    const uint8_t *loadKeyHash(
        const uint8_t *hexHash = 0,
        bool verbose = false
    );

    void loadKeyList(
        std::vector<uint160_t> &result,
        const char *str,
        bool verbose = false
    );

    void loadHash256List(
        std::vector<uint256_t> &result,
        const char *str,
        bool verbose = false
    );

    std::string pr128(
        const uint128_t &y
    );

    void showFullAddr(
        const Hash160 &addr,
        bool both = false
    );

    uint64_t getBaseReward(
        uint64_t h
    );

    void canonicalHexDump(
        const uint8_t *p,
               size_t size,
           const char *indent
    );

    void showScriptInfo(
        const uint8_t *outputScript,
        uint64_t      outputScriptSize,
        const uint8_t *indent
    );

    void writeEscapedBinaryBufferRev(
        FILE          *f,
        const uint8_t *p,
        size_t        n
    );

    void writeEscapedBinaryBuffer(
        FILE          *f,
        const uint8_t *p,
        size_t        n
    );

    static inline void printEscapedBinaryBufferRev(
        const uint8_t *p,
        size_t        n
    ) {
        writeEscapedBinaryBufferRev(stdout, p, n);
    }

    static inline void printEscapedBinaryBuffer(
        const uint8_t *p,
        size_t        n
    ) {
        writeEscapedBinaryBuffer(stdout, p, n);
    }

    const char *getInterestingAddr();

#endif // __UTIL_H__
