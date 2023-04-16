#include <string.h>
#include <strings.h>
#include <inttypes.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <openssl/sha.h>
#include <openssl/rsa.h>
#include <openssl/pem.h>

// this is the struct used for the block header 
typedef struct chain_header
{
    uint64_t prev_block;
    uint64_t difficulty;
    uint64_t nonce;
    time_t timestamp;
    uint64_t transactions_no;
} chain_block_header_t;

// the struct used for the transaction block
typedef struct chain_transaction
{
    time_t timestamp;
    double amount;
    uint64_t hash;
    uint8_t signature[1024];
    uint8_t wallet_recipient_key[1024];
    uint8_t wallet_sender_key[1024];
} chain_block_transaction_t;

// struct for each chain. Each chain has one header and several transactions
typedef struct chain {
    chain_block_header_t header;
    chain_block_transaction_t *transaction;
} chain_details;

typedef struct thread_data {
    chain_block_header_t *header;
    uint64_t start_nonce;
    uint64_t end_nonce;
} thread_data_t;

// function used to print the header of the chain
void print_header (const chain_block_header_t *header){
    printf("\n\n\n\n--------- HEADER STARTING ------------\n");
    printf("Value of the prev header: %lu\n", header->prev_block);
    printf("Value of the difficulty: %X\n", (unsigned int)header->difficulty);
    printf("Value of the nonce: %lu\n", header->prev_block);
    printf("Value of the timestamp: %u\n", (unsigned int)header->timestamp);
    printf("Number of transactions: %u\n", (unsigned int)header->transactions_no);
    printf("--------- HEADER COMPLETED ------------\n\n\n\n\n");
}

// function used to print each value of the transaction
void print_transaction(const chain_block_transaction_t *transaction){
    printf("\n\n\n\n--------- TRANSACTION STARTING ------------\n");
    printf("Value of the timestamp: %u\n", (unsigned int)transaction->timestamp);
    printf("Value of the amount: %.2lf\n", transaction->amount);
    printf("Value of the hash: %u\n", (unsigned int)transaction->hash);
    printf("Value of the signature: ");

    for (int x = 0; x < 1024; x++) {
        printf("%X", transaction->signature[x]);
    }

    printf("\n\n");
    printf("Value of the wallet recipient key:");
    
    for (int x = 0; x < 1024; x++){
        printf("%X", (unsigned int)transaction->wallet_recipient_key[x]);
    }

    printf("\n\n");
    printf("Value of the wallet sender key:");
    
    for (int x = 0; x < 1024; x++){
        printf("%X", (unsigned int)transaction->wallet_sender_key[x]);
    }

    printf("\n\n");
    printf("--------- TRANSACTION COMPLETED ------------\n\n\n\n\n");
}

// function used to verify the transaction
int verify_transaction_hash(const chain_block_transaction_t *transaction, RSA *wallet_key) {
    if (wallet_key == NULL) {
        printf("Error: Invalid RSA key.\n");
        exit(1);
    }
    uint8_t decrypted_signature[1024];
    int rsa_size = RSA_size(wallet_key);

    // Decrypt the signature using the sender's public key
    RSA_public_decrypt(rsa_size, (const unsigned char *)(transaction->signature), (unsigned char *)(decrypted_signature), wallet_key, RSA_PKCS1_PADDING);

    // taking only the first 64 bits of the decrypted signature as the hash
    uint64_t hash_64 = 0;
    for (uint8_t i = 0; i < 8; i++) {
        hash_64 ^= ((uint64_t) decrypted_signature[i]) << (i * 8);
    }
    return hash_64 == transaction->hash;
}

RSA * read_public_key(){
    // Path to the PEM file containing the public key.
    char wallet_key_path[] = "../keys/0/public_key.pem";
    
    // Opening the PEM file
    FILE *wallet_key_file = fopen(wallet_key_path, "rb");
    RSA *wallet_key = RSA_new();
    
    // If there is a problem with the file, the programs stops
    if (wallet_key_file == NULL) {
        printf("The wallet key file could not be opened!\n");
        exit(1);
    }

    // Read the RSA public key from the PEM file
    PEM_read_RSA_PUBKEY(wallet_key_file, &wallet_key, NULL, NULL);

    if (wallet_key == NULL) {
        printf("Error reading the public key.\n");
        exit(1);
    }

    // Close the PEM file and return the RSA key pointer.
    fclose(wallet_key_file);
    return wallet_key;
}

int calculate_wallet_balance(RSA *wallet_key, chain_block_transaction_t transaction, double *wallet_balance ) {

    // Check if the wallet is the sender or recipient in the transaction
    // If the wallet is the sender, subtract the transaction amount from the balance
    // If the wallet is the recipient, it adds the transaction amount to the balance
    if (memcmp(&transaction.wallet_sender_key, wallet_key, 1024) == 0) {
       *wallet_balance -= transaction.amount;
    } else if (memcmp(&transaction.wallet_recipient_key, wallet_key, 1024) == 0) {
        *wallet_balance += transaction.amount;
    }
}

int wallet_print_choice()
{
    int user_choice;
    // the loops ends if the user enters 1 or 2, otherwise it will keep going
    while (1)
    {
        // Prompt the user to choose an option
        printf("Enter 1 to print the wallet balance for each transaction and the total.\n");
        printf("Enter 2 to print just the total balance.\n");
        scanf("%d", &user_choice);

        if (user_choice == 1 || user_choice == 2)
        {
            break;
        }
        else
        {   
            // If the user entered an invalid choice, prompt them again and the loop will restart
            printf("Invalid input! Please enter either 1 or 2.\n");
        }
    }
    return user_choice;
}

// function which asks the user if they want to display everything in a default way
int user_choice() {
    int choice;
    printf("Enter 1 to display just the transactions and headers details\n");
    printf("Enter 2 for additional choices:\n");
    scanf("%d", &choice);

    if (choice == 1) {
        printf("Displaying default output.\n");
    } else if (choice == 2) {
        printf("Displaying additional choices.\n");
        // Additional code for handling extra choices
    } else {
        printf("Invalid choice!\n");
        user_choice(); // Recursive call to prompt the user again
    }

    return choice;
}

// Function used to ask if the user wants to see the timestamps around
// (this can be used for testing purposes)
int ask_for_timestamps(){
    int choice;
    printf("Do you want to add timestamps to your program?\n");
    printf("Enter 1 for yes and 2 for no: ");
    scanf("%d", &choice);

    if (choice == 1){
        printf("Timestamps enabled!\n");
        // Your code for adding timestamps here
    } else {
        printf("Default display will be used.\n");
        // Your code for default display here
    }

    return choice;
}


// Function which prints the timestamp just if the user selected 1
void print_timestamp(int timestamp_choice){
    if(timestamp_choice == 1){
        time_t t = time(NULL);
        char buffer[80];
        strftime(buffer, 80, "%Y-%m-%d %H:%M:%S", localtime(&t));
        printf("Timestamp for testing: %s\n\n", buffer);
    }
}

// this fuction generates the hash of the block by taking hte header of the block and passing it
// to the SHA256 which will hash it
uint64_t generate_block_hash(const chain_block_header_t *header) {
    uint64_t hash_64 = 0;
    uint8_t hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;

    SHA256_Init(&sha256);
    SHA256_Update(&sha256, header, sizeof(chain_block_header_t));
    SHA256_Final(hash, &sha256);

    // as asked in task 1 of this assignment, we are taking just the first 64 bits of the hash
    // by applying XOR
    for (int i = 0; i < 8; i++) {
        hash_64 ^= ((uint64_t) hash[i]) << (i * 8);
    }

    return hash_64;
}

// this function is checking if the hash generated is less than the difficulty of the header
// if it is, then it means that valid Proof-of-Work has been found for that specific chain,
// which therefore satysfies the mining requirement 
int mining_check(chain_block_header_t *header){
    
    header->nonce++;
    uint64_t hash = generate_block_hash(header);

    if (hash <= header->difficulty) {
        return 0;
    }
    return 1;
}

// value used across the different threads (if the user decides to user multiples)
// this acts like a boolean, so if this becomes 1, the mining has been completed and all
// threads will stop
int mining_complete = 0;

// this is the function which is passed to the threads function (pthread_create)
void *mine_block_thread(void *args) {
    // from the parameters we destructure it and take the single variables
    thread_data_t *data = (thread_data_t *)args;
    chain_block_header_t *header = data->header;
    uint64_t nonce = data->start_nonce;
    uint64_t end_nonce = data->end_nonce;

    // if the mining is not complete and the nonce is withing its range, 
    // we keep doing the mining check.
    // Once that function returns a 1 and the mining has not already been completed by another thread
    // then we update the mining_complete variable, save the nonce inside the header and show to the user
    // that the mining was successful
    while (mining_complete !== 1 && nonce <= end_nonce) {

        //if the mining was completed
        if (mining_check(header) == 0) {
            if (mining_complete == 0) {
                mining_complete = 1;
                header->nonce = nonce;
                printf("Mining completed!");
                return NULL;
            }
        }

        // if the mining is not completed, then the nonce is increased
        nonce++;
    }   

    return NULL;
}

// this function is mining the header of a chain
// the mining will happen in threads (the number of threads is decided by the user)
void mine_block(chain_block_header_t *header, int threads) {
    uint64_t hash;
    int mining = 1;

    pthread_t thread_ids[threads];
    thread_data_t thread_data[threads]; // the data which will be passed to the function which will ran using threads
    uint64_t nonce_range = ULLONG_MAX / threads;

    // looping for the number of threads
    for (int i = 0; i < threads; i++) {
        // for each thread the header is passed and the start and end of the nonce is calculated
        // (this is done so that two or more threads do not run the same nonce)
        thread_data[i].header = header;
        thread_data[i].start_nonce = (i * nonce_range) + header->nonce;
        thread_data[i].end_nonce = ((i + 1) * nonce_range - 1) + header->nonce;

        // calling the pthread_create function and passing the thread id, the function will be run in the threads
        // and the thread_data which will be passed as params to the function
        pthread_create(&thread_ids[i], NULL, mine_block_thread, &thread_data[i]);
    }

    // this function (pthread_join) will ensure that the main thread waits for each of the worker threads to complete
    // before continuing execution
     for (int i = 0; i < threads; i++) {
        pthread_join(thread_ids[i], NULL);
    }
}

// function used to ask the user for how many threads they want to use for the mining function
int threads_choice() {
    int thread_no;
    printf("Enter the number of threads to use for the mining function\n");
    scanf("%d", &thread_no);

    // if the user inputs 0 or less than 0, then the choice is invalid and the 
    // threads_choice is called in a recursive way
    if (thread_no <= 0 ) {
        printf("Invalid choice!\n");
        threads_choice();
    } 
    return thread_no;
}