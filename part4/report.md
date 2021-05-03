Name: Yao-Fu Yang
NUID: 001378604
Date: 4/9/2020

PART TWO: Experimentation was done with the default values included in the Benchmarker.c file.

Case 1: ParseTheFiles (Non-multi-threaded approach)
 < Tiny Data Directory >
Total time to parse the directory: 0.0052 seconds
Total files crawled: 10 files.
Toal entires in the index: 204

 < Small Data Directory >
Total time to parse the directory: 0.24 seconds
Total files crawled: 24 files.
Toal entires in the index: 20062

 < Large Data Directory >
Total time to parse the directory: 1145.05 seconds
Total files crawled: 52 files.
Toal entires in the index: 639209


Case 2: ParseTheFiles_MT (5-Threads Approach)
 < Tiny Data Directory >
Total time to parse the directory: 0.0022 seconds
Total files crawled: 10 files.
Toal entires in the index: 204

 < Small Data Directory >
Total time to parse the directory: 0.25 seconds
Total files crawled: 24 files.
Toal entires in the index: 20062

 < Large Data Directory >
Total time to parse the directory: 1415.40 seconds
Total files crawled: 52 files
Toal entires in the index: 639209


Case 3: ParseTheFiles_MT (10-Threads Approach)
 < Tiny Data Directory >
Total time to parse the directory: 0.0027 seconds
Total files crawled: 10 files.
Toal entires in the index: 204

 < Small Data Directory >
Total time to parse the directory: 0.29 seconds
Total files crawled: 24 files.
Toal entires in the index: 20062

 < Large Data Directory >
Total time to parse the directory: 1511.67 seconds
Total files crawled: 52 files
Toal entires in the index: 639209


PART THREE: Experiment was done with different number of cores and varying amounts of threads.
Experiment was limited to data_tiny and data_small due to the vast amount of time required to run multiple trials of the large data.

<Trial 1: Running Trials with 2 Processors>
 - Case 1: Tiny_Data Directory
    Time for Non-threaded approach: 0.0024 seconds.
    Time for 5-threaded approach: 0.0046 seconds.
    Time for 10-threaded approach: 0.0061 seconds.

 - Case 2: Small_Data Directory
    Time for Non-threaded approach: 0.56 seconds.
    Time for 5-threaded approach: 0.91 seconds.
    Time for 10-threaded approach: 0.71 seconds.

<Trial 2: Running Trials with 3 Processors>
 - Case 1: Tiny_Data Directory
    Time for Non-threaded approach: 0.0036 seconds.
    Time for 5-threaded approach: 0.0046 seconds.
    Time for 10-threaded approach: 0.0067 seconds.

 - Case 2: Small_Data Directory
    Time for Non-threaded approach: 0.53 seconds.
    Time for 5-threaded approach: 1.02 seconds.
    Time for 10-threaded approach: 0.56 seconds.

<Trial 3: Running Trials with 4 Processors>
 - Case 1: Tiny_Data Directory
    Time for Non-threaded approach: 0.0041 seconds.
    Time for 5-threaded approach: 0.0068 seconds.
    Time for 10-threaded approach: 0.018 seconds.

 - Case 2: Small_Data Directory
    Time for Non-threaded approach: 0.56 seconds.
    Time for 5-threaded approach: 0.69 seconds.
    Time for 10-threaded approach: 0.89 seconds.

<Trial 4: Running Trials with single-threads>
 - Several trials were run between the non-threaded ParseTheFile function and the single-threaded ParseTheFile_MT function. The times between the data_tiny and the data_small were inconsistent and the difference between the two cases is not significant.

 - Based on a single trial using the large dataset, it appears that the single-threaded ParseTheFile_MT took about 150 seconds longer than the non-multi-threaded ParseTheFile function. I suspect that this is because even though we are only passing a single thread to the multi-threaded ParseTheFile function, it is still creating an additional thread that needs to be joined later on (and perhaps the slowness is due to how the threads are implemented in the function). 


