# UDP-Echo

UDP-Echo is a network utility used to test the reachability of some hosts on a User Datagram Protocol (UDP) network and to measure the round-trip time for messages sent from the originating host (server) to destination hosts (clients).

UDP-Echo operates by sending User Datagram Protocol (UDP) echo request packets to the target hosts and waiting for UDP responses. In the process it measures the time from transmission to reception (round-trip time) and records any packet loss. The results of the test are printed in the form of a statistical summary of the response packets received, including the minimum, maximum, and the mean round-trip times.

UDP-Echo may be executed with various command-line switches to enable special operational modes. For example, options include specifying the packet size of the probe, automatic repeated operation for sending a specified count of probes, and setting receiving timeouts.

This utility only supports Linux and OS X.

##Message format

<table class="wikitable" style="float: right; margin-left: 1.5em; margin-right: 0; margin-top: 0; text-align: center;">
<caption>IP Datagram</caption>
<tr>
<th>&#160;</th>
<th scope="col" style="width:110px;">Bits 0–7</th>
<th scope="col" style="width:110px;">Bits 8–15</th>
<th scope="col" style="width:110px;">Bits 16–23</th>
<th scope="col" style="width:110px;">Bits 24–31</th>
</tr>
<tr style="background:#e0e0ff;">
<th rowspan="5">IP Header<br />
(20 bytes)</th>
<td>Version/IHL</td>
<td>Type of service</td>
<td colspan="2">Length</td>
</tr>
<tr style="background:#e8e8ff;">
<td colspan="2">Identification</td>
<td colspan="2"><i>flags</i> and <i>offset</i></td>
</tr>
<tr style="background:#e8e8ff;">
<td>Time To Live (TTL)</td>
<td>Protocol</td>
<td colspan="2">Checksum</td>
</tr>
<tr style="background:#e8e8ff;">
<td colspan="4">Source IP address</td>
</tr>
<tr style="background:#e8e8ff;">
<td colspan="4">Destination IP address</td>
</tr>
<tr style="background:#ffe8e8;">
<th rowspan="2">UDP Header<br />
(8 bytes)</th>
<td colspan="2">Source Port</td>
<td colspan="2">Destination port</td>
</tr>
<tr style="background:#ffe8e8;">
<td colspan="2">Length</td>
<td colspan="2">CheckSum</td>
</tr>
<tr style="background:#ffe8e8;">
<th>UDP Payload<br />
(<i>optional</i>)</th>
<td colspan="4">Payload Data</td>
</tr>
</table>

User can specify a UDP payload length and server will generate a random string to fill the payload. Client will echo a UDP packet with the same payload after receiving the packet.

##Usage

###Generic options

* -h [ --help ]

    Display help message. This option is incompatible with any other option.

* -v [ --version ]

    Display version number. This option is incompatible with any other option.
    
* -p [ --port ] *port*

    Speciy listen/send port for client/server. This option is mandatory for client. If you don't specify a port for a server, system will automatically allocate a free port. This option is compatible with server options or client options.
    
###Server options

All server options is incompatible with client options.

* -i [ --inputfile ] *filename*

    Specify path of the file contains all clients address. An address consists of IPv4 and port seperated with space, tab or linebreak. Addresses should also be seperated by space, tab or linebreak. Below is a legal input.
    
    ```
    127.0.0.1 1234
    192.168.1.100 1234
    192.168.1.101 5555
    192.168.1.101 3000 
    ```
    
* -c [ --count ] *count*

    Stop after sending *count* packets. The default value is 5.
    
* -t [ --timeout ] *value*

    Specify a timeout, in seconds. The default value is 3. If this is set to zero, it will never timeout.

* -s [ --packetsize ] *value*

    Specify the number of data bytes to be sent in each packet. The default value is 28, which is size of UDP header and IPv4 header. Value less than 28 will be ignored. Don't set this value too large which exceeds the limit for the data length imposed by the underlying IPv4 protocol.

###Client options

* -l [ --listen ] 

    Listen for incoming messages. This option is mandatory for client and only compatible with -p [ --port ] option.

##Example
Supposing we have a server S (192.168.1.100) and three clients A (192.168.1.101: 8888), B (192.168.1.102: 9999) and C (192.168.1.104: 4444), of which B is not working.

The address configuration is saved in file *clients*, whose content is

```
192.168.1.104 4444
127.0.0.1 5555
192.168.1.101 8888
192.168.1.102 9999
```

Run command `./UDP-Echo -l -p 8888` on client A, run command `./UDP-Echo -l -p 9999` on Client C, run `./UDP-Echo -l -p 5555` on Server S.

Run command `./udp_echo -i clients -s 128 -p 10086 -c 5 -t 2` on Server S.
This command specifies the configuration file, sets packet size to 128 bytes, specifies sending port as 10086, sends 5 packets to each client and waits at most two seconds for response.

The client will output something like 

```
Client received 128 bytes message from 192.168.1.100:10086
Client received 128 bytes message from 192.168.1.100:10086
Client received 128 bytes message from 192.168.1.100:10086
Client received 128 bytes message from 192.168.1.100:10086
Client received 128 bytes message from 192.168.1.100:10086
```

The server will output

```
Server sent 128 bytes message to 192.168.1.102:9999, seq = 0
Server sent 128 bytes message to 192.168.1.101:8888, seq = 0
Server sent 128 bytes message to 127.0.0.1:5555, seq = 0
Server sent 128 bytes message to 192.168.1.104:4444, seq = 0
Server received 128 bytes message from 127.0.0.1:5555, seq = 0, time = 0.251ms
Server sent 128 bytes message to 127.0.0.1:5555, seq = 1
Server received 128 bytes message from 127.0.0.1:5555, seq = 1, time = 0.166ms
Server sent 128 bytes message to 127.0.0.1:5555, seq = 2
Server received 128 bytes message from 127.0.0.1:5555, seq = 2, time = 0.153ms
Server sent 128 bytes message to 127.0.0.1:5555, seq = 3
Server received 128 bytes message from 127.0.0.1:5555, seq = 3, time = 0.154ms
Server sent 128 bytes message to 127.0.0.1:5555, seq = 4
Server received 128 bytes message from 127.0.0.1:5555, seq = 4, time = 0.152ms
Server received 128 bytes message from 192.168.1.104:4444, seq = 0, time = 126.357ms
Server sent 128 bytes message to 192.168.1.104:4444, seq = 1
Server received 128 bytes message from 192.168.1.101:8888, seq = 0, time = 126.814ms
Server sent 128 bytes message to 192.168.1.101:8888, seq = 1
Server received 128 bytes message from 192.168.1.104:4444, seq = 1, time = 5.95ms
Server sent 128 bytes message to 192.168.1.104:4444, seq = 2
Server received 128 bytes message from 192.168.1.101:8888, seq = 1, time = 6.864ms
Server sent 128 bytes message to 192.168.1.101:8888, seq = 2
Server received 128 bytes message from 192.168.1.104:4444, seq = 2, time = 5.935ms
Server sent 128 bytes message to 192.168.1.104:4444, seq = 3
Server received 128 bytes message from 192.168.1.104:4444, seq = 3, time = 1.911ms
Server sent 128 bytes message to 192.168.1.104:4444, seq = 4
Server received 128 bytes message from 192.168.1.104:4444, seq = 4, time = 2.078ms
Server received 128 bytes message from 192.168.1.101:8888, seq = 2, time = 9.205ms
Server sent 128 bytes message to 192.168.1.101:8888, seq = 3
Server received 128 bytes message from 192.168.1.101:8888, seq = 3, time = 3.02ms
Server sent 128 bytes message to 192.168.1.101:8888, seq = 4
Server received 128 bytes message from 192.168.1.101:8888, seq = 4, time = 2.318ms
Server's waiting for message from 192.168.1.102:9999 timed out seq = 0 time = 2149.34ms
Server sent 128 bytes message to 192.168.1.102:9999, seq = 1
Server's waiting for message from 192.168.1.102:9999 timed out seq = 1 time = 2000.86ms
Server sent 128 bytes message to 192.168.1.102:9999, seq = 2
Server's waiting for message from 192.168.1.102:9999 timed out seq = 2 time = 2000.85ms
Server sent 128 bytes message to 192.168.1.102:9999, seq = 3
Server's waiting for message from 192.168.1.102:9999 timed out seq = 3 time = 2000.87ms
Server sent 128 bytes message to 192.168.1.102:9999, seq = 4
Server's waiting for message from 192.168.1.102:9999 timed out seq = 4 time = 2000.82ms

--- statistics ---
Client 192.168.1.102:9999 : Packet transmitted/received/loss = 5/0/5
Client 192.168.1.101:8888 : Packet transmitted/received/loss = 5/5/0, round-trip min/avg/max = 2.318/29.6442/126.814 ms
Client 127.0.0.1:5555 : Packet transmitted/received/loss = 5/5/0, round-trip min/avg/max = 0.152/0.1752/0.251 ms
Client 192.168.1.104:4444 : Packet transmitted/received/loss = 5/5/0, round-trip min/avg/max = 1.911/28.4462/126.357 ms
```

##References 
1. [User Datagram Protocol](http://en.wikipedia.org/wiki/User_Datagram_Protocol)
2. [Internet Protocol version 4](http://en.wikipedia.org/wiki/IPv4)
3. [RFC 862 - Echo Protocol](http://www.rfc-base.org/txt/rfc-862.txt)

##LICENSE
Copyright (c) 2015 Jamis Hoo. See the LICENSE file for license rights and limitations(MIT).
