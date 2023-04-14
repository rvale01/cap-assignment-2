# CAP Chain Generator

Here is the repo for the Communications and Protocols binary block chain generator.

## single_block
Run with the command ./chain_generator

Make this executable by running chmod +x chain_generator

This will generate a single block, and two key pairs for transactions.

The ./print_chain program can be used to help debug, this will print out key chain info.

You can run ./print_chain > chain.txt to save the chain details into a text file.

## complex_chain

You can run this using the following command:
./complex_chain_generator -wallets 5 -blocks 15 -max-transactions 8

## mining_chain

To run this:
 ./mining_chain -difficulty easy

 Difficulty can be set to easy, medium, hard, veryhard, extreme.
