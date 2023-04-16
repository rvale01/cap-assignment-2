<h1>ASSIGNMENT 2</h1>

This folder contains all the tasks (1 and 2) required in assignment 2 of this module.

# How to run it
To run the code, you can just run the following commands in this directory:

<code>make</code>
<code>./main</code>

# Task 1
Task 1 requred just a quick update of a function, by just using the XOR to get the first 64bits of the hash

# Task 2
Task 2 was a bit more complex than task 1. It consists of creating the mining process. The mining uses threads in this case. For each thread, the mining is checked by hashing the whole header, if the hashed value is less than the difficulty, then the mining is completed and the mining will stop for this thread and for the others threads as well (this happens because all threads read a shared variable: mining_complete).

More specific details of how the threading is achieved can be found in the code.

# Extras
In this assignment, two extra things were added than what was asked in the assignment. The whole mining process works in threads, plus the user can also choose how many threads to run.

As shown in the comments inside the code, the pthread_create and pthread_join are used to manage how the functions work in threads

Also, in order for the threads to work, there is a range of nonces which is created. Each thread will run within that range. Each thread finish running either if the range of the nonce is over or if mining process is completed.