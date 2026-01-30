# Redis CLI (C++)

A Redis command-line client written in **C++17**, implementing an **event-driven interactive REPL**, **Redis pub/sub**, and **one-shot command execution**. Designed to closely mirror the behavior of Redis’ official redis-cli while focusing on correctness and robustness.

## Features

### Interactive REPL

* Custom Redis prompt: host:port>
* Command history and line editing via GNU Readline
* Case-insensitive command handling
* Clean exit using exit, quit, or Ctrl+D

### Event-Driven Architecture

* Uses poll() to multiplex terminal input (stdin) and the Redis TCP socket
* No busy waiting
* Non-blocking user interface while waiting on network I/O
* Clear separation between UI handling and network logic

### Redis Pub/Sub Support

* Supports SUBSCRIBE with a dedicated subscription event loop
* Handles Redis push messages asynchronously
* Allows clean exit from subscription mode
* Properly drains UNSUBSCRIBE responses to maintain protocol correctness

### One-Shot Command Mode

Execute a single Redis command directly from the shell:

./my\_redis\_cli GET mykey

* Sends exactly one command
* Prints the response
* Exits immediately
* No interactive REPL or readline initialization

### Robust Socket Handling

* Detects Redis disconnections via POLLHUP, POLLERR, and EOF detection using recv(MSG\_PEEK)
* Fails fast on unrecoverable socket or protocol errors
* Ensures clean shutdown paths in all modes

## Project Structure

Client/

├── main.cpp &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; # Argument parsing and entry point

├── CLI.\*   &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; # REPL, event loop, pub/sub handling

├── RedisClient.\*  &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; # TCP connection management

├── CommandHandler.\* &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; # Command parsing and RESP formatting

├── ResponseParser.\*  &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;  # RESP2 response parsing

Makefile

## Build

### Requirements

* Linux (tested on Ubuntu / Xubuntu / WSL)
* g++ with C++17 support
* GNU Readline development headers

Install dependencies (Ubuntu/Debian):

sudo apt install libreadline-dev

### Compile

make

The binary is generated at:

bin/my\_redis\_cli

## Usage

### Interactive Mode

./bin/my\_redis\_cli

Example:

127.0.0.1:6379> PING

PONG

127.0.0.1:6379>

### One-Shot Mode

./bin/my\_redis\_cli SET key value

./bin/my\_redis\_cli GET key


### Pub/Sub Example

Terminal 1:
./bin/my\_redis\_cli

127.0.0.1:6379> SUBSCRIBE news

(Subscribed) Type 'exit' or 'quit' to leave subscription mode.


Terminal 2:
redis-cli PUBLISH news "hello"


Terminal 1 output:

message

news

hello


Exit subscription mode:

exit

(Exited subscription mode)

127.0.0.1:6379>


## Protocol Support

* Redis Serialization Protocol RESP2
* Supported reply types: Simple Strings, Errors, Integers, Bulk Strings, Arrays
