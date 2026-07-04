# Central Server (Hub) Messaging Project

A terminal-based chat system for local networks, written in C++ with the [cpp-httplib](https://github.com/yhirose/cpp-httplib) library. A Raspberry Pi acts as the central server (hub): all messages are sent to the Pi, which stores them and distributes them to any client that asks. Clients only need to know the Pi's IP address — they never need each other's addresses.

The project consists of two files:

| File | Runs on | Role |
|---|---|---|
| `hub.cpp` | Raspberry Pi | Collects, stores, and distributes messages |
| `client.cpp` | All other devices | Sends messages, fetches new ones and prints them |

## How It Works

The hub is an HTTP server on port 8080 that answers two kinds of requests:

- **`POST /message`** — When a client sends a message, the hub appends it to its list.
- **`GET /message?from=N`** — When a client says "I have seen N messages," the hub returns every message after the N-th one.

Each client does two jobs at once: the main loop sends whatever you type to the hub, while a background thread asks the hub once per second whether there are new messages and prints any that arrive. This regular asking technique is called **polling**.

```
                       ┌──────────────────────────┐
                       │  Raspberry Pi (HUB)      │
                       │  message list: [.....]   │
                       └───▲──────▲──────▲────────┘
            POST /message  │      │      │  GET /message?from=N
                 ┌─────────┘      │      └─────────┐
            ┌────┴────┐      ┌────┴────┐      ┌────┴────┐
            │  Ali    │      │  Veli   │      │  Ayşe   │
            │ (Mac)   │      │ (Laptop)│      │ (PC)    │
            └─────────┘      └─────────┘      └─────────┘
```

Your own messages also come back from the hub and appear on your screen; this serves as a natural confirmation that the message reached the hub.

## Requirements

- A Raspberry Pi (the hub) and at least one client device on the same local network
- The `g++` compiler on each device
- The `httplib.h` header file

cpp-httplib is a header-only library: no installation or linking is required — just place the single `httplib.h` file in the same folder as the source code. There is no need to download the whole repository.

## Setup

### On the Raspberry Pi (hub)

```bash
mkdir messenger && cd messenger
# copy hub.cpp into this folder
wget https://raw.githubusercontent.com/yhirose/cpp-httplib/master/httplib.h
g++ -std=c++11 -Wno-psabi hub.cpp -o hub -lpthread
```

### On client devices

```bash
mkdir messenger && cd messenger
# copy client.cpp into this folder
curl -O https://raw.githubusercontent.com/yhirose/cpp-httplib/master/httplib.h   # macOS
# or: wget https://raw.githubusercontent.com/yhirose/cpp-httplib/master/httplib.h  # Linux
g++ -std=c++11 -Wno-psabi client.cpp -o client -lpthread
```

Compiler flags: `-std=c++11` enables the C++11 standard required by httplib, `-lpthread` links the threading library, and `-Wno-psabi` hides the harmless "parameter passing changed in GCC 7.1" notes seen on ARM (the Pi) — they are not errors.

## Usage

1. Start the hub on the Pi first:

```bash
./hub
```

You should see "Hub listening on port 8080...". Find the Pi's IP with `hostname -I` (for example `192.168.1.37`).

2. On each client device, start the client with your name and the hub's IP:

```bash
./client Ali 192.168.1.37
```

3. Type and press Enter. Your message goes to the hub and appears on every client's screen (including yours) as `Ali: your message` within a second. Press `Ctrl+C` to quit.

Adding a new device requires no configuration at all: install the client on it and start it with the hub's IP. Anyone who joins later sees the entire chat history, since the hub keeps every message in its list.

## Limitations and Possible Improvements

- **Single point of failure:** If the hub goes down, nobody can message.
- **History is not persistent:** Messages live in RAM and are lost when the hub restarts.
- **No encryption or authentication:** Messages are plain text and anyone on the network can send them. Use only on local networks you trust.
- **Polling delay:** Messages arrive with up to 1 second of delay rather than instantly.

## Source Code
https://github.com/hexpad/raspberry-pi-chat-hub/blob/main/client.cpp
