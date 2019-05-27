
include_directories(
        /usr/local/include
)
link_directories(
        /usr/local/lib
        /usr/local/lib/openssl
)
#aux_source_directory(. DIR_SRC)
#add_subdirectory(util)
add_executable(BlockChain main.cpp block.cpp block.h transaction.cpp transaction.h tx_input.cpp tx_input.h tx_output.cpp tx_output.h proof_of_work.cpp proof_of_work.h util/sha256.cpp util/sha256.h block_chain.cpp block_chain.h wallet.cpp wallet.h util/db_helper.cpp util/db_helper.h util/crypt.cpp util/crypt.h wallet_helper.cpp wallet_helper.h)
#add_executable(BlockChain ${DIR_SRC})
#target_link_libraries (BlockChain gflags ssl crypto util)
target_link_libraries (BlockChain gflags ssl crypto leveldb jansson secp256k1)