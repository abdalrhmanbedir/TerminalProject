# TCP Client/Server File Management Project (Qt + qmake)

A TCP client/server project built with **Qt (qmake)** that supports authenticated file operations for multiple concurrent clients using a simple text-based protocol.

---

## Features

- TCP server that accepts multiple clients
- Per-client session state (authentication)
- Simple text-based protocol (one command per line)
- File operations restricted to a dedicated server directory
- Path traversal protection
- Console-based TCP client
- Unit tests using GoogleTest (gtest)
- Modular and clean architecture

---

## Project Structure

text
terminalproject/
├─ ServerApp/                 # TCP server
│  ├─ main.cpp
│  ├─ ClientSession.h/.cpp
│  ├─ CommandDispatcher.h/.cpp
│  ├─ CommandHandler.h/.cpp
│  ├─ FileService.h/.cpp
│  ├─ Protocol.h/.cpp
│  └─ ServerApp.pro
├─ ClientApp/                 # TCP client (console)
│  ├─ main.cpp
│  └─ ClientApp.pro
└─ Gtest/                     # Unit tests (gtest)
   ├─ test_main.cpp
   ├─ test_protocol.cpp
   ├─ test_fileservice.cpp
   ├─ ... (other tests)
   └─ Gtest.pro
Protocol (Commands)
Each request is one line terminated by \n.

Server responses are line-based.

Commands are case-insensitive on the verb (server normalizes to uppercase).

## Authentication
Must authenticate before any file command:

AUTH <username>,<password>
Example:

AUTH abdalrhmanmoghazy,575859
## Responses:

OK Authenticated

ERROR 403 INVALID_CREDENTIALS

OK AlreadyAuthenticated

## File Commands (require AUTH)
All file paths are treated as relative to the server data/ directory.
Path traversal like ../ should be rejected.

## CREATE
Create an empty file:

CREATE <filename>
## Responses:

OK CREATED

ERROR 409 FILE_ALREADY_EXISTS

ERROR 403 FORBIDDEN_PATH

## WRITE
Overwrite file content:


WRITE <filename>;<data>
## Responses:

OK WRITTEN

ERROR 404 FILE_NOT_FOUND

## APPEND
Append to file:


APPEND <filename>;<data>
## Responses:

OK APPENDED

ERROR 404 FILE_NOT_FOUND

## READ
Read full file:


READ <filename>
## Response format:


OK <len>
<raw_file_bytes...>\n
Where <len> is byte count of the payload.

## DELETE
Delete file:


DELETE <filename>
## Responses:

OK DELETED

ERROR 404 FILE_NOT_FOUND

## RENAME
Rename file:


RENAME <old>;<new>
## Responses:

OK RENAMED

ERROR 404 FILE_NOT_FOUND

ERROR 409 FILE_ALREADY_EXISTS

## LIST
List files:


LIST
## Response format:


OK <count>
file1.txt
file2.txt
...
## INFO
Get file info:


INFO <filename>
Example response:


OK size=123 modified=1700000000
Build & Run
Requirements
Qt 6 (Core + Network) with qmake

A C++ compiler (g++/clang++)

On Linux: make

## Build Server

mkdir -p build-server
cd build-server
qmake ../ServerApp/ServerApp.pro
make -j
./ServerApp
Server listens on port 5555 by default.

## Build Client

mkdir -p build-client
cd build-client
qmake ../ClientApp/ClientApp.pro
make -j
./ClientApp
Run Multiple Clients
Open multiple terminals and run the client in each terminal while the server is running.

## Unit Testing (GoogleTest)
Install gtest (Ubuntu example)

sudo apt-get update
sudo apt-get install -y libgtest-dev cmake
cd /usr/src/googletest
sudo cmake -S . -B build
sudo cmake --build build -j
sudo cp build/lib/libgtest*.a /usr/lib/
Build & Run Tests

mkdir -p build-gtest
cd build-gtest
qmake ../Gtest/Gtest.pro
make -j
./ServerApp_tests
Notes / Design
The server maintains a per-client session (authentication state).

File operations are restricted to a server-controlled directory (data/) for safety.

Code is organized into small modules:

Protocol: parsing input lines into commands

CommandHandler: business logic for commands

FileService: file system operations

ClientSession: network session per connected client

CommandDispatcher: routes parsed commands to handler
