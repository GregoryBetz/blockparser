
// Very simple blockchain stats

#include <util.h>
#include <common.h>
#include <option.h>
#include <callback.h>

struct TxInputs : public Callback {

    optparse::OptionParser parser;

    

    uint128_t maxInputCountInTx; // global max
    uint128_t tmpMaxInputCountInTx; // temporary counter for current Tx
    
    uint128_t nbOutputs;
    

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
        nbOutputs = 0;

        maxInputCountInTx = 0;

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
        // volume += value;
    }

    virtual void wrapup() {
        printf("\n");
        #define P(x) (pr128(x).c_str())

            printf("    maxInputCountInTx = %s\n", P(maxInputCountInTx));
            printf("    nbOutputs = %s\n", P(nbOutputs));
            printf("\n");

        #undef P
    }

    virtual void         startTX(const uint8_t *p, const uint8_t *hash) 
    { 
            if (tmpMaxInputCountInTx > maxInputCountInTx) 
            {
                maxInputCountInTx = tmpMaxInputCountInTx;
            }
            tmpMaxInputCountInTx = 0; 
    }
    virtual void      startInput(const uint8_t *p                     ) 
    { 
        ++tmpMaxInputCountInTx; 
    }
    virtual void     startOutput(const uint8_t *p                     ) { ++nbOutputs;     }
};

static TxInputs txInputs;

