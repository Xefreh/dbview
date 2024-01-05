# DBView

DBView is a command-line application designed to manage employee details by saving binary data to a file.

## Prerequisites

Before you begin, ensure you have met the following requirements:
- You have a Linux or macOS machine. (Windows steps may vary)
- You have installed GCC (GNU Compiler Collection).
- You have basic knowledge of using terminal and Makefile.

## Compiling the Project

DBView uses a Makefile for easy compilation and running. To compile the project, follow these steps:

1. Navigate to the project directory where the Makefile is located.
2. Run the following command to compile the project:
   ```
   make
   ```

## Running the Program

After compiling the project, you can run DBView with different commands for managing employee details. The Makefile includes a 'run' target for convenience, which executes a sample command to create a new database and add an employee:

1. To run the application with the sample commands specified in the Makefile:
   ```
   make run
   ```
   This will execute the following:
   - Create a new database file `mynewdb.db`.
   - Add an employee with the details "Timmy H., 123 Sheshire Ln., 120".

2. To run the application with custom commands, use the following format:
   ```
   ./bin/dbview [options]
   ```
   Replace `[options]` with your desired command-line arguments. The options are:
   - `-n`: Create a new database file.
   - `-f <database file>`: Path to the database file. (required)
   - `-a`: Add an employee to the database.
   - `-r`: Remove an employee from the database.

## Cleaning the Build

To clean the build files (object files, binary, and database files), run:
```
make clean
```
