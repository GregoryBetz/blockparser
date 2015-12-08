
// Very simple blockchain stats

#include <util.h>
#include <common.h>
#include <option.h>
#include <callback.h>


typedef struct InputData {
    uint128_t nbInputs;
    uint128_t nbOutputs;
    uint128_t volume;
} InputData;

typedef GoogMap<
    Hash256,
    InputData *,
    Hash256Hasher,
    Hash256Equal
>::Map TxMap;



void printInputData(InputData *data)
{
    printf("\n");
    #define P(x) (pr128(x).c_str())
        printf("    maxInputCountInTx = %s\n", P(data->nbInputs));
        printf("    nbOutputs = %s\n", P(data->nbOutputs));
        printf("    volume = %s\n", P(data->volume));
        printf("\n");
    #undef P
}


struct TxInputs : public Callback {

    optparse::OptionParser parser;

    
    // global Tx with maximum nb of inputs
    TxMap txMap;

    uint128_t maxInputCountInTx; 
    uint128_t nbOutputs; 
    uint128_t volume;

    // temporary counters for current Tx
    const uint8_t *tmpTxHash;
    uint8_t nullHash[32];

    uint128_t tmpMaxInputCountInTx;     
    uint128_t tmpOutputs;
    uint128_t tmpVolume; 
    

    TxInputs() {
        parser
            .usage("")
            .version("")
            .description("gather simple stats about the Tx inputs")
            .epilog("")
        ;
    }

    virtual const char                   *name() const         { return "txInputs"; }
    virtual const optparse::OptionParser *optionParser() const { return &parser;       }
    virtual bool                       needUpstream() const    { return true;         }

    virtual void aliases(
        std::vector<const char*> &v) const 
    {
        v.push_back("txinputs");
        v.push_back("tx_inputs");
    }



    virtual int init(
        int argc,
        const char *argv[]) 
    {
        // Init the txMap
        static uint8_t empty[kSHA256ByteSize] = { 0x42 };
        static uint64_t sz = 15 * 1000 * 1000;
        txMap.setEmptyKey(empty);
        txMap.resize(sz);

        volume = 0;
        nbOutputs = 0;
        maxInputCountInTx = 0;

        tmpOutputs = 0;
        tmpMaxInputCountInTx = 0;

        return 0;
    }

    virtual void endOutput(
        const uint8_t *p,
        uint64_t      value,
        const uint8_t *txHash,
        uint64_t      outputIndex,
        const uint8_t *outputScript,
        uint64_t      outputScriptSize) 
    {
        tmpVolume += value;
        if (txHash == nullptr) {
            tmpTxHash = nullHash;
        } else {
            tmpTxHash = txHash;
        }
    }

    virtual void wrapup() 
    {
        auto e = txMap.end();
        auto i = txMap.begin();
        while ( i!= e) 
        {            
            showHex(i->first);
            printInputData(i->second);
            ++i;
        }
    }


    virtual void startTX(const uint8_t *p, const uint8_t *hash) 
    { 
            if (tmpMaxInputCountInTx > maxInputCountInTx) 
            {
                maxInputCountInTx = tmpMaxInputCountInTx;
                nbOutputs = tmpOutputs;
                volume = tmpVolume;
                
                InputData *data = new InputData();
                data->nbOutputs = nbOutputs;
                data->nbInputs = maxInputCountInTx;
                data->volume = volume;

                txMap[tmpTxHash] = data;
            }   
            tmpMaxInputCountInTx = 0; 
            tmpOutputs = 0;
            tmpVolume = 0;
    }

    virtual void startInput(const uint8_t *p) 
    { 
        ++tmpMaxInputCountInTx; 
    }

    virtual void startOutput(const uint8_t *p) 
    { 
        ++tmpOutputs;     
    }
};

static TxInputs txInputs;

