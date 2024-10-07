# srs
session record server

### **Provided SIP Message:**

```sip
Max-Forwards: 70
Content-Length: 2414
User-Agent: OpenSIPS (3.5.0-dev (x86_64/linux))
Require: siprec
Content-Type: multipart/mixed;boundary=OSS-unique-boundary-42
```

### **Optional Headers Present in the Message:**

While not required, the following headers are commonly used to provide additional information:

1. **User-Agent**
    - **Purpose:** Identifies the software agent originating the request.
    - **Example:**
      ```
      User-Agent: OpenSIPS (3.5.0-dev (x86_64/linux))
      ```

2. **Require**
    - **Purpose:** Indicates that the server must support certain SIP extensions to process the request.
    - **Example:**
      ```
      Require: siprec
      ```
    - **Note:** In this case, `siprec` refers to the SIP Recording extension, signaling that the request requires support for recording sessions as per the [SIPREC specifications](https://tools.ietf.org/html/draft-ietf-siprec-siprec-framework-13).

### **Additional Insights:**

- **`Require: siprec` Header:**
    - This header indicates that the sender requires the SIP REC (Recording) extension to be supported by all SIP entities that handle this request. If any proxy or server in the request's path does not support this extension, it should respond with a `420 Bad Extension` error.

- **`User-Agent` Header:**
    - While not required, it provides valuable information about the software handling the SIP request, which can be useful for troubleshooting and compatibility checks.

### **Require**

   ```sip
   Require: siprec
   ```

- **Purpose:**
    - **Specify Required SIP Extensions:** Indicates that the sender requires support for specific SIP extensions to process the request.

- **Components:**
    - **Extension Token:** `siprec` refers to the SIP Recording extension.

  - **Usage:**
      - SIP entities receiving the request must support all extensions listed in the `Require` header.
      - If any required extension isn't supported, the receiver must respond with a `420 Bad Extension` error.

  - **About `siprec`:**
      - **SIP Recording (`siprec`):** Defined in [RFC 7866](https://datatracker.ietf.org/doc/html/rfc7866), `siprec` facilitates the recording of SIP-based media sessions.
      - **Functionality:** Allows for the passive recording of calls by intercepting media streams without interfering with the actual communication between endpoints.
      - **Use Cases:** Common in call centers, legal recordings, and compliance scenarios.

---------------------------------------------------------
Certainly! Let's delve into the **`Supported: 100rel`** SIP header, exploring its purpose, functionality, usage scenarios, and how to implement or handle it within **PJSIP**. This comprehensive guide will provide you with a thorough understanding of this SIP extension and its practical applications.

---

## **Table of Contents**

1. [Introduction to `100rel`](#introduction-to-100rel)
2. [Understanding the `Supported` Header](#understanding-the-supported-header)
3. [Purpose and Functionality of `Supported: 100rel`](#purpose-and-functionality-of-supported-100rel)
4. [Usage Scenarios](#usage-scenarios)
5. [Practical SIP Message Examples](#practical-sip-message-examples)
6. [Implementing and Handling `Supported: 100rel` in PJSIP](#implementing-and-handling-supported-100rel-in-pjsip)
    - [Adding `Supported: 100rel` to Outgoing SIP Messages](#adding-supported-100rel-to-outgoing-sip-messages)
    - [Processing Incoming `Supported: 100rel` Headers](#processing-incoming-supported-100rel-headers)
7. [Best Practices and Considerations](#best-practices-and-considerations)
8. [Additional Resources](#additional-resources)
9. [Conclusion](#conclusion)

---

## **Introduction to `100rel`**

### **What is `100rel`?**

- **`100rel`** stands for **"Reliable Provisional Responses"** in SIP (Session Initiation Protocol).
- It is defined in **[RFC 3262](https://datatracker.ietf.org/doc/html/rfc3262)**.
- The extension enhances SIP's reliability by ensuring that provisional responses (like `100 Trying`, `180 Ringing`, etc.) are delivered reliably, similar to final responses (`200 OK`, `486 Busy Here`, etc.).

### **Why was `100rel` Introduced?**

- **Reliability Issues:** In standard SIP, provisional responses are sent unreliably over UDP, which can lead to issues like missed signaling, causing SIP dialogs to be established incorrectly.
- **Enhanced User Experience:** Reliable delivery of provisional responses ensures that call progress indicators (like ringing) are accurately reflected on the caller's side.
- **Support for Advanced Features:** Reliable provisional responses are essential for certain SIP features, such as call forking and early media handling.

---

## **Understanding the `Supported` Header**

### **Purpose of the `Supported` Header**

- The **`Supported`** header in SIP is used to **declare the SIP extensions** that the sender **supports**.
- It allows SIP entities (like user agents, proxies, and servers) to **advertise their capabilities**, enabling more informed decision-making during SIP message exchanges.

### **Syntax of the `Supported` Header**

```plaintext
Supported: <token1>, <token2>, ..., <tokenN>
```

- **`<token>`** represents the name of a SIP extension.
- Multiple extensions can be listed, separated by commas.

### **Example:**

```plaintext
Supported: 100rel, replaces, timer
```

- Indicates support for:
    - **`100rel`**: Reliable Provisional Responses
    - **`replaces`**: Replacing existing dialogs
    - **`timer`**: SIP Timer Framework for managing transaction timeouts

---

## **Purpose and Functionality of `Supported: 100rel`**

### **Why Use `Supported: 100rel`?**

Including **`Supported: 100rel`** in SIP messages communicates to the recipient that the sender can handle reliable provisional responses. This enables both parties to leverage the enhanced reliability features provided by the `100rel` extension.

### **How `100rel` Enhances SIP Communication**

1. **Reliable Delivery of Provisional Responses:**
    - Ensures that provisional responses (e.g., `180 Ringing`) are acknowledged and retransmitted if necessary, similar to how final responses are handled.

2. **State Management:**
    - Helps in maintaining accurate call state information, preventing scenarios where a caller is unaware of call progress due to lost provisional responses.

3. **Support for Advanced Features:**
    - Essential for features like **call forking**, where multiple branches may be handling a single SIP request.

### **Interaction with Other SIP Headers**

- **`Require: 100rel`**:
    - When a SIP message includes **`Require: 100rel`**, it mandates that the recipient **must** support the `100rel` extension to process the request.

- **`Proxy-Require: 100rel`**:
    - Similar to `Require`, but specifically enforces that **intermediate proxies** must support `100rel`.

- **`Supported: 100rel`**:
    - **Declares support** without mandating its use. It allows the recipient to decide whether to utilize the `100rel` features.

---

## **Usage Scenarios**

1. **Call Progress Indication:**
    - Ensuring that callers receive accurate call progress updates, such as ringing, by reliably delivering provisional responses.

2. **Call Forking:**
    - In scenarios where a SIP request forks to multiple endpoints, `100rel` ensures that each forked branch can reliably communicate provisional responses.

3. **Early Media Handling:**
    - Facilitating the exchange of media before the call is fully established, relying on reliable provisional signaling.

4. **Network Reliability Improvements:**
    - Enhancing the robustness of SIP communications over unreliable transport protocols like UDP.

---

## **Practical SIP Message Examples**

### **1. Outgoing SIP INVITE with `Supported: 100rel`**

```plaintext
INVITE sip:bob@example.com SIP/2.0
Via: SIP/2.0/UDP 192.168.1.100:5060;branch=z9hG4bK776asdhds
Max-Forwards: 70
From: "Alice" <sip:alice@example.com>;tag=1928301774
To: "Bob" <sip:bob@example.com>
Call-ID: a84b4c76e66710@client.example.com
CSeq: 314159 INVITE
Contact: <sip:alice@192.168.1.100:5060>
Supported: 100rel
Content-Type: application/sdp
Content-Length: 142

v=0
o=alice 53655765 2353687637 IN IP4 192.168.1.100
s=Example Session
c=IN IP4 192.168.1.100
t=0 0
m=audio 5004 RTP/AVP 0
a=sendrecv
```

**Explanation:**

- **`Supported: 100rel`**: Indicates that Alice's SIP client supports reliable provisional responses.
- This allows Bob's SIP client to utilize `100rel` features, enhancing call setup reliability.

### **2. SIP Response Utilizing `100rel`**

```plaintext
SIP/2.0 180 Ringing
Via: SIP/2.0/UDP 192.168.1.100:5060;branch=z9hG4bK776asdhds
From: "Alice" <sip:alice@example.com>;tag=1928301774
To: "Bob" <sip:bob@example.com>;tag=314159
Call-ID: a84b4c76e66710@client.example.com
CSeq: 314159 INVITE
Supported: 100rel
RSeq: 1
Require: 100rel
Content-Length: 0
```

**Explanation:**

- **`Supported: 100rel`**: Bob's SIP server declares support for `100rel`.
- **`Require: 100rel`**: Bob's SIP server mandates that reliable provisional responses be used for this call.
- **`RSeq: 1`**: Part of the `100rel` extension, it provides a reliable sequence number for provisional responses.
- This response ensures that the provisional response (`180 Ringing`) is reliably transmitted and acknowledged by Alice's SIP client.

---

In SIP (Session Initiation Protocol), the `Require` header is used to indicate that a certain extension or feature is mandatory for the recipient of the SIP message to understand and support. If the recipient does not support the required feature, it should respond with an error indicating the feature is not supported. This is useful for making sure that both sides of a SIP communication session are compatible in terms of features or capabilities.

Let’s break down each of the specific `Require` values (`siprec`, `100rel`, `replaces`, and `timer`) with detailed explanations and examples.

### 1. `Require: siprec`

- **Explanation**: This indicates that the sender requires support for SIP recording (also known as SIPREC). SIPREC is a protocol that allows for media and signaling to be recorded during a SIP session. When the `Require: siprec` header is present, it tells the other party that it must support session recording as part of the communication session.

- **Use Case**: This might be used in call centers or for compliance purposes where all conversations need to be recorded.

- **Example**:
  ```plaintext
  INVITE sip:agent@company.com SIP/2.0
  Via: SIP/2.0/UDP 203.0.113.1:5060
  From: <sip:user@company.com>;tag=1234
  To: <sip:agent@company.com>
  Call-ID: abcdefg@203.0.113.1
  CSeq: 1 INVITE
  Require: siprec
  Contact: <sip:user@203.0.113.1>
  Content-Length: 0
  ```

- **Meaning**: In this example, the SIP `INVITE` message is being sent to initiate a session, but the sender demands that the receiving agent (callee) supports SIP recording. If the callee does not support SIPREC, it would reject the request.

### 2. `Require: 100rel`

- **Explanation**: This requires the recipient to support *reliable provisional responses* (specifically 1xx responses) as defined in RFC 3262. Normally, provisional responses (like "180 Ringing") are not guaranteed to reach the sender. With `100rel`, these responses must be sent reliably using PRACK (Provisional Acknowledgement).

- **Use Case**: Often used in situations where you need to ensure that intermediate signaling (before the session is fully established) is reliably delivered. It’s common in environments where losing provisional responses could cause issues, like in certain VoIP systems.

- **Example**:
  ```plaintext
  INVITE sip:agent@company.com SIP/2.0
  Via: SIP/2.0/UDP 203.0.113.1:5060
  From: <sip:user@company.com>;tag=1234
  To: <sip:agent@company.com>
  Call-ID: abcdefg@203.0.113.1
  CSeq: 1 INVITE
  Require: 100rel
  Contact: <sip:user@203.0.113.1>
  Content-Length: 0
  ```

- **Meaning**: The `Require: 100rel` indicates that the sender expects the recipient to send reliable provisional responses. For example, if the recipient sends a "180 Ringing" response, it must ensure that the sender acknowledges receiving it via PRACK.

### 3. `Require: replaces`

- **Explanation**: This means the sender requires the recipient to support the `Replaces` header, which is used in call control operations. The `Replaces` header allows a new call to replace an existing call leg (usually used in call transfer or call parking scenarios).

- **Use Case**: Commonly used for advanced call handling features like call transfer. For example, if user A is talking to user B, and user C wants to replace user B, the `Replaces` header facilitates this switch.

- **Example**:
  ```plaintext
  INVITE sip:agent@company.com SIP/2.0
  Via: SIP/2.0/UDP 203.0.113.1:5060
  From: <sip:user@company.com>;tag=1234
  To: <sip:agent@company.com>
  Call-ID: abcdefg@203.0.113.1
  CSeq: 1 INVITE
  Require: replaces
  Contact: <sip:user@203.0.113.1>
  Content-Length: 0
  ```

- **Meaning**: In this case, the `Require: replaces` signals that the sender expects the callee to be able to handle call replacement. If the call involves replacing an existing call leg, the recipient must be able to process it.

### 4. `Require: timer`

- **Explanation**: This indicates that the sender requires the recipient to support session timers as defined in RFC 4028. Session timers are used to keep SIP sessions alive by periodically refreshing them with re-INVITE or UPDATE requests. The timer helps detect dead sessions (e.g., due to network failures) and prevents hanging call legs.

- **Use Case**: Particularly useful in long-duration SIP sessions, where periodic checks are needed to make sure both sides are still active. Common in VoIP systems to ensure that if a call is dropped, it can be detected and cleared.

- **Example**:
  ```plaintext
  INVITE sip:agent@company.com SIP/2.0
  Via: SIP/2.0/UDP 203.0.113.1:5060
  From: <sip:user@company.com>;tag=1234
  To: <sip:agent@company.com>
  Call-ID: abcdefg@203.0.113.1
  CSeq: 1 INVITE
  Require: timer
  Session-Expires: 1800;refresher=uas
  Contact: <sip:user@203.0.113.1>
  Content-Length: 0
  ```

- **Meaning**: Here, the sender expects the callee to support session timers, meaning both sides need to periodically refresh the session to keep it alive. The session will expire after 1800 seconds unless refreshed.

### Summary of Each `Require` Value:
- **`siprec`**: Requires support for SIP session recording.
- **`100rel`**: Requires support for reliable provisional responses (PRACK).
- **`replaces`**: Requires support for call leg replacement (used in call transfers).
- **`timer`**: Requires support for session timers to manage session expiration.

---
## Endpoint.audDevManager().setNullDev()
In PJSIP, `Endpoint.audDevManager().setNullDev()` is used to manage the audio device in the application. Here's a detailed explanation:

- **PJSIP** is a multimedia communication library that implements standard protocols such as SIP (Session Initiation Protocol), SDP (Session Description Protocol), and RTP (Real-Time Transport Protocol). It is commonly used for VoIP (Voice over IP) applications.

- **`Endpoint`**: This is a high-level API in PJSIP that represents the entire application. It provides functions to create and manage different PJSIP components, such as transports, accounts, and audio/video management.

- **`audDevManager()`**: The `audDevManager()` method of `Endpoint` is used to access the **Audio Device Manager**. This component is responsible for managing the audio devices used in the application, such as speakers, microphones, or other audio input/output devices.

- **`setNullDev()`**: The `setNullDev()` method configures PJSIP to use a "null" audio device, meaning no real audio device is used for the media session. This can be useful in scenarios where you want to establish SIP sessions but do not need actual audio transmission. For example:
    - **Testing or Development**: During testing or debugging, you may want to establish SIP sessions without real audio.
    - **Server Applications**: In server-side applications like SIP proxies, there may be a need to manage call sessions without actually transmitting audio.

By using `setNullDev()`, you effectively disable audio capture and playback. The media streams will still be processed, but no sound will be transmitted or played.

Here’s an example use case:
```cpp
// Access the audio device manager
AudioDevManager& adm = Endpoint::instance().audDevManager();

// Set the audio device to null
adm.setNullDev();
```

This will make the PJSIP endpoint avoid interacting with any hardware device for audio, and the media sessions will have no actual audio input/output.