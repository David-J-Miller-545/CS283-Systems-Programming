1. How does the remote client determine when a command's output is fully received from the server, and what techniques can be used to handle partial reads or ensure complete message transmission?

_Our remote client knows when the full output is recieved from the server when it comes across an `EOF` character (our defined end boundary since TCP does not preserve boundaries naturally). To handle partial reads you could set up a recv loop that receives input from the server until it comes across the `EOF` char (our defined end boundary)._

2. This week's lecture on TCP explains that it is a reliable stream protocol rather than a message-oriented one. Since TCP does not preserve message boundaries, how should a networked shell protocol define and detect the beginning and end of a command sent over a TCP connection? What challenges arise if this is not handled correctly?

_Firstly the protocol would be required to be known by sender and receiver. Next, the protocol requires a header that should be of fixed size which contains information about the message for instance the length of the actual message. Alternatively a protocol could be restricted to commonly known data structures that have delimiter-based framing for instance a string ending with the '\0' character. If this is handled incorrectly the data sent from messages may be parsed incorrectly yielding in unexpected or undesirable results. One of these for example would be indefinitely waiting to receive an unmarked end._

3. Describe the general differences between stateful and stateless protocols.

_A stateful protocol maintains information about previous interactions creating a dependence of a current request on previous ones. A stateless protocol treats each request independently from all others (like a package)._

4. Our lecture this week stated that UDP is "unreliable". If that is the case, why would we ever use it?

_Although UDP doesn't maintain absolute data integrity, it has less overhead than TCP and maintains a low latency which is vital for real-time applications._

5. What interface/abstraction is provided by the operating system to enable applications to use network communications?

_The operating system provides Application Programming Interfaces (APIs) some of which contain abstractions to generalize low-level details of network protocols and hardware. The APIs enable applications to use network communictaions. The main type of APIs used to expedite network communications are Socket APIs._