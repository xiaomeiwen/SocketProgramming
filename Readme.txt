Name:           Xiaomei Wen
Student ID:     7022928251

1. What have your done?
  This is a socket programming project. In this project, it contains 4 files. The client.cpp run as the client side. Other files sever as the Google Compute Engine for processing. Client send job.txt to edge server, and edge server separate the work to server_and and server_or. These two backend server will do computations and send the results back to edge server. And user will get the results.

2. What your code files and what each one of them does?
  In my project, the buffer size of each line is 30.
  Max size of the total file is 4000.
  Max number of lines is 100.
  a. client.cpp
    Read filename from argv, then read each line of the file. Each line will buffer in a sizeof 30 char array, then send to edge server. It use TCP to connect with edge server. It will receive result from edge server and print the results as the same sequence as the input.
  b. edge.cpp
    Can work continously, receiving messages from client. Edge server can separate messages, send messages begin with 'a' to server_and,  send messages begin with 'o' to server_or. It also use flag array to remember the sequence of input. Using UDP to connect with back servers. Receiving results from back server and send them back to client.
  c. server_and
    Recieve messages from edge server. Extract 2 numbers from the input string. Do AND computation. Send the result to edge server. Use UDP socket.
  d. server_or
    Recieve messages from edge server. Extract 2 numbers from the input string. Do OR computation. Send the result to edge server. Use UDP socket.

3. What the TA should do to run your programs.
  Compile all cpp files using "make all" first.
  Open one terminal, run "make edge"
  Open another terminal, run "make server_and"
  Open the third terminal, run "make server_or"
  Open a terminal for client, run "./client <filename>"

4. The format of all the messages exchanged.
  Reading from txt using char Array.
  Client sending to Edge Server using char[30] for each line.
  Edge Server using char[30] for receiving each line.
  Edge Server using string array buffer messages AND and OR.
  Edge Server turn string to char array, then send them to backserver(each line has a sizeof 30).
  Back server change each line to 2 strings, computation result for each line is a string.
  Back server send messages back using char arrays.
  Edge server send results back using char arrays.

5. Idiosyncrasy programs?
  In my project, the buffer size of each line is 30.
  Max size of the total file is 4000.
  Max number of lines is 100.
  If input message of And or Or equals to zero, may cause error in back server.

6. Reused Code.
  No reuse.