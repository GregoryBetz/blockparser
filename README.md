blockparser
===========
A fairly fast, quick and dirty bitcoin whole blockchain parser.

Credits
-------
Written by znort987@yahoo.com

If you find this useful: 1ZnortsoStC1zSTXbW6CUtkvqew8czMMG

Ported to Windows by zgalli@email.com

Build it on Linux
-----------------

1.  Turn your x86-64 Ubuntu box on

2.  Make sure you have an up to date satoshi client blockchain in ~/.bitcoin

3.  Run this:
```
sudo apt-get install libssl-dev build-essential g++-4.4 libboost-all-dev libsparsehash-dev git-core perl
git clone git://github.com/znort987/blockparser.git
cd blockparser
make
```

Problems: 
anyone have this problem "Segmentation Fault" follow this to make a swap file:
http://askubuntu.com/questions/178712/how-to-increase-swap-space
Tested on ( Ubuntu 12.04, 250 ssd, 16gb ram, 9GB Swap )

Build it on Windows
-------------------
1.  Open blockparser.sln with Microsoft Visual Studio.

2.  Set environment variable BLOCKCHAIN_DIR to your satoshi client blockchain folder.
    E.g. if your blockchain folder is e:\Coindata\Bitcoin\ then:
    go to Properties->Debugging->Environment and type: BLOCKCHAIN_DIR=E:\Coindata\Bitcoin

3.  Compile.

Try it on Linux
---------------
* Compute simple blockchain stats, full chain parse (15 minutes with 34 GB blockchain in May 2015):
```
./parser simpleStats
```

* Extract all transactions for popular address 1dice6wBxymYi3t94heUAG6MpG5eceLG1 (28 minutes):
```
./parser transactions 06f1b66fa14429389cbffa656966993eab656f37
```

* Compute the closure of an address, that is the list of addresses that provably belong to the same person.
  (This function is turned off in Windows because of excessive RAM usage):
```
./parser closure 06f1b66fa14429389cbffa656966993eab656f37
```

* Compute and print the balance for the top 1000 keys (33 minutes):
```
./parser allBalances -l 1000 -w 1000 > allBalances.txt
```

* See how much of the BTC 10K pizza tainted each of the TX in the chain
```
./parser taint >pizzaTaint.txt
```

* See all the block rewards and fees:
```
./parser rewards >rewards.txt
```

* See a greatly detailed dump of the pizza transaction
```
./parser show
```

Try it on Windows
-----------------
* Open a command line. Set environment variable BLOCKCHAIN_DIR to your satoshi client blockchain folder.
  E.g. if your blockchain folder is e:\Coindata\Bitcoin\ type:
```
set BLOCKCHAIN_DIR=e:\Coindata\Bitcoin\
```
* Run blockparser.exe with the same arguments as above, e.g.:
```
blockparser.exe simpleStats
```

Caveats
-------
* According to choosen argument it needs quite a bit of RAM to work. As of May 2015 with a 34 GB blockchain:
  * simpleStats uses 300 MB.
  * allBalances uses 2.5 GB.
  * closure uses too much memory, 8 GB in my machine is not enough, so I switched it off in Windows build.

* The code isn't particularly clean or well architected. It was just a quick way for me to learn
  about bitcoin. There isnt much in the way of comments either.

* OTOH, it is fairly simple, short, and efficient. If you want to understand how the blockchain
  data structure works, the code in parser.cpp is a solid way to start.

* blockparser uses mmap() extensively. There has been report that it does not play well with
  encrypted partitions. Solution: move your blockchain to a normal disk. That's likely to make
  your bitcoin install a lot more efficient anyways.

Hacking the code
----------------
* parser.cpp contains a generic parser that mmaps the blockchain, parses it and calls
  "user-defined" callbacks as it hits interesting bits of information.

* util.cpp contains a grab-bag of useful bitcoin related routines. Interesting examples include:
  * showScript
  * getBaseReward
  * solveOutputScript
  * decompressPublicKey

* cb/allBalances.cpp -- code to all balance of all addresses
* cb/closure.cpp -- code to compute the transitive closure of an address
* cb/dumpTX.cpp -- code to display a transaction in very great detail
* cb/help.cpp -- code to dump detailed help for all other commands
* cb/pristine.cpp -- code to show all "pristine" (i.e. unspent) blocks
* cb/rewards.cpp -- code to show all block rewards (including fees)
* cb/simpleStats.cpp -- code to compute simple stats.
* cb/sql.cpp -- code to product an SQL dump of the blockchain
* cb/taint.cpp -- code to compute the taint from a given TX to all TXs
* cb/transactions.cpp -- code to extract all transactions pertaining to an address

* You can very easily add your own custom command. You can use the existing callbacks in
  directory ./cb/ as a template to build your own:
  * cp cb/allBalances.cpp cb/myExtractor.cpp
  * Add to Makefile
  * Hack away
  * Recompile
  * Run

* You can also read the file callback.h (the base class from which you derive to implement your
  own new commands). It has been heavily commented and should provide a good basis to pick what
  to overload to achieve your goal.

* The code makes heavy use of the google dense hash maps. You can switch it to use sparse hash
  maps (see Makefile, search for: DENSE, undef it). Sparse hash maps are slower but save quite a
  bit of RAM.

* Windows version works well only with sparse hash.

Thanks
------
* Thanks to the Bitcoin Core developers for base58, ripemd160 and sha256 implementation
* Thanks to Jason Lee for uint128_t
* Thanks to johnnyw for gettimeofday.c
* Thanks to stathis for precompiled openssl library
* Thanks to Toni Ronkko for dirent API

License
-------
Code is in the public domain.
