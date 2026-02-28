# WebSocket Technology Concepts

## 📚 Table of Contents

1. [What is WebSocket?](#what-is-websocket)
2. [HTTP vs WebSocket](#http-vs-websocket)
3. [WebSocket Protocol](#websocket-protocol)
4. [Connection Lifecycle](#connection-lifecycle)
5. [Message Framing](#message-framing)
6. [Real-Time Communication Patterns](#real-time-communication-patterns)
7. [JSON Serialization](#json-serialization)
8. [Client-Server Architecture](#client-server-architecture)
9. [ESP32 Implementation](#esp32-implementation)
10. [Performance Considerations](#performance-considerations)
11. [Security](#security)
12. [Use Cases](#use-cases)

---

## What is WebSocket?

### Definition

**WebSocket** is a communication protocol that provides **full-duplex** (bi-directional) communication channels over a single TCP connection.

### Key Characteristics

```
Traditional HTTP:           WebSocket:
┌─────────┐                ┌─────────┐
│ Client  │                │ Client  │
└────┬────┘                └────┬────┘
     │                          ║
     │ Request  ────>           ║
     │ Response <────           ║ Persistent
     │                          ║ Connection
     │ Request  ────>           ║
     │ Response <────           ║
     │                          ║
     │                          ║ Data ↔
     │                          ║ Data ↔
     │                          ║ Data ↔
┌────┴────┐                ┌───╨────┐
│ Server  │                │ Server │
└─────────┘                └────────┘
```

**WebSocket Advantages:**

- ✅ **Persistent connection** - stays open
- ✅ **Bi-directional** - both sides can send anytime
- ✅ **Low latency** - no connection overhead per message
- ✅ **Real-time** - instant data push/pull
- ✅ **Efficient** - low bandwidth overhead

---

## HTTP vs WebSocket

### Protocol Comparison

| Feature         | HTTP                                 | WebSocket             |
| --------------- | ------------------------------------ | --------------------- |
| **Connection**  | Request-Response                     | Persistent            |
| **Direction**   | Client → Server only                 | Bi-directional        |
| **Overhead**    | Headers per request (~500-800 bytes) | 2-6 bytes per message |
| **Latency**     | High (new connection each time)      | Low (persistent)      |
| **Real-time**   | No (polling/long-polling hacks)      | Yes (native)          |
| **Server Push** | Not supported                        | Native support        |
| **Use Case**    | Document retrieval                   | Real-time apps        |

---

### Communication Models

#### HTTP Polling (Inefficient)

```
Client               Server
  │                    │
  ├──── Request ──────>│ "Any updates?"
  │<─── Response ──────┤ "No"
  │                    │
  │ (wait 1 second)    │
  │                    │
  ├──── Request ──────>│ "Any updates?"
  │<─── Response ──────┤ "No"
  │                    │
  │ (wait 1 second)    │
  │                    │
  ├──── Request ──────>│ "Any updates?"
  │<─── Response ──────┤ "Yes! Data: 25.3°C"
  │                    │
```

**Problems:**

- 🔴 Wastes bandwidth (constant HTTP headers)
- 🔴 Delayed updates (polling interval)
- 🔴 Server load (many unnecessary requests)

---

#### HTTP Long-Polling (Better, but still inefficient)

```
Client               Server
  │                    │
  ├──── Request ──────>│ "Tell me when data changes"
  │                    │ (waits... waits...)
  │                    │ (new data arrives!)
  │<─── Response ──────┤ "Data: 25.3°C"
  │                    │
  ├──── Request ──────>│ "Tell me when data changes"
  │                    │ (waits... waits...)
```

**Problems:**

- 🟡 Complex to implement
- 🟡 Still uses HTTP overhead
- 🟡 Hard to manage timeouts

---

#### WebSocket (Optimal)

```
Client               Server
  │                    │
  ├─ Handshake ───────>│
  │<─ Upgrade ─────────┤
  ║                    ║
  ║══════════════════════ Persistent Connection
  ║                    ║
  ║<───── Data ────────╢ Server pushes: "25.3°C"
  ║                    ║
  ║────── Data ───────>║ Client sends: "Set LED Red"
  ║                    ║
  ║<───── Data ────────╢ Server confirms: "LED: Red"
  ║                    ║
```

**Advantages:**

- ✅ Instant updates (no polling)
- ✅ Minimal overhead (2-6 bytes per message)
- ✅ Simple implementation
- ✅ True bi-directional communication

---

## WebSocket Protocol

### RFC 6455

WebSocket is standardized as **RFC 6455** (2011).

**Protocol URI schemes:**

- `ws://` - Unsecured WebSocket (like HTTP)
- `wss://` - Secured WebSocket over TLS (like HTTPS)

**Default ports:**

- `ws://` → Port 80
- `wss://` → Port 443
- Custom ports allowed (e.g., `ws://192.168.1.100:81`)

---

### Protocol Layers

```
┌──────────────────────────────────┐
│   Application (JSON, text, etc)  │ ← Your data
├──────────────────────────────────┤
│   WebSocket Protocol (RFC 6455)  │ ← Framing, control
├──────────────────────────────────┤
│   TCP (Transmission Control)     │ ← Reliable transport
├──────────────────────────────────┤
│   IP (Internet Protocol)         │ ← Routing
└──────────────────────────────────┘
```

---

## Connection Lifecycle

### 1. Opening Handshake

WebSocket starts as HTTP, then **upgrades** to WebSocket protocol.

#### Client Request (HTTP)

```http
GET /ws HTTP/1.1
Host: 192.168.1.100:81
Upgrade: websocket
Connection: Upgrade
Sec-WebSocket-Key: dGhlIHNhbXBsZSBub25jZQ==
Sec-WebSocket-Version: 13
```

**Key headers:**

- `Upgrade: websocket` - Request protocol upgrade
- `Connection: Upgrade` - Maintain connection
- `Sec-WebSocket-Key` - Random base64 key (security)
- `Sec-WebSocket-Version: 13` - Protocol version

---

#### Server Response (HTTP 101 Switching Protocols)

```http
HTTP/1.1 101 Switching Protocols
Upgrade: websocket
Connection: Upgrade
Sec-WebSocket-Accept: s3pPLMBiTxaQ9kYGzzhZRbK+xOo=
```

**Key headers:**

- `101 Switching Protocols` - Upgrade accepted
- `Sec-WebSocket-Accept` - Computed response (validates handshake)

**After this → Connection switches to WebSocket protocol!**

---

### 2. Data Transfer

```
Client ←══════════════════════════════════→ Server
         Text/Binary/Control Messages
```

**Message types:**

- **Text frames** - UTF-8 text (JSON, etc)
- **Binary frames** - Raw binary data
- **Ping/Pong** - Keep-alive heartbeat
- **Close** - Connection termination

---

### 3. Connection States

```
                 ┌─────────┐
                 │ CLOSED  │
                 └────┬────┘
                      │ new WebSocket()
                      ▼
              ┌───────────────┐
              │  CONNECTING   │ ← Opening handshake
              └───────┬───────┘
                      │ Handshake success
                      ▼
              ┌───────────────┐
              │     OPEN      │ ← Data transfer
              └───────┬───────┘
                      │ close() or error
                      ▼
              ┌───────────────┐
              │   CLOSING     │ ← Close handshake
              └───────┬───────┘
                      │
                      ▼
                 ┌─────────┐
                 │ CLOSED  │
                 └─────────┘
```

**JavaScript API:**

- `WebSocket.CONNECTING = 0`
- `WebSocket.OPEN = 1`
- `WebSocket.CLOSING = 2`
- `WebSocket.CLOSED = 3`

---

### 4. Closing Handshake

```
Client                      Server
  ║                           ║
  ║──── Close Frame ─────────>║
  ║      (status: 1000)       ║
  ║                           ║
  ║<──── Close Frame ─────────║
  ║      (status: 1000)       ║
  ║                           ║
  X                           X
CLOSED                     CLOSED
```

**Common close codes:**

- `1000` - Normal closure
- `1001` - Going away (e.g., page reload)
- `1006` - Abnormal closure (no close frame)
- `1011` - Server error

---

## Message Framing

### Frame Structure

WebSocket messages are sent in **frames**.

```
 0                   1                   2                   3
 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
+-+-+-+-+-------+-+-------------+-------------------------------+
|F|R|R|R| opcode|M| Payload len |    Extended payload length    |
|I|S|S|S|  (4)  |A|     (7)     |             (16/64)           |
|N|V|V|V|       |S|             |   (if payload len==126/127)   |
| |1|2|3|       |K|             |                               |
+-+-+-+-+-------+-+-------------+ - - - - - - - - - - - - - - - +
|     Extended payload length continued, if payload len == 127  |
+ - - - - - - - - - - - - - - - +-------------------------------+
|                               | Masking-key, if MASK set to 1 |
+-------------------------------+-------------------------------+
| Masking-key (continued)       |          Payload Data         |
+-------------------------------- - - - - - - - - - - - - - - - +
:                     Payload Data continued ...                :
+ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - +
|                     Payload Data continued ...                |
+---------------------------------------------------------------+
```

---

### Frame Fields

| Field           | Bits     | Description                                 |
| --------------- | -------- | ------------------------------------------- |
| **FIN**         | 1        | Final fragment (1 = complete message)       |
| **RSV1-3**      | 3        | Reserved (extensions)                       |
| **Opcode**      | 4        | Frame type (text/binary/control)            |
| **MASK**        | 1        | Payload masked? (client → server must be 1) |
| **Payload len** | 7        | Length (0-125, or 126/127 for extended)     |
| **Masking key** | 32       | XOR mask (client → server only)             |
| **Payload**     | Variable | Actual data                                 |

---

### Opcodes

| Opcode | Type         | Description           |
| ------ | ------------ | --------------------- |
| `0x0`  | Continuation | Fragment continuation |
| `0x1`  | Text         | UTF-8 text message    |
| `0x2`  | Binary       | Binary data message   |
| `0x8`  | Close        | Connection close      |
| `0x9`  | Ping         | Heartbeat request     |
| `0xA`  | Pong         | Heartbeat response    |

---

### Masking (Security)

**Client → Server:** MUST be masked  
**Server → Client:** MUST NOT be masked

**Why?**  
Prevents cache poisoning attacks on intermediary proxies.

**How it works:**

```
Masked Data = Original Data XOR Masking Key

Example:
Original:    "Hello"        = 0x48 0x65 0x6C 0x6C 0x6F
Masking Key: [0x12, ...]   = 0x12 0x34 0x56 0x78 0x90
XOR Result:                 = 0x5A 0x51 0x3A 0x14 0xFF

Server receives, XORs again to get original.
```

---

### Message Fragmentation

Large messages can be split into multiple frames:

```
Frame 1: FIN=0, Opcode=Text,   Payload="Hello "
Frame 2: FIN=0, Opcode=Cont,   Payload="World"
Frame 3: FIN=1, Opcode=Cont,   Payload="!"

→ Reassembled: "Hello World!"
```

**Most implementations handle this automatically.**

---

## Real-Time Communication Patterns

### 1. Request-Response

```javascript
// Client sends request
ws.send(JSON.stringify({
  type: "ping"
}));

// Server responds
{
  "type": "pong",
  "timestamp": 1234567890
}
```

---

### 2. Server Push (Broadcast)

```javascript
// Server pushes data to all clients
for (let client of clients) {
  client.send(
    JSON.stringify({
      type: "sensors",
      temperature: 25.3,
      humidity: 60,
    }),
  );
}
```

---

### 3. Pub/Sub (Subscribe Pattern)

```javascript
// Client subscribes
ws.send(
  JSON.stringify({
    type: "subscribe",
    topic: "sensors",
  }),
);

// Server broadcasts only to subscribers
for (let client of subscribedClients) {
  client.send(sensorData);
}
```

**Our Lab-19 uses this pattern!**

---

### 4. Command-Control

```javascript
// Client sends command
ws.send(JSON.stringify({
  type: "led",
  r: 255,
  g: 0,
  b: 0
}));

// Server executes and acknowledges
{
  "type": "ack",
  "command": "led",
  "status": "success"
}
```

---

## JSON Serialization

### Why JSON?

**JSON (JavaScript Object Notation)** is the standard for WebSocket data:

✅ **Human-readable** - Easy to debug  
✅ **Language-agnostic** - Works everywhere  
✅ **Structured** - Type-safe communication  
✅ **Flexible** - Nested objects/arrays  
✅ **Well-supported** - Libraries available

---

### ESP32 ArduinoJson Library

```cpp
#include <ArduinoJson.h>

// Parse incoming JSON
StaticJsonDocument<200> doc;
deserializeJson(doc, payload);

const char* type = doc["type"];
int value = doc["value"];

// Create outgoing JSON
StaticJsonDocument<200> response;
response["type"] = "ack";
response["status"] = "success";

String output;
serializeJson(response, output);
webSocket.sendTXT(client, output);
```

---

### JSON Message Protocol (Lab-19)

#### Client → Server

```json
// LED Control
{
  "type": "led",
  "r": 255,
  "g": 128,
  "b": 0
}

// Ping
{
  "type": "ping"
}

// Subscribe
{
  "type": "subscribe"
}
```

---

#### Server → Client

```json
// Sensor Data
{
  "type": "sensors",
  "dht_temperature": 25.3,
  "dht_humidity": 60,
  "bmp_temperature": 25.1,
  "bmp_pressure": 1013.25
}

// LED State
{
  "type": "ledState",
  "r": 255,
  "g": 0,
  "b": 0
}

// System Status
{
  "type": "status",
  "uptime": 12345,
  "clients": 2,
  "heap": 256000,
  "rssi": -45
}

// Acknowledgment
{
  "type": "ack",
  "message": "LED updated"
}

// Error
{
  "type": "error",
  "message": "Invalid command"
}
```

---

### Protocol Design Best Practices

1. **Always include `type` field** - Identifies message purpose
2. **Use consistent naming** - camelCase or snake_case
3. **Include timestamps** - For logging/debugging
4. **Add error handling** - Graceful degradation
5. **Version your protocol** - For future compatibility

```json
{
  "version": "1.0",
  "type": "sensors",
  "timestamp": 1234567890,
  "data": { ... }
}
```

---

## Client-Server Architecture

### ESP32 WebSocket Server

```
┌─────────────────────────────────────┐
│         ESP32 Server                │
│                                     │
│  ┌────────────────────────────┐    │
│  │   WiFi Connection          │    │
│  └────────────────────────────┘    │
│                                     │
│  ┌────────────────────────────┐    │
│  │   WebSocket Server :81     │◄───┼─── ws://IP:81/ws
│  │   - Client management      │    │
│  │   - Message routing        │    │
│  │   - Broadcast support      │    │
│  └────────────────────────────┘    │
│                                     │
│  ┌────────────────────────────┐    │
│  │   HTTP Server :80          │◄───┼─── http://IP/
│  │   - Static files           │    │
│  │   - REST endpoints         │    │
│  └────────────────────────────┘    │
│                                     │
│  ┌────────────────────────────┐    │
│  │   Application Logic        │    │
│  │   - Sensor reading         │    │
│  │   - LED control            │    │
│  │   - Data processing        │    │
│  └────────────────────────────┘    │
│                                     │
│  ┌────────────────────────────┐    │
│  │   Hardware (GPIO)          │    │
│  │   - DHT11 sensor           │    │
│  │   - BMP180 sensor          │    │
│  │   - RGB LED (PWM)          │    │
│  └────────────────────────────┘    │
└─────────────────────────────────────┘
```

---

### Multiple Client Management

```cpp
// Track connected clients
#define MAX_CLIENTS 8

struct ClientInfo {
  uint8_t num;
  bool connected;
  bool subscribedToSensors;
  unsigned long lastPing;
};

ClientInfo clients[MAX_CLIENTS];

// Broadcast to all subscribed clients
void broadcastSensorData() {
  String json = createSensorJSON();

  for (int i = 0; i < MAX_CLIENTS; i++) {
    if (clients[i].connected &&
        clients[i].subscribedToSensors) {
      webSocket.sendTXT(clients[i].num, json);
    }
  }
}
```

---

### Event-Driven Architecture

```cpp
void webSocketEvent(uint8_t num,
                    WStype_t type,
                    uint8_t * payload,
                    size_t length) {

  switch(type) {
    case WStype_DISCONNECTED:
      handleDisconnect(num);
      break;

    case WStype_CONNECTED:
      handleConnect(num);
      break;

    case WStype_TEXT:
      handleMessage(num, payload, length);
      break;

    case WStype_ERROR:
      handleError(num);
      break;
  }
}
```

---

## ESP32 Implementation

### WebSocketsServer Library

**By Markus Sattler**

```cpp
#include <WebSocketsServer.h>

// Create server on port 81
WebSocketsServer webSocket = WebSocketsServer(81);

void setup() {
  // Initialize
  webSocket.begin();

  // Register event handler
  webSocket.onEvent(webSocketEvent);
}

void loop() {
  // Handle incoming messages
  webSocket.loop();
}
```

---

### Memory Management

**ESP32 has limited memory!**

```
ESP32 RAM:
- Total: ~520 KB
- Available: ~300 KB (after WiFi/BT)
- Per client: ~2-4 KB (WebSocket buffers)
- Max clients: 8-10 realistic
```

**Optimization strategies:**

1. **Use StaticJsonDocument** (not DynamicJsonDocument)

   ```cpp
   StaticJsonDocument<200> doc;  // Stack allocation
   ```

2. **Limit message size**

   ```cpp
   if (length > 512) {
     return;  // Reject large messages
   }
   ```

3. **Reuse buffers**

   ```cpp
   static char buffer[256];  // Reuse across calls
   ```

4. **Monitor heap**
   ```cpp
   Serial.printf("Free heap: %d\n", ESP.getFreeHeap());
   ```

---

## Performance Considerations

### Latency

**Typical latency breakdown:**

```
Component                  Latency
────────────────────────────────────
WiFi transmission          1-5 ms
WebSocket processing       <1 ms
JSON serialization         1-2 ms
Sensor reading            10-100 ms
────────────────────────────────────
Total (typical)           15-110 ms
```

**Optimization:**

- ✅ Use JSON efficiently (small messages)
- ✅ Read sensors in separate task
- ✅ Cache sensor values
- ✅ Reduce update frequency

---

### Bandwidth

**Message sizes:**

```
HTTP Request:              ~800 bytes
HTTP Response:             ~500 bytes
────────────────────────────────────
Total per poll:           ~1300 bytes

WebSocket frame overhead:   2-6 bytes
JSON sensor data:          ~150 bytes
────────────────────────────────────
Total per update:          ~156 bytes

Bandwidth savings:         8x less!
```

**At 1 update/second:**

- HTTP polling: 1.3 KB/s = 10.4 Kbps
- WebSocket: 156 B/s = 1.2 Kbps

**Scales with multiple clients:**

- 10 clients × 1.2 Kbps = 12 Kbps (WebSocket)
- 10 clients × 10.4 Kbps = 104 Kbps (HTTP)

---

### Concurrency

**ESP32 FreeRTOS:**

```cpp
// Sensor task (Core 0)
xTaskCreatePinnedToCore(
  sensorTask,        // Function
  "SensorTask",      // Name
  4096,              // Stack size
  NULL,              // Parameters
  1,                 // Priority
  NULL,              // Handle
  0                  // Core
);

// WebSocket runs on Core 1 (loop)
void loop() {
  webSocket.loop();  // Handle connections
  httpServer.handleClient();  // Handle HTTP
}
```

**Benefits:**

- Sensor reading doesn't block WebSocket
- Multiple clients handled efficiently
- Smooth real-time updates

---

## Security

### Threats

1. **Man-in-the-Middle (MITM)**
   - Attacker intercepts traffic
   - Reads/modifies messages
2. **Unauthorized Access**
   - Anyone on network can connect
   - Send malicious commands

3. **Denial of Service (DoS)**
   - Flood with connections
   - Exhaust ESP32 memory

---

### Mitigations

#### 1. Use WSS (WebSocket Secure)

```cpp
// Add TLS/SSL encryption
#include <WiFiClientSecure.h>
WebSocketsServer webSocket =
  WebSocketsServer(443, "/ws", "");  // Port 443
```

**Encrypts all traffic** (like HTTPS)

---

#### 2. Authentication

```cpp
void handleConnect(uint8_t num, String url) {
  // Check for API key
  if (!url.contains("?key=SECRET123")) {
    webSocket.disconnect(num);
    Serial.println("Unauthorized connection rejected");
    return;
  }

  clients[num].authenticated = true;
}
```

---

#### 3. Rate Limiting

```cpp
unsigned long lastCommand[MAX_CLIENTS] = {0};
const unsigned long COMMAND_RATE_LIMIT = 100;  // ms

void handleMessage(uint8_t num, String msg) {
  // Check rate limit
  unsigned long now = millis();
  if (now - lastCommand[num] < COMMAND_RATE_LIMIT) {
    return;  // Ignore (too fast)
  }
  lastCommand[num] = now;

  // Process message...
}
```

---

#### 4. Input Validation

```cpp
void handleLED(JsonDocument& doc) {
  // Validate range
  int r = constrain(doc["r"], 0, 255);
  int g = constrain(doc["g"], 0, 255);
  int b = constrain(doc["b"], 0, 255);

  setRGBColor(r, g, b);
}
```

---

#### 5. WiFi Security

```cpp
// Use WPA2 (not open WiFi!)
WiFi.begin(ssid, password);

// Optional: Restrict to specific MAC addresses
WiFi.softAPmacAddress(mac);  // Get MAC
// Configure router to allow only known MACs
```

---

## Use Cases

### Perfect for WebSocket:

✅ **Real-time dashboards** (IoT sensor monitoring)  
✅ **Live data visualization** (charts, graphs)  
✅ **Remote control** (LED, motors, actuators)  
✅ **Chat applications** (messaging)  
✅ **Multiplayer games** (game state sync)  
✅ **Notifications** (alerts, events)  
✅ **Collaborative apps** (shared editing)  
✅ **Live sports scores** (instant updates)  
✅ **Financial tickers** (stock prices)  
✅ **Video streaming metadata** (captions, overlays)

---

### Not ideal for WebSocket:

❌ **File uploads** (use HTTP POST)  
❌ **Static content** (use HTTP GET)  
❌ **SEO-critical pages** (use HTTP)  
❌ **One-time requests** (use HTTP)  
❌ **Large data transfers** (use HTTP/FTP)  
❌ **Infrequent updates** (< 1/minute → use HTTP)

**Rule of thumb:**  
If data updates **frequently** or needs to be **pushed instantly** → WebSocket  
Otherwise → HTTP

---

## Summary

### Key Takeaways

1. **WebSocket = Persistent, Bi-directional Communication**
   - Stays open (no reconnection overhead)
   - Both sides can send anytime
   - Low latency, low bandwidth

2. **Protocol**
   - Starts as HTTP, upgrades to WebSocket
   - Binary framing with opcodes
   - Client-to-server masking required

3. **Message Format**
   - JSON for structured data
   - Type-based message routing
   - Request-response and broadcast patterns

4. **ESP32 Implementation**
   - WebSocketsServer library (Markus Sattler)
   - Event-driven architecture
   - Memory-conscious design (max 8-10 clients)

5. **Security**
   - Use WSS for encryption
   - Implement authentication
   - Rate limiting and input validation

6. **Perfect for Real-Time IoT**
   - Instant sensor updates
   - Remote control with immediate feedback
   - Multiple client synchronization

---

### Further Reading

**Official Specifications:**

- [RFC 6455 - WebSocket Protocol](https://tools.ietf.org/html/rfc6455)
- [MDN WebSocket API](https://developer.mozilla.org/en-US/docs/Web/API/WebSocket)

**Libraries:**

- [WebSocketsServer by Markus Sattler](https://github.com/Links2004/arduinoWebSockets)
- [ArduinoJson v6](https://arduinojson.org/)

**Tools:**

- [WebSocket Test Client](https://www.websocket.org/echo.html)
- [Chrome DevTools Network Tab](chrome://inspect) - Monitor WebSocket

**Related Topics:**

- MQTT (alternative IoT protocol)
- Server-Sent Events (SSE) - one-way server push
- WebRTC (real-time media)

---

**📚 Now you understand WebSocket technology!**

Use this knowledge to build real-time applications, debug protocol issues, and design efficient IoT communication systems. 🚀
