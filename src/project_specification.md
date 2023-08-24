# Project 2022 (Winter) CSC209

## Overview
In CSC209, subsequent CS courses, and many aspects of your career, your primary way of interacting with Linux systems will be through the Shell.

A shell is a powerful program that processes user input, provides a command line interface and allows you to chain
commands together, so that the output of one program becomes the input to another. A shell is also the primary method for accessing system resources and allows you to control many interactions with the operating system. For more information feel free
to read more about the various shells [here](https://en.wikipedia.org/wiki/Unix_shell).

In CSC209, your project this term is to write a C program that implements a fully functional shell that we will call mysh. mysh will provide a vast range of functionality matching, or even outperforming, that of other builtin shells in many ways.

Writing a shell is a large task and will be completed via several milestones throughout the term.

This assignment/project is to be done individually (not in groups). All the work you submit must be your own.


## Milestones, Timelines and General Overview

This section lists the milestones and their deliverables. You are encouraged to read the entire document in order to approach each weeks' lecture/lab/PCRS materials within the context of how it might apply to any given milestone.

#### Timelines

- Milestone 1: Getting started (Thu, Jan 27 @ 9:00pm plus grace tokens)
- Milestone 2: Strings and Heap (Thu, Feb 3 @ 9:00pm plus grace tokens)
- Milestone 3: File System (Thu, Feb 17 @ 9:00pm plus grace tokens)
- Milestone 4: Pipes (Thu, Mar 10 @ 9:00pm plus grace tokens)
- Milestone 5: Signals and Process Management (Thu, Mar 17 @ 9:00pm plus grace tokens)
- Milestone 6: Network Support (Thu, Mar 31 @ 9:00pm plus grace tokens)
- Final submission: Due on the last day of classes (Fri, Apr 8) @ 5:00pm (no grace tokens, no late submissions)

For each milestone and for the final submission, your repo must contain a Makefile and all .c and .h files required.

#### Makefile
The Makefile must produce an executable called
mysh

The command "make run" must start your shell.

The Makefile must use the following flags to compile your files:<br>
-g -Wall -Werror -fsanitize=address

#### General Overview

Your mysh will support the following commands as builtins: (Don't worry! We will go into details about how they are expected to function later in this document.)

- echo
- ls
- cd
- cat
- wc
- pipes (|)
- bg (&)
- kill
- ps
- exit
- start-server
- close-server
- send
- start-client
cd 20

All code must be submitted through git to MarkUs before the due date to qualify for bonus points and marks (see the marking section at the end of the document). You may commit as early and as often as you wish, and you are encouraged to commit your code to the repo every day. Do not leave it until the last minute, as troubleshooting issues with the repo and git may require several days, so it's best to get this set up and become familiar with the process as early as possible.

Double-check that what you have committed for each milestone works as you expect on the lab machines. (Hint: checkout your code into a new directory on the lab machines and run make and the complete test suite).

Any ambiguity and vagueness in regard to the specifications should be clarified with the instructors (usually on piazza: check the FAQ posts for each milestone). More often than not, the functionality should mimic that of an actual Linux shell found on the lab machines, but in some cases the expected behaviour will deviate.


### Milestone 1: Getting Started (week 3)

Due Date: (Thu, Jan 27 @ 9:00pm plus grace tokens)
Required Functionality:
- User prompt: mysh$ (this is shown and waiting for user input)
- Command: echo \[string\]
  - e.g., mysh$ echo hello world
    - Outputs the \[string\] component (i.e., hello world) to stdout
    - In contrast to bash, echo does not handle quotes in a unique way. Specifically, echo "hello world" should display "hello world", not hello world.
    - Similar to bash, extra spaces are ignored. So echo "hello&nbsp; &nbsp; &nbsp;world" displays  "hello world"
- Command: exit
  - e.g., mysh$ exit
  - Terminates the shell program with exit code 0.
- Limitations and Notes:
  - The maximum input line length, including the command itself, is 64 characters.
  - If more than 64 characters are entered, output an error to stderr: `ERROR: input line too long` followed by a new line.
  - Upon receiving any command other than echo or exit, your code should output:
    `ERROR: Unrecognized command: [the unrecognized command]` followed by a new line.
  - Your shell should NEVER crash, no matter what the input is.


### Milestone 2: Strings and the Heap (week 4)

  Due Date: (Thu, Feb 3 @ 9:00pm plus grace tokens)
  Required Functionality:
  - All the functionality requirements of previous milestones.
  - Setting and using environment variables:
    - e.g.: \
      mysh$ myvar=hello \
      mysh$ myvar2=world \
      mysh$ echo $myvar $myvar2 \
      hello world
    - The program must support storing an arbitrary number of environment variables.
    - Your programs memory footprint must grow proportionally and reasonably with the number of environment variables.
    - Hint: You are required to dynamically allocate memory for environment variables.


### Milestone 3: File system (week 6)

 Due Date: (Thu, Feb 17 @ 9:00pm plus grace tokens)

 Required Functionality:

 - All the functionality requirements of previous milestones.
 - Error Handling: Whenever a builtin command fails, you must report an appropriate error messages. Some required descriptions are outlined below. For any other error scenarios, choose a corresponding descriptive message.
   - ERROR: Description of error message (for errors not covered in the next two cases)
   - ERROR: Builtin failed: builtin name
   - For all commands not specified in the previous milestones or those not yet implemented (e.g., true, yes), check whether they are builtin (to your shell), and if so, run the builtin commands, otherwise display the error `ERROR: Unrecognized command: [the unrecognized command]`
 - Command: ls
   - ls \[path\]
     - path can be absolute or relative
     - --f substring. Display all directory contents that contain the substring
     - --rec \[path\] Indicates a recursive ls traversal starting from path
     - --d \<depth value\>. The search depth of the file traversal tree
     - --rec and --d must be provided together. Report an error if the arguments are not provided together.
     - Report `ERROR: Invalid path` if the provided path is invalid
 - Command: cd
   - cd \[path\]
   - path can be absolute or relative
   - Some additional features:
     - cd ... (change directory 2 directories up: ../.. must also work)
     - cd .... (change directory 3 directories up: ../../.. must also work)
     - cd .. (zero or more copies of "/.." may be appended, and cd must change directory the correct amount of times)
     - Report `ERROR: Invalid path` if the provided path is invalid
 - Command: cat
   - cat \[file\] (displays the contents of the file to stdout)
     - Report `ERROR: No input source provided` if there is no file provided
     - Report `ERROR: Cannot open file` if the provided file path is invalid
 - Command: wc
   - wc \[file\]
   - e.g., Assume the file foobar has 1234 characters in it, each separated by any whitespace (e.g., \n, \t, \r).  These characters form 251 words and span 25 lines. Then ...\
     mysh$ wc foobar.txt \
     word count 251 \
     character count 1234  \
     newline count 25
    - The implementation for this function may not use string methods
    - Report `ERROR: No input source provided` if there is no file provided
    - Report `ERROR: Cannot open file` if the provided file path is invalid


 ### Milestone 4: Pipes (week 8)

 Due Date: (Thu, Mar 10 @ 9:00pm plus grace tokens)

 Required Functionality:

  - All the functionality requirements of previous milestones.
  - Refactor cat and wc
    - If there is a file name provided, proceed as in milestone 3.
    - If a file argument was provided, proceed as in milestone 3
    - If there is standard input, execute the command by reading standard input.
    - If there is no standard input, display `ERROR: No input source provided`.
  - Pipes work for all built-in commands.
    - E.g., `cat some_file | wc`
      - This should pipe the contents of some_file as input to the wc command.
      - Note: Input via pipe is not subject to the 64 character command line limit.

 ### Milestone 5: Signals and Process management (week 9)

Due Date: (Thu, Mar 17 @ 9:00pm plus grace tokens)

Required Functionality:
  - All the functionality requirements of previous milestones.
  - If a shell command does not exist, use the default bash implementation
  - If a shell command does not exist, use the implementation from /bin or /usr/bin
  - The shell must not terminate when pressing ctrl+c
  - Backgrounding a process via &
    - Starts the program as a background process
    - Display `[number] pid` when the process is launched. e.g., `[1] 25787`. The behaviour of this is identical to bash.
    - Display `[number]+  Done command` when the process completes. The behavioural is identical to bash, except that we will only display Done messages. If a process terminates because of a signal, a Done is still shown.
  - Command: kill \[pid\] \[signum\]
    - Send signal number signum to the process with id pid.
    - If signum is not provided, send a SIGTERM
    - If the process id is invalid, report `ERROR: The process does not exist`
    - If the signal is invalid report `ERROR: Invalid signal specified`
  - Command: ps
    - List process names and ids for all processes launched by this shell. For example: \
    mysh$ sleep 50 &  \
    [1] 8574 \
    mysh$ sleep 20 & \
    [2] 8575 \
    mysh$ ps \
    sleep 8575 \
    sleep 8574 \
    mysh$

### Milestone 6: Network support (week 11)

  Due Date: (Thu, Mar 31 @ 9:00pm plus grace tokens)

  Required Functionality:
  - All the functionality requirements of previous milestones.
  - Command: start-server \<port number>
    - Initiates a background server on the provided port number. The server must be non-blocking and must allow multiple connections at the same time.
    - Report `ERROR: No port provided` if no port is provided
  - Command: close-server
    - Terminates the current server.
  - Commmand: send \<port number> \<hostname> \<message>
    - Send a message to the hostname at the port number.
  - Command: start-client \<port number> \<hostname>
    - Starts a client that can send multiple messages. The messages are taken as standard input. The client is connected once and can continue sending messages until pressing a CTRL+D or CTRL+C.
    - Report `ERROR: No port provided` if no port provided
    - Report `ERROR: No hostname provided` if no hostname provided


### Final Project

  Due Date: Last day of classes @ 5pm (__NO Grace tokens and due at 5pm!!!!__ -- it's time to enjoy the summer!).

  Required Functionality:
  - All the functionality requirements of previous milestones as tested by the milestone tests (70%)
  - Correctly handle additional use/edge cases (30%)

## Grading of the Project

For each milestone you will be provided with a range of tests in various test suites. We will use these test suites to determine 70% of the project's grade. The other 30% will come from additional test cases that cover further edge cases. You are encouraged to test your project against additional edge cases that you could think of. Do not just rely on the provided test cases.

Submitting a milestone on time (before the due date, not using grace tokens) and passing at least 70% of the test suite points will award you a 1% bonus, so it is possible to earn a total of 6% bonus points toward your final project.

Test cases are cumulative for each milestone, meaning milestone 2 will include tests for milestone 1 and milestone 2. In order to receive the bonus points for milestone 2, you must receive at least 70% on all test cases (including the milestone 1 test cases). In other words, Milestone N contains tests for milestone 1 to N for a total of M tests, you must obtain 70% of the points in M tests to receive the bonus point for milestone N.


### Hints and How to Get Started

Begin by checking out / pulling your repo. For milestone 1, you should now have additional directories in your repo (one directory named M1). Move the zip file containing the starter code into this M1 directory and unzip it. You will now see two additional directories called "src" and "tests". Add both directories to your repo and commit them to MarkUs (this is to make sure they are in the correct place and your git access works. It is easier to trouble shoot now than before a deadline when you have done a lot of work already). You can delete the zip file, as you won't be needing it again.

The starter code contains several files. The source files which you will submit are in the src directory. This is what you need to check in. The structure of what you should see in your repo is MX/src/your_source_files (where X is {1..6} corresponding to each milestone).

You do not need to check in the tests directory, as we will be using our own. I recommend you familiarize yourself with the code by reading the files in src in the following order.

- builtins.h and builtins.c
- io_helpers.h and io_helpers.c
- mysh.c
- the other files are empty, but you will use variables.h/c when dealing with environment variables later; and commands.h/c when parsing and distinguishing between multiple commands your shell supports or has to call from the system directly.

Look for the two TODOs in mysh.c; and then move on to io_helpers.c; last, implement the builtin command echo in builtins.c.

To compile, simply type 'make' and press enter.

As the project progresses you will make further changes to all these files regularly.

Milestone 1 is very short, and it is highly recommended that you continue with milestone 2 immediately to keep the momentum going.


### Running the tests

You are provided with a partial test suite for the first milestone. (We will release test suites for future milestones shortly as well.) Take a look at main_test_runner.py. Using python3 you can run all the tests by typing python3 main_test_runner.py. This will place a FEEDBACK...txt file into your src directory. Each entry in the feedback file corresponds to a test case which you are given. Since you are not checking in the tests directory, feel free to add further test cases as well.
We will use the test cases that we provided to you, plus some additional test suites which are testing additional edge cases, not provided to you.
