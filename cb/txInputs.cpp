
// Very simple blockchain stats

#include <util.h>
#include <common.h>
#include <option.h>
#include <callback.h>

struct TxInputs : public Callback {

    optparse::OptionParser parser;

    
    // global Tx with maximum nb of inputs
    uint8_t txHash[256];

    uint128_t maxInputCountInTx; 
    uint128_t nbOutputs; 
    uint128_t volume;

    // temporary counters for current Tx
    uint8_t tmpTxHash[256];
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
    virtual bool                       needUpstream() const    { return false;         }

    virtual void aliases(
        std::vector<const char*> &v
    ) const {
        v.push_back("txinputs");
        v.push_back("tx_inputs");
    }

    virtual int init(
        int argc,
        const char *argv[]
    ) {
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
        uint64_t      outputScriptSize
    ) {
        tmpVolume += value;
        if (txHash != nullptr)
        {         
            memcpy(tmpTxHash, txHash, 256);
        }
    }

    virtual void wrapup() {
        printf("\n");
        #define P(x) (pr128(x).c_str())

            printf("    The TX found: ");
            showHex(txHash);
            printf("    maxInputCountInTx = %s\n", P(maxInputCountInTx));
            printf("    nbOutputs = %s\n", P(nbOutputs));
            printf("    volume = %s\n", P(volume));
            printf("\n");

        #undef P
    }

    virtual void startTX(const uint8_t *p, const uint8_t *hash) 
    { 
            if (tmpMaxInputCountInTx > maxInputCountInTx) 
            {
                maxInputCountInTx = tmpMaxInputCountInTx;
                nbOutputs = tmpOutputs;
                volume = tmpVolume;
                showHex(tmpTxHash);
                memcpy(txHash, tmpTxHash, 256);
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

