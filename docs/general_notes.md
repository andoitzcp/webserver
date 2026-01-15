# General notes

## Notes on [article_1](https://m4nnb3ll.medium.com/webserv-building-a-non-blocking-web-server-in-c-98-a-42-project-04c7365e4ec7)

### Non-Blocking server:

In the context of web servers, blocking refers to the situation where a thread is waiting 
for an operation to complete before it can proceed further. 

The most significant blocking issue arises when dealing with input/output (I/O) operations, 
such as reading from or writing to sockets. 

To address this w will use asynchronous I/O.

### Asynchronous I/0 (select, poll, epoll, ...):

- [select](https://man7.org/linux/man-pages/man2/select.2.html):
    `select()` allows a program to monitor multiple file descriptors,
    waiting until one or more of the file descriptors become "ready"
    for some class of I/O operation (e.g., input possible).  A file
    descriptor is considered ready if it is possible to perform a
    corresponding I/O operation (e.g., read(2), or a sufficiently
    small write(2)) without blocking.

- [poll](https://man7.org/linux/man-pages/man2/poll.2.html):
    `poll()` performs a similar task to select(2): it waits for one of a
    set of file descriptors to become ready to perform I/O.

- [epoll](https://man7.org/linux/man-pages/man7/epoll.7.html):
    `epoll API` performs a similar task to poll(2): monitoring
    multiple file descriptors to see if I/O is possible on any of
    them.  The epoll API can be used either as an edge-triggered or a
    level-triggered interface and scales well to large numbers of
    watched file descriptors.

poll and epoll are simpler/better alternatives to select , 
but select is more portable (can be used accross different platforms)

### Parsing HTTP

- [HTTP Semantics](https://datatracker.ietf.org/doc/html/rfc9110)
- [HTTP 1.1](https://datatracker.ietf.org/doc/html/rfc9112)

HTTP messages consist of a request or response line, headers, an empty line(CRLF or \r\n), 
and an optional message body.

```
HTTP-message   =    start-line CRLF
                    *( field-line CRLF )
                    CRLF
                    [ message-body ]

start-line     =    request-line / status-line
```

#### Resquest Line:

The request line in an HTTP request message contains the method, URI, and HTTP version.

```
request-line =  method SP request-target SP HTTP-version
```

```http
example:        GET /path/to/resource HTTP/1.1
``` 

- Method        = GET
- URI           = /path/to/resource 
- HTTP Version  = HTTP/1.1

#### Response Line / Status Line:

```
status-line =   HTTP-version SP status-code SP [reason phrase]
```

```http
example:        HTTP/1.1 200 OK        
``` 

- HTTP Version  = HTTP/1.# 
- Status code   = 200       [Status-code Index](https://developer.mozilla.org/en-US/docs/Web/HTTP/Reference/Status)
- Reason phrase = OK

#### [Headers](https://developer.mozilla.org/en-US/docs/Web/HTTP/Reference/Headers)
Both requests and responses can include headers, 
providing additional information about the message such as content type, content length, and more.

``` http
Host: example.com
Content-Type: text/html
Content-Length: 256
```

#### Message Body:
The message body, though optional, may contain data relevant to the request or response. 
Parsing the message body depends on factors like content type and length, transfer coding etc...

### Proccessing request and generating response
After finishing parsing the HTTP messages,
the next steps involve implementing logic to handle different HTTP methods,
process requests, and generate appropriate responses.

![Response generation procces](/resources/webserv_ Building a Non-Blocking Web Server in C++98 (A 42 project) _ by MannBell _ Medium_files/1_qw6GeQe9tMULtH0ifqtL7Q.webp)
