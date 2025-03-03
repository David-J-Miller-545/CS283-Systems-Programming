1. Your shell forks multiple child processes when executing piped commands. How does your implementation ensure that all child processes complete before the shell continues accepting user input? What would happen if you forgot to call waitpid() on all child processes?

_Within my implementation the shell waits for piped commands to execute in their child processes. If we forgot about waitpid() on the children shell and the children may race against eachother and not yield proper output._

2. The dup2() function is used to redirect input and output file descriptors. Explain why it is necessary to close unused pipe ends after calling dup2(). What could go wrong if you leave pipes open?

_If you leave the pipes open it restricts the amount of available file descriptors as well as having two referrences to each of the file endpoints of the pipe._

3. Your shell recognizes built-in commands (cd, exit, dragon). Unlike external commands, built-in commands do not require execvp(). Why is cd implemented as a built-in rather than an external command? What challenges would arise if cd were implemented as an external process?

_CD is internal instead of external because it can not be cast onto a child process to change the directory of the parent. If we did try to change the pwd in the child however, it would change where the child is looking and not effect the parent._

4. Currently, your shell supports a fixed number of piped commands (CMD_MAX). How would you modify your implementation to allow an arbitrary number of piped commands while still handling memory allocation efficiently? What trade-offs would you need to consider?

_As opposed to containing all of the commands in an array we would contain them in some linked list that would filter out space for commands that are executed and allocate space for commands that have not been (possibly while other commands are running in the pipeline). This would be much more difficult to implement then the current implenations. You would have to allocate more memory per command to contain references to commands further in the pipeline, you would also require some way to toggle between reading and running commands._
