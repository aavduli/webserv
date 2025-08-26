Initialization of fd_set:
We initialize separate fd_set structures for each type of event(read or write…) we wish to monitor. These structures serve as bit masks representing sets of file descriptors(so that we can know which FDs are ready for read/write, bit 1 for ready and 0 for not).

Populating the fd_set:
File descriptors of interest are added to the respective fd_set using macros like FD_SET.

Setting the Timeout:
We can set a timeout value, to determine the maximum duration select should wait for an event. If the timeout elapses without any events, select returns, allowing the program to proceed.

Calling select:
The program invokes the select function, providing the "highest file descriptor value" + 1 (man select), the three fd_sets, and the timeout; We can set NULL as value for events that we are not interested in, or for timeout if we wish select to block indefinitely until an event occurs.

Blocking or Returning:
select enters a blocking state, awaiting events on the monitored file descriptors or until the timeout expires. Upon an event or timeout, select returns control to the program.

Checking the fd_set:
After returning, the program examines the fd_sets to identify which file descriptors are ready for the specified events, by using macros like FD_ISSET.

Handling Events:
We can respond to events by executing the required actions based on the identified file descriptors. For instance, we may read/write data from/to a socket or handle errors.

Looping or Exiting:
We often employ a loop to repeat the monitoring process, allowing continuous asynchronous handling of events on multiple file descriptors.
