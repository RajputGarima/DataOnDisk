# DataOnDisk

Given a buffer manager that acts like a mini OS and controls the storage and retrieval of a file by providing some APIs for each action. <br />
Each file is stored in memory as pages of size PAGE_CONTENT_SIZE(a constant in constants.h). At any given time, you can have at max BUFFER_SIZE(a constant in constants.h) number of pages in memory. <br />


The aim of the project is to work on very large files, consisting of integers, that can't be stored completely in the buffer and perform 3 operations on these files.
1. Given a sorted file and an integer, find the page number and offset of the integer using **Binary Search.**
2. Given a sorted file and an integer, perform **Sorted Insertion** in the file.
3. Given a file, sort it using **External Sort-Merge Algorithm.**
