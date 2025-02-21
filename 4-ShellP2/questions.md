1. Can you think of why we use `fork/execvp` instead of just calling `execvp` directly? What value do you think the `fork` provides?

    > **Answer**:  `fork` creates a new child process which then executes `execvp`. `execvp` when called dumps the current program and loads whatever new program that was called to run into the current process memory. This meaning if we called it in our main process it would stop the program so instead we `fork` to create a child process that can run that new program instead.

2. What happens if the fork() system call fails? How does your implementation handle this scenario?

    > **Answer**:  If the system call fails a child process is not created and the value of -1 is returned to its caller.

3. How does execvp() find the command to execute? What system environment variable plays a role in this process?

    > **Answer**:  _If the filename does not include a slash character, it will search for the file in coloon-seperated list of directory pathnames specified in the **PATH** environment variable. If the filename does include the slash character it assume the filename is the full pathname to said file to be executed._

4. What is the purpose of calling wait() in the parent process after forking? What would happen if we didn’t call it?

    > **Answer**:  _The purpose of waiting is so that the parent doesn't keep running past the parent._

5. In the referenced demo code we used WEXITSTATUS(). What information does this provide, and why is it important?

    > **Answer**:  _`WEXITSTATUS()` provides the return status of a child process, this is important if the parent intends to do error handling._

6. Describe how your implementation of build_cmd_buff() handles quoted arguments. Why is this necessary?

    > **Answer**:  _My implementation of `build_cmd_buff()` handles quoted arguments through a new function I created from `extract_tok_from_quotes()`. Essentially it is called when the first instance of a quote it found and attempts to find the second, if it doesn't find a second it returns NULL and `build_cmd_buff()` fails._

7. What changes did you make to your parsing logic compared to the previous assignment? Were there any unexpected challenges in refactoring your old code?

    > **Answer**:  _Instead of using the common `strtok()` I had to use `strtok_r()`, maintain, and also update a `saveptr` which I also utilize in my `extract_tok_from_quotes()` function. The only real unexpected challenge was maintaining the `saveptr` between parsing spaces and quotes._

8. For this quesiton, you need to do some research on Linux signals. You can use [this google search](https://www.google.com/search?q=Linux+signals+overview+site%3Aman7.org+OR+site%3Alinux.die.net+OR+site%3Atldp.org&oq=Linux+signals+overview+site%3Aman7.org+OR+site%3Alinux.die.net+OR+site%3Atldp.org&gs_lcrp=EgZjaHJvbWUyBggAEEUYOdIBBzc2MGowajeoAgCwAgA&sourceid=chrome&ie=UTF-8) to get started.

- What is the purpose of signals in a Linux system, and how do they differ from other forms of interprocess communication (IPC)?

    > **Answer**:  _Signals exist to interrupt the current process to alert them of something, it is then determined by that process how to handle it. Opposed to other forms of interprocess communication signals are simple, event-driven communication, only providing basic information about the event to be handled._

- Find and describe three commonly used signals (e.g., SIGKILL, SIGTERM, SIGINT). What are their typical use cases?

    > **Answer**:  _SIGKILL forces immediate termination of a process without doing any cleanup. It is typically used as last resort when a process is unresponsive or is a risk. SIGTERM requests a process to terminate allowing for cleanup and a proper exit, commonly used when one process just wants another process to shut down. SIGINT is a request to interrupt the process in progress to be handled. Most commonly SIGINT is sent by the user when they press Ctrl+C and terminates the process. Process can handle SIGINT in different ways however the default is to terminate._

- What happens when a process receives SIGSTOP? Can it be caught or ignored like SIGINT? Why or why not?

    > **Answer**:  _When a process recieves SIGSTOP it immediately stops and can not be caught like SIGINT because it it designed to forcefully stop a process without letting the process handle it._
