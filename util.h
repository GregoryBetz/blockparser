#ifndef __UTIL_H__
    #define __UTIL_H__

    #include <string>
    #include <vector>

    #include <common.h>
    #include <algorithm>
    #include <errlog.h>
    #include <rmd160.h>
    #include <sha256.h>

    #if defined(_WIN64)
        #include <io.h>
        #define lseek64 _lseeki64
        typedef int64_t int128_t;
        typedef uint64_t uint128_t;
    #else
        #include <unistd.h>
        typedef signed int int128_t __attribute__((mode(TI)));
        typedef unsigned int uint128_t __attribute__((mode(TI)));
    #endif

    typedef const uint8_t *Hash160;
    typedef const uint8_t *Hash256;
    struct uint160_t { uint8_t v[kRIPEMD160ByteSize]; };
    struct uint256_t { uint8_t v[   kSHA256ByteSize]; };
    struct Hash160Hasher { uint64_t operator()( const Hash160 &hash160) const { uintptr_t i = reinterpret_cast<uintptr_t>(hash160); const uint64_t *p = reinterpret_cast<const uint64_t*>(i); return p[0]; } };
    struct Hash256Hasher { uint64_t operator()( const Hash256 &hash256) const { uintptr_t i = reinterpret_cast<uintptr_t>(hash256); const uint64_t *p = reinterpret_cast<const uint64_t*>(i); return p[0]; } };

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
        unsigned int mSize;
        std::string mName;
        static CacheableMap* OneLoadedMap;
        size_t mOffset = 0;

        int mapOpen()
        {
            mFd = open(mName.c_str(), O_RDONLY
                #ifdef WIN32
                    | O_BINARY
                #endif // WIN32
                );
            return mFd;
        }

        void mapClose()
        {
            auto r = close(mFd);
            mFd = 0;
            if (r<0) {
                sysErr(
                    "failed to close block chain file %s",
                    mName.c_str()
                    );
            }
        }

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

    struct BlockFile {
        int fd;
        uint64_t size;
        std::string name;
    };

    struct Chunk {

    private:
        size_t size;
        size_t offset;
        mutable uint8_t *data;
        const BlockFile *blockFile;

    public:
        void init(
            const BlockFile *_blockFile,
            size_t _size,
            size_t _offset
        ) {
            data = 0;
            size = _size;
            offset = _offset;
            blockFile = _blockFile;
        }

        const uint8_t *getData() const {
            if(likely(0==data)) {
                auto where = lseek64(blockFile->fd, offset, SEEK_SET);
                if(where!=(signed)offset) {
                    sysErrFatal(
                        "failed to seek into block chain file %s",
                        blockFile->name.c_str()
                    );
            }
            data = (uint8_t*)malloc(size);

                auto sz = read(blockFile->fd, data, size);
                if(sz!=(signed)size) {
                    //fatal("can't read block");
                }
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

        size_t getSize() const                { return size;      }
        size_t getOffset() const              { return offset;    }
        const BlockFile *getBlockFile() const { return blockFile; }

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
            const uint8_t   *_hash,
            const BlockFile *_blockFile,
            size_t          _size,
            Block           *_prev,
            uint64_t        _offset      
        ) {
            chunk = Chunk::alloc();
            chunk->init(_blockFile, _size, _offset);

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
                void setEmptyKey(
                    const Key &
                ) {
                }

                void resize(
                    const size_t &size
                ) {
                    MapBase::reserve(size);
                }
            };
        };

    #elif defined SPARSE_HASH

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
                    ) {
                        this->set_empty_key(empty);
                    }
                };
            };

        #else

            // Slower, uses less RAM
            #include <google/sparse_hash_map>

            template<
                typename Key,
                typename Value,
                typename Hasher,
                typename Equal
            >
            struct GoogMap {

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

    #if defined(DEBUG)
        #define DEBUG_SKIP(type, var, p) type dummy_##var = *(type*)p;
    #else
        #define DEBUG_SKIP(type, var, p)
    #endif

    #define SKIP(type, var, p)       \
        DEBUG_SKIP(type, var, p)     \
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
        const char    *indent = 0,
        bool          showAscii = false
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
        uint8_t       *addrType
    );

    #if defined(DARKCOIN)
        void h9(
                  uint8_t *h9r,
            const uint8_t *buf,
            uint64_t      size
        );
    #endif

    #if defined(PAYCON)
        void h13(
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
                  bool pad = false,
              uint8_t type = 0
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

    std::vector<std::string> getBlockFiles(std::string);

#endif // __UTIL_H__
