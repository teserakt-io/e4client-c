# Testing Tool e4clic 

e4clic is a Linux command-line testing tool for E4 clients.

The basic E4 C client functionality is provided by `e4client.c` and 
`e4client.h`. These components are designed to be lightweight and
are shared across platform. 

They are only dependant on SHA3 and AES256-SIV implementation, which
use an interfaces defined in `crypto/
in `mro.c` and `mro.h`, for which size-optimized versions exist for low-end
embedded targets. 

This version of E4C uses the PAHO MQTT C Client library
for networking.


## Build Dependencies

A `Makefile` is provided for building the `e4clic` command line tool.

You can take care of most build requirements in an Ubuntu system with 
`sudo apt install gcc make cmake gcc libssl-dev`

The only external build dependency is for the Paho MQTT C Client Library, or
more specifically the `MQTTClient.h` header file and `libpaho-mqtt3c`
library file. See the project homepage at https://www.eclipse.org/paho/clients/c/ and github sources at https://github.com/eclipse/paho.mqtt.c

You may install the libraries system-wide or you may use the provided
`build_paho-mqtt3c.sh` script to fetch the library, compile it, and
a provide symbolic link to the static library.


## Usage

The utility is invoked with two command line parameters:
```
e4clic <Broker> <ClientId>
```
The first parameter to e4clic is the location for MQTT broker,
typically located on port 1883, and second parameter is the client id
for this particular host.
```
$ ./e4clic tcp://test.mosquitto.org:1883 Alice
!!! broker = tcp://test.mosquitto.org:1883
!!! my client id = Alice
!!! command topic = e4/Alice
```
The test client has a bunch of single-letter commands prefixed with
exclamation mark (`!`):
```
?
--- e4clic has single-letter commands prefixed with '!'
--- <message>           Broadcast message to current topic.
--- !t <topic>          Change current topic.
--- !s <topics>         Subscribe (and change current) to topics.
--- !u <topics>         Unsubscribe from topics.
--- !z <clients>        Locally zeroise (set to default) Client's IdKey.
--- !0 <clients>        Send RemoveTopic(current) command.
--- !1 <clients>        Send ResetTopics() command.
--- !2 <key> <clients>  Send SetIdKey(key) command.
--- !3 <key> <clients>  Send SetTopicKey(current, key).
--- !l                  Dump all topic keys.
--- !x  or  !q          Disconnect and quit.
```

## Tutorial

We will illustrate basic usage logic of e4clic with these examples.

### Exchanging messages

Let's establish two clients, "Alice" and "Bob" and subscribe them to topic 
`/hello`. On first terminal window:

```
$ ./e4clic tcp://test.mosquitto.org:1883 Alice
!!! broker = tcp://test.mosquitto.org:1883
!!! my client id = Alice
!!! command topic = e4/Alice
!s /hello
!!! Subscribed to /hello
```
On second:
```
./e4clic tcp://test.mosquitto.org:1883 Bob
!!! broker = tcp://test.mosquitto.org:1883
!!! my client id = Bob
!!! command topic = e4/Bob
!s /hello
!!! Subscribed to /hello

```
Now, if you enter a line of text (without `!`), it will be broadcat on the
topic `/hello` without encryption, as encryption keys are not set:
```
Hello, World!
>>> (!105) /hello:
000 Hello, World!      48 65 6C 6C 6F 2C 20 57 6F 72 6C 64 21
<<< (!104) /hello:
000 Hello, World!      48 65 6C 6C 6F 2C 20 57 6F 72 6C 64 21
```
The fist debug line indicates a sent message (`>>>`), and error 105,
TopicKeyMissing. Both clients recevie the message (`<<<`) with error 104,
indicating that the message is too short to be encrypted.

### Setting Topic Keys

By default the command channel key is set to to all zeros, which can be
easily verified with `!l` command. In case of Alice:
```
!l
!!!  0 topic hash = C8BB7B9AA30C7322C6F0A894699FBD2917E371CB60973AFA9FFA37C7AF6877FF
!!!  0  topic key = 00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000
```
We will use the `!z` command to make Alice aware of Bob's zero command key
```
!z Bob
```
This does not create any traffic, but the other client command key is
now visible via `!l`. We can now set topic key for current topic (previously 
set to `/hello`)
with 
```
!3 key Bob
```
A sha3("key") is set both locally and on remote machine for the current
topic (set via `!t` or `!s`).
```
!!! SetTopicKey(/hello, 99B0188470D1F47F4CA39B9964A97E78644B2E103CAA35EB918AB6494E0F0626F994D5AE441D017F8010252A805B50E646D5D5676B752D45477C34182F59D632) -> e4/Bob
>>> (ok) e4/Bob:
000 Y{<[....;*.X.J.=   59 7B 3C 5B 00 00 00 00 3B 2A 15 58 80 4A 95 3D
010 W.J&.....jk...r.   57 19 4A 26 04 90 0D A7 7F 6A 6B A2 A9 96 72 9A
020 H.G.u...r.G.A..Y   48 DC 47 92 75 BB 9D 8F 72 8D 47 CD 41 CB 9A 59
030 ......s9;f..5..&   B8 E7 85 81 AA 1E 73 39 3B 66 A6 F9 35 B0 9E 26
040 .!.7.r...;..j...   9F 21 B3 37 C7 72 AC ED EB 3B E9 83 6A 12 C3 8C
050 ..0.\......jT.f.   D2 89 30 BC 5C 09 F1 FD FD EB DD 6A 54 CF 66 86
060 ...g.S..q..B..Y#   A5 06 F2 67 DD 53 D9 08 71 D7 E2 42 E1 B8 59 23
070 .....1...          C8 FA D0 BF BB 31 B8 05 CF
```
We may now broadcast and reveive encrypted messages:
```
hello
>>> (ok) /hello:
000 ..<[.....})..1..   87 7F 3C 5B 00 00 00 00 FE 7D 29 DC A8 31 AE 0D
010 .mE.v....j.3.      C1 6D 45 83 76 CE EC 9B F6 6A D3 33 AE
<<< (ok) /hello:
000 hello              68 65 6C 6C 6F
```
We can see the ciphertext and that the current client also receives and decrypts its own message.


### Setting Client ID Keys

We Alice can set the command key for Bob (assuming that she knows the previous one -- typically zero if `!z Bob` is called):

```
!2 bobkey Bob
!!! SetIdKey(FFA50B52DB77E2D4535B2E1FA846B14A89DF142802865BBFA8CCEC39EC35E13120EA8D2B16C9BAD7B2F513A6E851F7BE5EB97769D40832136810BF6987815E9F) -> e4/Bob
>>> (ok) e4/Bob:
000 ..<[....K....W.c   BF 7F 3C 5B 00 00 00 00 4B AE E7 83 B5 57 0C 63
010 Nv..u'..Y.\..g!@   4E 76 CD EB 75 27 06 8D 59 CE 5C 2E A8 67 21 40
020 ....#qi*.e0...T.   06 B0 94 90 23 71 69 2A 9D 65 30 8B FC E8 54 F9
030 Uu.....R..vlv.yr   55 75 CF 96 BA C7 E8 52 92 84 76 6C 76 E1 79 72
040 J._....9vTc.+..M   4A CF 5F 14 B5 FD EF 39 76 54 63 0A 2B CC DE 4D
050 ..\;....E          F9 9E 5C 3B 2E 89 C7 AD 45
```

This can be repeated a number of times, each time the key replaces the previous one, which can be verified with `!l`

