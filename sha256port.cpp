
#include <crypto\sha256.h>
#include <sha256port.h>

void sha256(
    uint8_t       *result,
    const uint8_t *data,
    size_t        len
)
{
    CSHA256 sha256;
    sha256.Write(data, len);
    sha256.Finalize(result);
}
