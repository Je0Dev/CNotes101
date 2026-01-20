How to Build & Run (Linux-only)
Type make. This will compile c_redis.
Run the server: ./c_redis
In another terminal, use telnet or nc (netcat) to connect and send commands:
telnet 127.0.0.1 6379
SET mykey hello (Press Enter)
Server should respond: +OK
GET mykey (Press Enter)
Server should respond: +hello
GET nonkey (Press Enter)
Server should respond: $-1
Press Ctrl+C in the server terminal to shut it down gracefully.
