# DanCompiler

A simple compiler in C++ translating custom programming language to a custom assembler. Input is parsed using Flex and Bison.

Created as a final project for university course "Formal Languages and Translation Techniques" during winter semester 2017/2018.

### Notice 1

Everything in the directory labor4 is a copy of materials included by the lecturer with the task description, including interpreter of assembler language and sample tests. Some of the files could have been modified by me for debugging purposes.

### Notice 2

The repository was created somewhere in the middle of the project development due to author's sclerosis.

## How to run

Make sure you have bison and flex installed, then compile the code:

    make

The program takes as an argument path to the source code and, optionally, path to the output file. If it is not specified, output will be directed to the standard output.

    ./dc input_file [output_file]

The program compiles successfully with the following configuration:

* Ubuntu 16.04.3 LTS
* GCC 5.4.1
* Bison 3.0.4
* Flex 2.6.0

## Author

Created by Tomasz Kuczak.
