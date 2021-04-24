# Multithreading-Demo
A multithreading demo project for Operating Systems course CSI 500 at UAlbany. It is a demonstration of how multithreading can speed up an application. This was a collaborative project between myself and Jinyu T., students then taking the OS course.

The application simply capitalizes all letters and replaces spaces with a character given by the user. The process was separated into four threads working together  using queues as buffers in the shared memory and semaphores to avoid race conditions. There was a noticable improvement in the execution speed when the buffer size was increased keeping the threads busy. For an input file with 1 million lines of lorem ipsum text repeated every 1k lines, here is a table showing the effect of increasing the buffer size on the execution time.

| Buffer Size | Execution Time (in seconds) |
| ----------: | --------------------------: |
|           1 |                       48.07 |
|           2 |                       53.02 |
|           5 |                       39.50 |
|          10 |                       32.17 |
|         100 |                       20.87 |
|        1000 |                       10.18 |
|       10000 |                        8.58 |
|      100000 |                        5.68 |

See docs/effect_of_buffer_size.txt for output details.

To run the applicaion

    > proc INPUT_FILE [--demo] --buffersize N

    --demo
        An optional demo mode which shows lines processed for each thread with status of its queue.

    --buffersize N
        Sets the buffer size of each of the processing threads to N.
