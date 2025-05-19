# Scheduler-Algorithm
Simulated FCFS and priority-based schedulers.

Part 1:
examples compilation scripts: test_1.txt, test_2.txt

manual compilation: 
    Alternatively, you can compile the source file manually using the following command:
        g++ interrupts_101280101_101219454.c -o sim
        ./sim input_data_n.txt FCFS
    - n = 1, 2, ..., 10
    - ./sim input_data_n.txt FCFS calls your simulator for FCFS
    Similarly:
        ./sim input_data_1.txt EP calls your simulator for EP

Part 2:
    - n = A, B, C, E
    Use the following command to compile the code:
        gcc -o code Part2_n_101280101_101219454.c -lpthread
    Execution: Run the compiled program using:
        ./code
