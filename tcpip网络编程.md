**惨痛教训**

当运行结果与预期不合时，就将正确代码（不要看书上的！那个不对齐容易忽略一些行）与自己的代码逐行比对。有时候可能根本不是代码写错了，而是将写好的代码复制粘贴到vim时有的行被挪到上一行注释后面了。我晕。

# tcpip协议详解

## tcpip协议族  

### tcpip协议族体系结构 以及 主要协议

![image-20231208215724533](https://cdn.jsdelivr.net/gh/808bass666/picBed@main/202312132031049.png)

#### 数据链路层 

数据链路层上实现了网卡接口的网络驱动程序，以处理数据在物理媒介（如以太网、令牌环等上的传输）。

数据链路层上常用的两个协议是：（它们都实现了ip和mac地址的相互转换）

- ARP (Address Resolve Protocol)
- RARP (Reverse Address Resolve Protocol) 逆····
    - 仅用于网络上的某些无盘（无存储设备）工作站，它们无法记住自己的ip地址，但可以利用网卡上的物理地址来向网络管理者（服务器或网络管理软件）查询自身的ip

#### 网络层 

网络层上实现数据包的选路和转发。

网络层上最重要的两个协议是：

- IP . 采用逐跳的方式确定通信路径
- ICMP (Internet Control Message Protocol). 它是IP协议的重要补充，主要用于检测网络连接。

> ICMP并非严格意义上的网络层协议，因为它使用处于同一层的IP协议提供的服务。（一般来说，上层协议使用下层协议提供的服务）

ICMP报文格式：

![image-20231208212717903](https://cdn.jsdelivr.net/gh/808bass666/picBed@main/202312132031051.png)

- 8位类型：用于区分报文类型。ICMP报文分为两**大类**：
    - 差错报文。回应网络错误，如目标不可达->3，重定向->5
    - 查询报文。查询网络信息，如查看目标是否可达->8
- 8位代码：用于进一步细分类型（条件）
    - 如重定向报文使用0表示对网络重定向，使用1表示对主机重定向
- 16位校验和：对==整个报文==进行CRC循环冗余校验，以检验报文在传输过程中是否损坏

#### 传输层 

传输层上为两台主机上的**应用程序**提供**端到端**的通信。与网络层使用的逐跳通信方式不同，传输层只关心通信的**起始端和目的端**。

传输层主要的三个协议是：

- TCP .  为应用层提供==可靠的、面向连接的、基于流==的服务
    - 基于流的数据**没有边界（长度）限制**，它源源不断地从通信的一端流入另一端。发送端可以逐个字节地向数据流中写入数据，接收端也可以逐个字节地将它们读出。
- UDP . 为应用层提供==不可靠的、无连接的、基于数据报==的服务
    - 基于数据报的服务：每个UDP数据报都有一个长度，接收端必须以该长度为最小单位，将所有内容**一次性读出**，否则数据将被**截断**。
- SCTP 流控制传输协议(Stream Control Transmission Protocol). 比较新的传输层协议，是为了在因特网上**传输电话信号**而设计的

#### 应用层

应用层负责处理应用程序的逻辑。

数据链路层、网络层和传输层负责处理网络通信细节，这部分必须既稳定又高效，因此它们都在内核空间中实现，如最开始的那张图所示。
而应用层则在用户空间实现，因为它负责处理众多逻辑，如文件传输、名称查询和网络管理等。
当然，也有少数服务器程序是在内核中实现的，这样代码就无须在用户空间和内核空间来回切换（主要是数据的复制)，不过这种代码实现起来较复杂，不够灵活，且不便于移植。

应用层的常用程序：

- ping，它是应用层上的应用程序，而不是协议。其利用ICMP报文检测网络连接，用于调试网络环境。

应用层的部分协议：

- telnet 远程登录协议
- OSPF 开放最短路径优先协议 (Open Shortest Path First)，是动态路由更新协议，用于路由器之间的通信（告知对方各自的路由信息）
- DNS 域名服务协议 (Domain Name Service) ，提供域名和ip间的转换

> 应用层协议（或程序）可能跳过传输层直接使用网络层提供的服务，比如 ping程序和OSPF协议。

### 封装 

![image-20231209213045319](https://cdn.jsdelivr.net/gh/808bass666/picBed@main/202312132031052.png)

![image-20231209214503570](https://cdn.jsdelivr.net/gh/808bass666/picBed@main/202312132031053.png)

TCP和UDP封装的区别：

UDP封装无需为应用层数据保存副本，成功发送之后（不要求成功接收）UDP内核缓冲区中的该数据报就被丢弃了

以太网帧封装：

以太网帧的**数据部分**长度要求至少为46字节。

![image-20231209222100802](https://cdn.jsdelivr.net/gh/808bass666/picBed@main/202312132031054.png)

- 类型(2Byte)：用于标识上层协议。经以太网驱动程序识别后交付给对应模块进行处理。
    - 若为 0x800，则帧的数据部分为：IP数据报
    - 若为 0x806，则帧的数据部分为：ARP报文
    - 若为 0x835，则帧的数据部分为：RARP报文

### 分用 

分用是依靠头部的**类型字段//协议字段**实现的。

比如：

1. 以太网帧头部使用(2Byte)**类型**字段标识上层协议。经以太网驱动程序识别后交付给对应模块进行处理。

    - 若为 0x800，则帧的数据部分为：IP数据报

    - 若为 0x806，则帧的数据部分为：ARP报文

    - 若为 0x835，则帧的数据部分为：RARP报文

2. IP数据报头部使用**协议**字段标识上层协议（TCP, UDP以及**使用IP协议的ICMP**）
3. TCP和UDP数据报头部使用(2Byte)**端口号**区分上层应用

![image-20231209220413631](https://cdn.jsdelivr.net/gh/808bass666/picBed@main/202312132031055.png)

### 测试网络 

### ARP协议 

![image-20231209222238366](https://cdn.jsdelivr.net/gh/808bass666/picBed@main/202312132031056.png)

![image-20231209223554552](https://cdn.jsdelivr.net/gh/808bass666/picBed@main/202312132031057.png)

携带ARP数据的以太网帧与其它以太网帧（如携带IP数据报的以太网帧）的区别：

- ARP请求和应答是从以太网驱动程序发出的，而并非向图纸描述的那样从ARP模块直接发到以太网上。

### DNS协议 

![image-20231209230345517](https://cdn.jsdelivr.net/gh/808bass666/picBed@main/202312132031058.png)

### socket和tcpip协议族的关系

由socket定义的这一组api提供的功能有两点：

1. 将应用程序数据从用户缓冲区--复制到-->TCP/UDP内核**发送**缓冲区（写/传输，如send()），或者从TCP/UDP内核**接收**缓冲区--复制数据到-->用户缓冲区（读/接收）
2. 通过它们可以修改各层协议的头部信息或其它数据结构，从而精细地控制底层通信的行为（如能通过socketopt()设置IP数据报在网络上的存活时间）

> socket是一套通用的网络编程接口，不但可以访问内核中TCP/IP协议栈，还能访问其它网络协议栈

## IP协议详解 

### ip服务的特点

IP协议为上层协议提供**无状态、无连接、不可靠**的服务。

- 无状态：指通信双方不同步传输数据的状态信息，所以IP数据报的传输都是相互独立、没有上下文关系的。

### ipv4头部结构

![image-20231210210510388](https://cdn.jsdelivr.net/gh/808bass666/picBed@main/202312132031059.png)

- 4位版本号：指定IP协议的版本（IPv4为 4 ）

- 4位头部长度：表示IP头部有多少个4字节（4bit最大能表示15，因此IP头部最长是60字节）

- 8位类型服务：应用程序根据实际需要设置服务类型，分为：最小延时、最大吞吐量、最高可靠性、最小费用

    - 如ssh和telnet这样的登录程序就需要最小延时的服务

    - ftp文件传输程序就需要最大吞吐量的服务

- 16位总长度：指整个IP数据报的长度（以字节为单位），因此最长是2^16-1 = 65535个字节。==但由于MTU的限制，长度超过MTU的数据报都将被分片，每个分片的总长度字段都是该分片的长度，因此实际长度远小于最大值==

- 16位标识：唯一地**标识**主机发送的**每一个数据报**。**初始值随机，之后每发送一个数据报，值就+1**。该值在分片时被复制到每个分片中，因此**同一个数据报的所有分片都具有相同的标识值**。

- 3位标志字段：第二位DF表示"Don't Fragment"，第三位MF表示"More Fragment"（除了最后一个分片外，其它分片都要置1）

- 13位分片位移：分片相对原始==IP数据报的**数据部分开始处**==的偏移。

    ![image-20231210203657172](https://cdn.jsdelivr.net/gh/808bass666/picBed@main/202312132031060.png)

- 8位生存时间(TTL)。用于防止数据报陷入路由循环

- 8位协议：用于区分上层协议
    - ICMP是1，TCP是6，UDP是17
- 16位头部校验和：由发送端填充，接收端使用CRC对该IP数据报头部进行检验。

#### ip头部选项

可用的ip选项包括：

- 记录路由：用于跟踪数据报的传递路径

    > `traceroute`程序是更为可靠的记录路由选项的替代品

- 时间戳：用于测量路由之间数据报传输的时间

- 松散源路由选择：指定一个路由器ip地址列表，数据报发送过程中==必须经过==其中的所有路由器（还能经过其它的路由器）

- 严格源路由选择：指定一个路由器ip地址列表，数据报发送过程中==只能经过==其中的所有路由器（不能经过其它的路由器）

    > 使用松散源路由选择和严格源路由选择的例子大概只有`traceroute`程序

### ip分片

分片可能发生在发送端或中转路由器上。最终在目标机器上被内核中的IP模块组装。

ip分片和重组参考三个信息：**标识、标志和片偏移。**

==以太网帧的MTU是1500 Byte==.

![image-20231210212734478](https://cdn.jsdelivr.net/gh/808bass666/picBed@main/202312132031061.png)

第二个IP分片不包含ICMP头部信息，因为IP模块重组该ICMP报文的时候只需要一份ICMP头部信息。

### ip路由

#### ip模块工作流程

1. 当IP模块收到IP数据报时，首先对==头部==做CRC检验，检验不通过，丢弃；否则，往下执行
2. 若头部设置了源站选路选项（松散源路由选项、严格源路由选项），则调用 <u>数据报转发子模块</u> 来处理该数据报。
3. 查看目的ip，看是否是发给自己的（广播也算）。
4. 是发给自己的，则根据头部的 协议字段 来决定交给哪个上层应用（分用）；不是则调用 <u>数据报转发子模块</u> 来处理该数据报。

 <u>数据报转发子模块</u> 的处理流程：

- 首先检测系统是否允许转发
- 不允许就丢弃；允许，将对该数据报执行一些操作，然后把他交给IP数据报 输出子模块 。

![image-20231210215157616](https://cdn.jsdelivr.net/gh/808bass666/picBed@main/202312132031062.png)

> 虚线箭头：路由表更新过程

#### 路由机制

静态的路由更新方式：通过route命令或其他工具手动修改路由表。

动态的、自动的路由更新方式：通过BGP (Border Gateway Protocol，边际网关协议)、RIP ( Routing Information Protocol，路由信息协议)、OSPF等 动态路由协议 协议来发现路径，并更新自己的路由表。

### ip转发 



### 重定向 

#### ICMP重定向报文

ICMP重定向报文也能用于更新路由表。

![image-20231210215627123](https://cdn.jsdelivr.net/gh/808bass666/picBed@main/202312132031063.png)

ICMP重定向报文的**数据部分**，给接收方提供了两个信息：

1. 引起重定向的IP数据报的**源端地址**
2. 应该使用的**路由器的IP地址**。

## TCP协议详解 

### 服务特点

![image-20231210220508820](https://cdn.jsdelivr.net/gh/808bass666/picBed@main/202312132031064.png)

### 头部结构 

> TCP头部最长是60字节。固定部分为20字节，选项部分最多40字节。

 ![image-20240526171623478](https://cdn.jsdelivr.net/gh/808bass666/picBed@main/202405261716518.png)

- 16位源端口和目的端口号
- 32位序号==seq==：seq被初始为随机值ISN，后续=ISN+【报文携带的数据的第一个字节】在整个字节流中的偏移
    - 比如传送的数据是字节流中的第1025~2048字节，那么该报文段的序号值就是ISN+1025
- 32位确认号==ack==：= 收到的报文段的序号值seq+1
- 4位头部长度：标识该TCP头部有多少个4字节. 因此==TCP头部最长为15*8=60字节==
- 6位标志位：
    - URG：标识紧急指针是否有效
    - ACK：表示seq确认号是否有效，携带ACK标志的TCP报文段为==确认报文段==
    - PSH：提示接收端应用程序应立即从TCP接收缓冲区读走数据
    - RST：要求对方重新建立连接，携带RST标志的TCP报文段为==复位报文段==
    - SYN：请求建立连接，携带SYN标志的TCP报文段为==同步报文段==（占用一个序号值）
    - FIN：通知对方本端要关闭连接了，携带SIN标志的TCP报文段为==结束报文段==（占用一个序号值）

- 16位窗口大小==win==：是TCP流量控制的一个手段，这里的窗口指==接收窗口==，它告诉对方本端的TCP接收缓冲区还能容纳多少字节的数据
- 16位校验和：由发送端填充，接收端执行CRC以检验==整个TCP报文==（不仅包括头部，也包括数据部分）是否损坏。**这也是TCP可靠传输的一个重要保障。**
- 16位紧急指针：是一个正的偏移量，它和seq的值相加表示最后一个紧急数据的下一字节的序号

#### TCP头部选项

最多40字节。由7种选项，这里只讲重要的一部分：

- 最大报文段长度（MSS: Max Segment Size）：用于TCP连接初始化时，通信双方使用该选项来协商最大报文段长度。
    - 通常设置为 `(MTU - 40) `字节，减掉的40B是20B的TCP头部和20B的IP头部，这样携带TCP报文段的IP数据报的长度就不会超过MTU（假设TCP和IP头部都不包含选项字段）。
    - 对以太网而言，`MSS = 1500 - 40 = 1460`字节
    - **只能出现在同步报文段**（SYN, Flags[S]）中，否则将被忽略。
- 窗口扩大因子选项：因为实际TCP模块的接收窗口大小远不止头部16位窗口大小能表示的65535，因此需要窗口扩大因子。
    -  `实际接收窗口大小 = TCP头部的16位窗口大小win * (2 ^ 窗口扩大因子)`
    - **只能出现在同步报文段**（SYN, Flags[S]）中，否则将被忽略。
    - 连接建立好后，每个传输方向的窗口扩大因子就固定了。

- 选择性确认选项（SACK）：在连接初始化时设置，是为了改善···的情况而产生的，能使TCP模块只重传丢失的报文段而不用重传所有未被确认的报文段。
- 时间戳选项：较为准确的计算计算通信双方之间的回路时间（RRT），为TCP流量控制提供重要信息。

![image-20231212144841156](https://cdn.jsdelivr.net/gh/808bass666/picBed@main/202312132031066.png)



### TCP状态转移:star:

#### TCP状态说明

- `LISTEN`：监听状态。服务端调用了listen函数，可以开始accept连接了。

- `SYN_SENT`：表示客户端已经发送了SYN连接请求报文。当客户端调用connect函数发起连接时，首先发SYN给服务端，然后自己进入SYN_SENT状态，并等待服务端发送ACK+SYN。

- `SYN_RCVD`（半连接状态）：表示服务端收到了客户端发送SYN报文。服务端收到这个报文后，进入SYN_RCVD状态，将该连接放到半连接队列，然后发送ACK+SYN给客户端

- `ESTABLISHED`（完全连接状态）：表示连接已经建立成功了。服务端发送完ACK+SYN后就会进入该状态，客户端收到ACK后也进入该状态。

- `FIN_WAIT_1`：表示主动关闭连接，等待对方对FIN的确认。无论哪方socket调用close函数发送FIN报文都会进入这个这个状态。

- `FIN_WAIT_2`：主动关闭连接方收到被动关闭方返回的ACK（同意关闭连接）后，会进入该状态，表示等待被动关闭方的FIN。

- `TIME_WAIT`：主动关闭连接方收到了被动关闭方的FIN报文，然后会发送ACK报文过去，进入该状态。等2MSL后即可完全关闭连接。如果FIN_WAIT_1状态下，收到对方同时带FIN标志和ACK标志的报文时，则可以直接进入TIME_WAIT状态，而无须经过FIN_WAIT_2状态。

- `CLOSING`：表示双方同时关闭连接。如果双方几乎同时调用close函数，那么会出现双方同时发送FIN报文的情况，此时就会出现CLOSING状态，表示双方都在关闭连接

- `CLOSE_WAIT`：表示被动关闭方等待关闭。当收到对方调用close函数发送的FIN报文时，回应对方ACK报文，此时进入CLOSE_WAIT状态。

    > 服务器端如果积攒大量的COLSE_WAIT状态的socket，有可能将服务器资源耗尽，进而无法提供服务。

- `LAST_ACK`：表示被动关闭方发送FIN报文后，等待对方的ACK报文状态，当收到ACK后完全关闭连接。


  ![image-20231219211252150](https://cdn.jsdelivr.net/gh/808bass666/picBed@main/202312192112227.png)

#### TCP状态转移总图

由TCP服务的特性可知，**TCP连接的任意一端在任一时刻都处于某种状态**，当前状态可以通过`netstat`命令查看

![image-20231212153117090](https://cdn.jsdelivr.net/gh/808bass666/picBed@main/202312132031069.png)

> 带颜色的箭头都是典型TCP状态转移路线。除了红线代表服务器外，其它所有线都代表的是客户端。
> 另外CLOSED是一个假想的初始状态，并不是一个实际的状态。

先说服务器端**典型**状态转移过程：

1. （被动打开）调用listen()进入`LISTEN（监听）`状态

2. 一旦监听到连接请求（SYN同步报文段），就将连接请求放入内核等待队列，并发送SYN|ACK同步确认报文段，此时该连接处于`SYN_RCVD（连接请求已接收）`状态

3. 如果收到客户端回应的ACK确认报文段，则该连接进入`ESTABLISHED（连接已建立）`状态

    > `ESTABLISHED`状态是指连接双方能够进行双向数据传输的状态。

4. 当客户端主动关闭连接时，会收到来自客户端的FIN结束报文段，服务器通过返回ACK确认报文段使该连接进入`CLOSE_WAIT（被动等待关闭）`状态

5. 检测到客户端关闭连接后，立即发送FIN结束报文段来关闭连接，此时该连接进入`LAST_ACK`状态

6. 收到来自客户端的ACK确认报文段，服务器端也关闭连接

客户端**典型**状态转移过程：

1. 调用connect()给服务器发送SYN同步报文段，向客户端发起连接请求，使连接进入`SYN_SENT（连接请求发送）`状态。此时有两种情况会导致connect()失败返回：

    - 如果**目标端口不存在（未被任何进程监听），或者该端口仍被处于`TIME_WAIT`状态的连接所占用**，则服务器将给客户端发送一个**复位报文段**（用于通知对方关闭连接或重新建立连接），connect()调用失败
    - 如果目标端口存在，但connect()**超时仍未收到**服务器的ACK确认报文段，connect()调用失败

2. 收到来自服务器的SYN|ACK同步确认报文段后，发送ACK确认报文段，该连接进入`ESTABLISHED（连接已建立）`状态

3. 当客户端主动关闭连接时，会向服务器发送FIN结束报文段，连接进入`FIN_WAIT_1（主动关闭连接，等待对方对FIN的确认）` 状态。接下来，

    1. 如果收到服务器 <u>专门用于确认的ACK确认报文段</u>，则连接进入`FIN_WAIT_2（被动关闭方同意关闭连接，等待被动关闭方的FIN）`状态，而此时服务器处于`CLOSE_WAIT（被动关闭方等待关闭）状态`，这一对状态是可能发生**半关闭**的状态。此时如果服务器也关闭连接（发送FIN结束报文段），客户端将给予确认并进入`TIME_WAIT（等待2MSL的时间以确保对方收到了 自己对其FIN的ACK确认）`状态。

        > 如果没收到服务器的FIN结束报文段，那么客户端将一直停留在`FIN_WAIT_2`状态。
        >
        > 这样的情况可能发生在：客户端执行半关闭后，未等服务器关闭连接（即还没收到服务器发来的FIN）就强行退出了，此时客户端连接由内核来接管，可称之为孤儿管理（类似孤儿进程）。？？？孤儿管理就是该连接没人管的意思吗？那一直停留在`FIN_WAIT_2`状态是什么意思？
        >
        > linux为了防止孤儿连接长时间存留在内核中，定义了两个内核变量：···，···

    2. 如果直接收到服务器端的 <u>携带ACK的FIN结束报文段</u>，客户端将给予确认并直接进入`TIME_WAIT`状态。

    3. 如果收到服务器端的仅用于结束的FIN结束报文段，进入`CLOSING（双方同时关闭连接）`状态，若再收到服务器发送的ACK确认报文段，客户端将直接进入`TIME_WAIT`状态

    > 为什么要引入`TIME_WAIT`状态？请看下一小节。

![image-20231212195508654](https://cdn.jsdelivr.net/gh/808bass666/picBed@main/202312121955738.png)

#### TIME_WAIT状态（2MSL等待状态）

==「主动断开连接」==的一方要经过TIME_WAIT 状态，等待`2MSL`才能完全关闭（MSL：报文最大生存时间）

》Tips：RFC 793中规定MSL为2分钟，实际应用中常用的是30秒，1分钟和2分钟等。

> **TIME_WAIT状态存在的原因有两点：**

1. 可靠地终止TCP连接
2. 保证让迟到的数据报 <u>有足够的时间被识别</u>（2MSL）并丢弃

下面解释一下这两个原因：

1. 主机A主动关闭时，如果用于  [ 确认服务器FIN ]  的最后一个ACK报文【7】丢失，那么主机B就会以为自己之前发送的FIN未能抵达主机A，于是会重发FIN。但是主机A已经是完全终止的状态，因此主机B永远无法收到主机A最后传来的ACK。因此主动关闭的一端（主机A）需要停留在某个状态以重传最后的ACK报文，这样主机B也可以正常终止。

    否则，如果直接就关闭了连接，那么之后再收到服务端的FIN结束报文段，客户端就会用RST复位报文段来回应服务器，而服务器认为这是一个错误，因为它期望的是ACK确认报文段（如报文段7）.

    **【总的就是，先发送FIN的一端 怕对方没收到最后一个ACK，需要停留在一个状态以重传ACK】**

2. 在linux上，一个TCP端口不能同时被打开多次（两次及以上）。当一个TCP连接处于`TIME_WAIT`状态时，我们将无法立即使用该连接占着的端口来建立一个新连接。

    反过来思考，如果不存在`TIME_WAIT`状态，则应用程序就能立即建立一个和刚关闭的连接相似的连接（"相似"：具有相同IP和端口号）。这个新的、和原来IP,端口相同的连接 被称为**原来的连接的化身**。

    这个化身可能接收到 属于原来的连接的、携带数据的TCP报文段（迟到的报文段），这显然是不应该发生的。

    而坚持`2MSL`能确保 两个传输方向上、尚未被接收到的、迟到的TCP报文段都已经消失（被中转路由丢弃）。因此一个连接的 新的化身 可以在`2MSL`时间后安全地建立，而绝对不会受到属于原来连接的数据。

> 如果需要避免`TIME_WAIT`状态，使得程序退出后能立即重启，那应该怎么做呢？

- 对于客户端程序，通常不用担心会因为`TIME_WAIT` 状态不能立即重启的问题。因为客户端一般使用系统**自动分配的**临时端口号来建立连接。

- 但如果是服务器**主动关闭连接**后==**异常终止**（Ctrl+C）==，由于服务器总是使用同一个服务端口号，因此连接的`TIME_WAIT`状态就导致它不能立即重启。

    ==为此，我们可以通过`socketopt()`设置`SO_REUSEADDR`选项来强制进程立即使用处于`TIME_WAIT`状态的连接占用的端口。==

> `Ctrl+C` 和 `Ctrl+Z` 的区别？

- `Ctrl+C` 

- `Ctrl+Z`

### RST复位报文段

什么情况下会发送复位报文段？

1. **访问一个不存在的端口（未被任何进程监听）时**

2. **需要 异常终止连接 时**。RST报文段是用于中断连接的强制手段，它可以立即关闭连接，而不需要等待正常的连接终止握手过程。

    给对方发送RST报文段后，对方收到这个报文段就会立即关闭连接，不再进行任何后续的数据传输或握手。

    > TCP通过发送带有RST标志的报文段来实现异常终止连接，以应对连接出现不可恢复错误或异常情况。
    >
    > 这种机制确保了在网络异常或其他问题导致连接无法正常关闭时，可以迅速中断连接，防止资源浪费和不必要的等待。

3. **处理半打开连接时**。当一端往处于半打开状态的连接写入数据时，另一端将回应一个RST报文段。（因为一端都没有该连接的任何信息了，也就没有维持连接的必要了）

    > 当一端关闭或异常终止了连接而对方没收到FIN，此时对方还维持着原来的连接，而本端已经没有该连接的任何信息了，这种状态就叫=="半打开状态"==。



### TCP交互数据流和成块数据流

> TCP报文段所携带的应用程序数据按长度分为：交互数据和成块数据。

交互数据仅包含很少的字节，使用交互数据的应用程序或协议对==实时性==要求很高，如telnet、ssh。

而成块数据的长度通常为TCP最大报文段长度（MSS: Max Segment Size），········对==传输效率==要求高，如 ftp。

### 交互数据流

> 广域网上携带交互数据的微小TCP报文段数量一般很多，可能经受很大的延迟，一个有效的解决办法是**延迟确认**和**Nagle算法**。

#### 延迟确认

服务器每次发送的ACK报文段，都包含本端需要发送的数据，这种处理方式就叫延迟确认，即**不马上确认上次收到的数据，而是延迟一段时间后将本端数据和ACK确认信息一起发出。**

> 可以联系四次挥手。
>
> 四次挥手中就没有使用延迟确认，因为服务器的ACK报文和FIN报文是分开发送的，而不是延迟后一起发送。

#### Nagle算法

该算法应用于TCP层，目的是防止因数据报过多而发生网络过载。

![image-20231221124700230](https://cdn.jsdelivr.net/gh/808bass666/picBed@main/202312211247337.png)

**要求**：

1. 一个TCP连接的通信双方在任意时刻都**最多只能发送一个未被确认的**TCP报文段。也就是"只有收到前一数据的ACK消息时，Nagle算法才发送下一数据".
2. 发送方在等待确认的同时收集本端需要发送的微小数据，并在确认到来时以一个TCP报文段将它们一并发出。

**应用场景**：

1. 传输大文件数据时，不适用Nagle算法，应通过套接字可选项`TCP_NODELAY`设为1禁用
2. 为了提高网络传输效率，未准确判断数据特性时不应该禁用Nagle算法

#### 优缺点

**优点**：

1. 极大地减少了网络上的微小TCP报文段的数量

2. 确认到达的越快，数据就发送得越快（自适应性）

    》因为上一个TCP报文段的确认还没到达 就不能发下一个

**缺点**：

1. 由于会将比较小的、连续发送的数据合并成一个较大的包，容易发生粘包现象

### 成块数据流

### 带外数据（紧急数据）

TCP接收带外数据的过程：

接收端只有在收到紧急指针标志时才检查紧急指针。

### TCP 如何实现可靠传输的？

> 校验和、连接管理、seq序列号和ack确认号、超时重传、拥塞控制、流量控制、数据拆分

1. **校验和**：验证TCP数据包在发送过程中是否有改动，一旦发现校验和有误就直接丢弃，且不发送确认消息。（发送方计算校验和，接收方进行验证）

2. **连接管理**：三次握手和四次挥手

3. **seq序列号和ack确认号：**

    - 发送方对每一次发送的消息都会编号，也就是序列号seq。
    - 接收方在每次接收到消息后回复ACK确认应答号，不仅告诉对方自己接收到了哪些数据，还告知对方下一次消息从要哪里开始发送。只要发送方没有接收到确认应答号ACK，都会重新发送数据。
    - 序列号和确认号能够保证数据包不重复、不丢弃和按序传输。

4. **超时重传**：发送方发送完数据后会启动一个重传定时器，如果超时未收到应答就将重传TCP数据报并重置计时器。每次重传超时时间都增加一倍。

    > 虽然超时会导致重传，但是重传可以发生在超时之前，即**快速重传**。

5. **快速重传**：当接收端、收到的数据包的seq序列号、比自己期望的序列号、大时，就会再次发送包含自己期望的seq序列号的ACK报文，发送方如果（在超时重传之前）收到三个相同的冗余ACK，那就知道是某段报文发生了丢包，只需重传该段报文即可，避免了超时重传。

6. **流量控制**：

    - 为什么要有流量控制？

        - 为了控制发送方的发送速率，保证接收方来得及接收。

    - 怎么进行流量控制？—— 通过【滑动窗口】

        - 发送方和接收方都维护一个滑动窗口，接收方通过TCP报文首部的窗口字段、告诉对方本端的TCP接收缓冲区还能容纳多少字节的数据。这样发送方就可以调整自己的发送速度。

            > 若缓冲区大小为0，那么发送方会停止发送数据，并且定期发送探测报文，来获取对方缓冲区大小

7. **拥塞控制**：

    - 为什么要有拥塞控制？

        - 因为如果发送数据包时网络很拥塞，那么此时发送大量数据的话，就容易丢包以及引发超时重传，导致拥塞程度更高。所以出现拥塞时，应当限制发送方的发送速率。需要【慢启动算法、拥塞避免算法、快速重传和快速恢复】来进行拥塞控制。
        - 拥塞控制和流量控制有点像但目的不同。流量控制更多的是一个端到端的控制，主要是为了控制接收方来得及接，所以要抑制发送端发送数据的速率。而拥塞控制是为了宏观上降低网络的拥塞程度。

    - 怎么进行拥塞控制？—— 通过【慢启动算法、拥塞避免算法、快速重传和快速恢复】

        - 拥塞控制就是控制的==发送窗口==，即发送端向网络一次连续写入的数据量。

             **`发送窗口 = min(接收端的接收窗口, 本端拥塞窗口)`**

8. **数据拆分**：

    - 把应用层的数据拆分成适合传输的一个个块，并且给每一个块进行编号，在接收端对数据包进行重排。

### 拥塞控制:star:

#### 概述 

> 拥塞控制有四个部分：**慢启动、拥塞避免；快速重传、快速恢复**

> 拥塞控制到底控制的是什么？

==SWND发送窗口==，即发送端向网络一次连续写入的数据量。

 `SWND发送窗口 = min(RWND接收端的接收窗口, CWND本端拥塞窗口)`

#### 慢启动和拥塞避免 

> 是 未检测到拥塞时 采用的积极避免拥塞的方法

![image-20231213184904370](https://cdn.jsdelivr.net/gh/808bass666/picBed@main/202312131849420.png)

##### 慢启动（指数增长）

慢启动也叫做**指数增长期**。

TCP连接建立后，发送端每收到一个接收端的确认，CWND就增加`CWND += min(N, SMSS)`。N为此次确认中包含的之前未被确认的字节数。？？？

> 慢启动算法的理由？

刚开始并不知道网络的实际情况，需要用一种**试探的方式**平滑的增加CWND的大小。

但是如果不是加其它手段，慢启动必然使得CWND很快膨胀（可见慢启动其实不慢）。因此还定义了一个重要的状态变量：`ssthresh 慢启动门限`。**当`cwnd<ssthresh`时，使用慢启动算法，当CWND>ssthresh时，将进入拥塞避免阶段**。


##### 拥塞避免 （线性增长）

拥塞避免算法有两种实现方式：

1. 每个RRT时间都要按`CWND += min(N, SMSS)`计算新的CWND，而不论该RRT时间内收到多少个确认

2. 每收到一个对新数据的确认报文段，就按照`CWND += SMSS*SMSS/CWND`来更新CWND

#### 快速重传和快速恢复

> 是拥塞发生时（可能发生在慢启动或拥塞避免阶段）进行拥塞控制的手段

<u>**发送端是如何判断拥塞已经发生的？对应的处理方式是？**</u>

1. **传输超时（TCP重传定时器溢出）**

    - 处理方式：
        - 启用慢启动和拥塞避免
        - 调整`ssthresh`，调整后cwnd将<SMSS<ssthresh，故而拥塞控制再次进入<u>慢启动阶段</u>。

2. **接收到重复的确认报文段**

    - 处理方式：
        - 根据是否连续收到3个重复的ACK报文段，来判断是否真的发生了拥塞，或者说TCP报文段是否确实丢失了。
        - 启用快速重传和快速恢复

    （但是如果发生在定时器溢出后，则会按传输超时的情况处理）



## 访问Internet上的web服务器

### 访问DNS服务器 

![image-20231221160531771](https://cdn.jsdelivr.net/gh/808bass666/picBed@main/202312211605852.png)

### DNS查询的两种方式

#### 递归查询 迭代查询

==递归查询 根好累==

**递归的意思是：只需要发出一次请求，就能得到相应的结果。**比如主机-本地DNS、本地DNS-转发器之间都是递归查询。

**迭代的意思是：需要经过多次挨个查询，才能得到相应的结果**。比如转发器-根服务器、转发器-.com域服务器等，都是迭代查询。

递归查询就要一直递归到底（结果）； 迭代查询每次找不到就只用完成一件事，那就是告诉请求者应该去找谁，其它的就不管了

![image-20231221172603404](https://cdn.jsdelivr.net/gh/808bass666/picBed@main/202312211726485.png)

> 例：在浏览器地址栏输入www.orentec.co.kr，并整理出网页显示过程。假设浏览器访问的默认DNS服务器中并没有关于www.orentec.co.kr的IP地址信息。

1. **浏览器缓存检查：** 浏览器首先会检查本地缓存，看是否已经有关于 "[www.orentec.co.kr](http://www.orentec.co.kr/)" 的IP地址信息。如果浏览器之前已经访问过这个网站，可能会缓存有关域名的一些信息，包括IP地址。
2. **系统缓存和 hosts 文件检查：** 如果浏览器本地缓存中没有找到相关信息，系统会检查系统缓存和 hosts 文件。
3. **DNS解析：** 如果以上步骤都没有找到相关信息，浏览器将发起DNS请求，向默认的DNS服务器(本地DNS服务器)查询 "[www.orentec.co.kr](http://www.orentec.co.kr/)" 的IP地址。若默认的DNS服务器中DNS缓存没有命中，则默认DNS服务器将根据域名系统的层级结构进行迭代查询:
    1. 它首先**向根**DNS服务器**请求顶级域**（.kr）的DNS服务器的IP地址，
    2. 然后它向**顶级域**（.kr）的DNS服务器发起请求，若没有
    3. 就接着向**二级域**（[co.kr](http://co.kr/)）的DNS服务器发起请求，若还没有
    4. 最后它向**三级域**（[orentec.co.kr](http://orentec.co.kr/)）的DNS服务器发起请求，最终得到所查域名对应的IP地址。
4. **DNS响应：** 
    - 默认DNS服务器（本地DNS服务器）将得到的 IP 地址返回给操作系统，同时自己将 IP 地址缓存起来
    - 操作系统将 IP 地址返回给浏览器，同时自己也将 IP 地址缓存起
    - 至此，浏览器就得到了域名对应的 IP 地址，并将 IP 地址缓存起
5. **建立连接：** 浏览器获得了 "[www.orentec.co.kr](http://www.orentec.co.kr/)" 的IP地址后，它将通过使用 HTTP 或 HTTPS 协议与该IP地址建立连接。
6. **网页加载：** 一旦连接建立，浏览器就会向服务器发送请求，并开始下载网页的 HTML、CSS、JavaScript 等资源。浏览器解析这些资源并渲染页面，最终显示 "[www.orentec.co.kr](http://www.orentec.co.kr/)" 的内容。

这个过程中涉及到多个系统组件，包括**浏览器、本地缓存、系统缓存、hosts 文件、DNS 服务器**等。如果默认的DNS服务器中没有 "[www.orentec.co.kr](http://www.orentec.co.kr/)" 的IP地址信息，整个过程中会涉及到DNS解析过程。

#### **DNS解析过程：**

![image-20231221174758648](https://cdn.jsdelivr.net/gh/808bass666/picBed@main/202312211749332.png)



# 深入解析高性能服务器编程

## linux网络编程基础

### 套接字概念 

1. 套接字是什么？

​	Socket本身有“插座”的意思，在Linux环境下，用于表示进程间网络通信的特殊文件类型。本质为**内核**借助**缓冲区**形成的伪文件。

2. 管道和套接字的区别？

​	管道主要应用于**本地进程间**通信，而套接字多应用于**网络进程间**数据的传递。

3. 在网络通信中，套接字一定是**成对出现**的.

​	欲建立连接的两个进程各自有一个socket来标识，这两个socket组成的socket pair唯一标识一个连接。因此可以用Socket来描述网络连接的一对一关系。

4. ==**一个套接字有两个缓冲区（发送缓冲区和接收缓冲区）**==

    一端的发送缓冲区对应对端的接收缓冲区。

### linux文件操作

对linux而言，文件操作与socket操作并无区别。

#### 文件描述符

是系统为了方便称呼，分配给文件或套接字的**整数**。

文件描述符（主要是linux中的术语）也称**文件句柄（主要是windows中的术语）**。

![image-20231204184232251](https://cdn.jsdelivr.net/gh/808bass666/picBed@main/202312132031070.png)

#### 文件操作

##### 打开 open

```c++
int open(const char* path, int flag);
p1: 文件路径（字符串地址形式）
p2: 文件打开模式（如有多个使用 " | "进行组合）
返回值：成功返回文件描述符，失败返回-1
```

![image-20231204184639363](https://cdn.jsdelivr.net/gh/808bass666/picBed@main/202312132031071.png)

##### 关闭 close

```c++
int close(int fd);
p1: 需要关闭的文件/套接字描述符
返回值：成功返回0，失败返回-1
```

##### 写入 write（传输）

```c++
ssize_t write(int fd, const void* buf, size_t nbytes);
p1: 要写入的文件描述符
p2: 要写入的数据的缓冲地址（向fd对应的文件中写入buf中保存的数据）
p3: 发送的数据的大小（size_t是通过typedef声明的unsigned int型）
返回值：成功返回写入的字节数，失败返回-1
```

补充：

1. ssize_t前多加的s表示signed，即带符号的意思，是signed int类型
2. 

> **以t为后缀的数据类型**在`sys/types.h`头文件中由typedef声明定义，算是给基本数据类型起的**别名**。
>
> 为什么要起别名呢？：为了方便以后的扩展，减少代码的变动。
>
> ![image-20231204190809959](https://cdn.jsdelivr.net/gh/808bass666/picBed@main/202312132031072.png)

##### 读取 read（接收）

```c++
ssize_t read(int fd, void* buf, size_t nbytes);
p1: 要接收数据的socket
p2: 保存接收数据的缓冲地址（即要把数据读到哪里）
p3: 指定最大接收的字节数（size_t是通过typedef声明的unsigned int型）
返回值：成功返回读取/接收的字节数（遇到文件结尾返回0），失败返回-1
```

### 数据读写 

> send,recv等读写接口都只作用于用户态，send的作用仅仅是**将要发送的数据拷贝到内核态的发送缓冲区**，而内核具体什么时候发送缓冲区内的数据是不管的，recv的作用也仅仅是**从内核的接收缓冲区中拷贝数据到用户态**。

#### TCP数据读写

对文件的读写操作read和 write同样适用于socket。但是socket编程接口提供了几个专门用于socket数据读写的系统调用，它们**增加了对数据读写的控制**。其中用于TCP流数据读写的系统调用是:

##### `recv()`

```cpp
ssize_t recv(int sockfd,void *buf,size_t len,int flags);
//recv读取sockfd 上的数据，buf和len参数分别指定读缓冲区的位置和大小，flags参数的含义见后文，通常设置为0。
//成功时返回实际读取到的数据的长度，它可能小于我们期望的长度len。因此我们可能要多次调用recv，才能读取到完整的数据。recv返回0表示对方已经关闭连接了。recv出错时返回-1并设置errno。
```

##### `send()`

```c++
ssize_t send(int sockfd,const void *buf,size_t len,int flags);
//send往sockfd上写入数据，buf和len参数分别指定写缓冲区的位置和大小。send成功时返回实际写入的数据的长度，失败则返回-1并设置errno。
```

##### recv和send返回值含义

> 这部分参考：[cppguide](https://cppguide.cn/pages/ee17fb/)

|   返回值 n    |                          返回值含义                          |
| :-----------: | :----------------------------------------------------------: |
|       0       |                         对端关闭连接                         |
|    大于 0     |           成功发送（send）或收取（recv） n 个字节            |
| 小于 0（ -1） | 出错或者被信号中断或者对端 TCP 窗口太小数据发不出去（send）或者当前网卡缓冲区已无数据可收（recv） |

我们来逐一介绍下这三种情况：

- **返回值等于 0**

    通常情况下，如果 **send** 或者 **recv** 函数返回 **0**，我们就认为对端关闭了连接，我们这端也关闭连接即可，这是实际开发时最常见的处理逻辑。~~send 函数主动发送 0 字节时也会返回 0，这是一种特例，我们下一节会介绍。~~

- **返回值小于 0**

    对于 **send** 或者 **recv** 函数返回值小于 **0** 的情况（即返回 **-1**），根据前文对阻塞与非阻塞的讨论，此时并不表示 **send** 或者 **recv** 函数一定调用出错。这里列一个表格说明：

    |      |            返回值和错误码            |           send 函数            |         recv 函数          | 操作系统说明 |
    | ---- | :----------------------------------: | :----------------------------: | :------------------------: | :----------: |
    | 1    | 返回 -1，错误码 EWOUDBLOCK 或 EAGAIN | TCP 窗口太小，数据暂时发不出去 | 当前内核缓冲区中无可读数据 |    Linux     |
    | 2    |        返回 -1，错误码 EINTR         |      被信号中断，需要重试      |    被信号中断，需要重试    |    Linux     |
    | 3    |    返回 -1，错误码不是上述 1 和 2    |              出错              |            出错            |    Linux     |
    | 4    |    返回 -1，错误码 WSAEWOUDBLOCK     | TCP 窗口太小，数据暂时发不出去 | 当前内核缓冲区中无可读数据 |   Windows    |
    | 5    |      返回 -1，错误码不是上述 4       |              出错              |            出错            |   Windows    |

    > 注意：这里是针对非阻塞模式下 socket 的 send 和 recv 返回值，如果是阻塞模式下 socket，如果返回值是 -1（Windows 上即 SOCKET_ERROR），则一定表示出错。

- **返回值大于 0**

    对于 **send** 和 **recv** 函数返回值大于 **0**，表示发送或接收多少字节，==需要注意的是，在这种情形下，我们一定要判断下 send 函数的返回值是不是我们期望发送的字节数长度，而不是简单判断其返回值大于 0。==

    所以最好采取的方式是：在一个循环中调用 send 函数，如果数据一次性发不完，记录偏移量，下一次从偏移量处接着发，直到全部发送完为止。

    ```c++
    //推荐的方式：在一个循环里面根据偏移量发送数据
    bool SendData(const char* buf, int buf_length) {
        //已发送的字节数目
        int sent_bytes = 0;
        int ret = 0;
        while (true) {
            ret = send(m_hSocket, buf + sent_bytes, buf_length - sent_bytes, 0);
            if (ret == -1) {
                if (errno == EWOULDBLOCK) {
                    //严谨的做法，这里如果发不出去，应该缓存尚未发出去的数据，后面介绍
                    break;
                } else if (errno == EINTR) {
                    continue;
                } else {
                    return false;
                }
            } else if (ret == 0) {
                return false;
            }
    
            sent_bytes += ret;
            if (sent_bytes == buf_length)
                break;
    
            //稍稍降低 CPU 的使用率
            usleep(1);
        } 
        return true;
    }
    ```

    

##### flag参数

为数据收发提供了额外控制，可取下列选项中的一个或几个的逻辑或。

![image-20231214192353746](https://cdn.jsdelivr.net/gh/808bass666/picBed@main/202312141923841.png)

#### UDP数据读写

另外，这两个函数也可以用于面向连接（STREAM）的socket数据读写，只需要把最后两个参数设置为NULL。

> 因为**UDP通信没有连接的概念**，所以我们**每次读/写数据都需要 获取发送端(读)/指定接收端(写) 的socket地址和长度。**
>
> 除此之外，返回值与其它参数的含义均与`recv()`和`send()`相同。

##### `recvfrom()` 

```c++
ssize_t recvfrom(int sockfd,void *buf,size_t len,int flags,struct sockaddr* src_addr,socklen_t* addrlen);
//recvfrom调用会获取数据传输端的地址，并填充到clnt_adr（因此serv端知道是从哪个客户端接收的数据），正是利用该地址将接收的数据逆向传输 
//读取sockfd上的数据，buf和len参数分别指定读缓冲区的位置和大小。因为UDP通信没有连接的概念，所以我们每次读取数据都需要获取发送端的socket地址，即参数src_addr所指的内容，addrlen则指定该地址的长度
```

##### `sendto()`

```c++
ssize_t sendto(int sockfd, const void *buf, size_t len, int flags, const struct sockaddr* dest_addr, socklen_t addrlen);
//往sockfd上写入数据，buf和len参数分别指定写缓冲区的位置和大小。dest_addr指定接收端的socket地址，addrlen指定该地址的长度
```

#### 通用数据读写

socket编程提供了一对通用的数据读写系统调用，不仅能用于TCP，还能用于UDP。

##### `recvmsg()` 

```c++
ssize_t recvmsg(int sockfd,struct msghdr* msg,int flag);
```

##### `sendmsg()`

```c++
ssize_t sendmsg(int sockfd,struct msghdr* msg,int flag);
```

##### msghdr结构体

```c++
struct msghdr {
    void* msg_name; /*指向一个socket地址结构变量, 用于指定通信对方的socket地址。对于面向连接的TCP协议，该成员没有意义，必须被设置为NULL */
    socklen_t msg_namelen; /*指定了msg_name所指socket地址的长度*/
    struct iovec* msg_iov; /*分散的内存块*/
    int msg_iovlen;  /*分散内存块的数量*/
    void* msg_control; /*指向辅助数据的起始位置*/
    socklen_t msg_controllen; /*辅助数据的大小*/
    int msg_flags; /*复制函数中的fags参数，并在调用过程中将更新后的标志设置道msg_flags中*/
};
struct iovec {
    void *iov_base; /*内存起始地址*/
    size_t iov_len; /*这块内存的长度*/
};
```

对于`recvmsg()`而言，数据将被读取并存放在msg_iovlen块分散的内存中，这些内存的位置和长度则由msg_iov指向的数组指定，这称为==分散读==；

对于`sendmsg()`而言，msg_iovlen块分散内存中的数据将被一并发送，这成为==集中写==；

这对通用数据读写函数的返回值与其它参数的含义均与`recv()`和`send()`相同。

### socket地址api



### socket基础api

#### 创建socket`socket()`

```c++
int socket(int domain, int type, int protocol);
p1: 套接字使用的协议族(PF: Protocol family)
p2: 服务类型，即套接字的数据传输方式（SOCK_STREAM流服务、SOCK_UGRAM数据报服务..）
p3: 通信时使用的协议（通常传0，除非同一协议族中存在多个数据传输方式相同的协议）
返回值：成功返回文件描述符，失败返回-1并设置errno
```

##### p1.协议族 domain

![image-20231204194334533](https://cdn.jsdelivr.net/gh/808bass666/picBed@main/202312132031073.png)

##### p2.服务类型 type

> 对于TCP/IP协议族而言，其值取`SOCK_STREAM`表示传输层使用TCP协议，取`SOCK_UGRAM`表示传输层使用UDP协议

![image-20231213211306286](https://cdn.jsdelivr.net/gh/808bass666/picBed@main/202312132113977.png)

###### 1.`SOCK_STREAM`流服务

特点：

- 传输过程中数据不会丢失、按序传输（类比传送带传送糖果）
- 传输的数据不存在数据边界（30个糖果是分批传送的，但是接收者凑齐30个后才装袋）
- 只能与另一个同样类型（流服务）的套接字进行连接

> 第二个特点也适用于write和read函数.
>
> 套接字内部有缓冲区，因此受到数据并不意味着要马上调用read，只要不超过buf容量，则可能在数据填充满缓冲区后调用一次read读取全部，也可能分成多次read调用读取。
>
> 也就是说，在面向连接的套接字中，read和write的调用次数并无太大意义。

###### 2.`SOCK_UGRAM`数据报服务

特点：

- 传输过程中数据可能丢失，强调快速传输而非传输顺序
- 传输的数据存在数据边界（发送write的次数要和接收read的次数一致）
- 每次传输的数据大小有限制

##### p3.协议protocol

##### ❗❗Attention❗❗

> socket创建套接字，**但此时套接字并不马上分为服务器端和客户端。**
>
> 如果紧接着调用bind,listen函数，将成为服务器端套接字；
>
> 如果紧接着调用connect函数，将成为客户端套接字；

#### 命名socket`bind() `

命名socket即给socket绑定地址。

在服务器程序中，只有命名socket后客户端才知道如何连接它；而客户端通常不需要命名socket，而是采用匿名方式。

```c++
int bind(int sockfd, struct sockaddr *myaddr, socklen_t addrlen);
：将myaddr所指的socket地址分配给sockfd，addrlen指出该socket地址的长度
返回值：成功返回0，失败返回-1并设置errno
```

#### 监听socket`listen()`

只有服务器端==调用listen函数，创建一个监听队列来存放待处理的客户端连接==，客户端才能进入可向服务器发出连接请求的状态，也就是说，这时客户端才能调用connect()。

```c++
int listen(int sock, int backlog);
p1: 希望进入监听状态的socket
p2: 内核监听队列的上限数，即最多让backlog个连接请求进入队列
//在内核版本2.2之前的Linux中，backlog指【所有处于半连接状态(SYN_RCVD)和完全连接状态(ESTABLISHED)】的socket 的上限。这就可以对DDOS攻击起到限制作用。
//但自内核版本2.2之后，它只表示处于【完全连接状态】的socket的上限（这对于DDOS攻击无能为力，此时可以通过设置【反向代理】的方式来阻止DDOS攻击），处于半连接状态的socket的上限则由/proc/sys/net/ipv4/tcp_max_syn_backlog 内核参数定义。backlog参数的典型值是5。
返回值：成功返回0，失败-1
```

#### 接受连接`accept()`

想接收数据，必然就需要套接字。但是仍采用前面listen()生成的门卫套接字（监听socket）吗？当然不行。因为门卫不止要负责接收一个连接请求，它是专门用来接收客户端连接请求的，因此需要一个单独的套接字专门用来进行数据交换。

而accept()就可以**自动创建**这样的套接字，并自动连接到 取出的连接 对应的 发起请求的客户端。

> 我们把 执行过listen调用、处于LISTEN状态 的socket称为`监听socket`，而所有处于`ESTABLISHED`状态的socket则称为`连接socket`.

accept函数用于从Accepte队列中**pop出一个已完成的连接**，若Accept队列为空，则accept函数所在的进程阻塞。

需要注意的是，accept函数从listen监听队列中（取出）接收一个连接，**而不论该连接处于何种状态（即使客户端网络异常掉线），更不关心网络状况的变化**。

```c++
int accept(int sock, struct sockaddr *addr, socklen_t *addrlen);
p1: 监听socket（门卫）
p2: `addr` is a pointer to a sockaddr structure.  This  structure  is  filled  in  with  the address of the peer socket （accept调用时会将其填充为客户端socket地址，大小由参数addrlen指出//传出参数
p3: `addrlen` is a value-result argument(传入传出参数): the caller must initialize it to contain the size of the structure pointed to  by  addr;  on  return it will contain the actual size of the peer address（传入的是sockaddr结构体的大小，传出的是客户端socket地址的实际大小。accept调用后就被填入客户端socket地址结构体中）
返回值：成功返回创建的新的连接socket【或者说返回该连接对应的客户端socket】，失败返回-1并设置errno
```

#### 发起连接`connect()`

客户端需要通过connect()来主动与服务器建立连接。建立连接是通过三次握手，而这个连接的过程是由内核完成，不是这个函数完成的，这个函数的作用仅仅是通知 Linux 内核，让 Linux 内核自动完成 TCP 三次握手连接。

客户端套接字的ip和端口会在调用connect()时自动分配（隐式绑定），而无需调用标记的bind()进行分配

```c++
int connect(int sock, struct sockaddr *serv_addr, socklen_t addrlen);
p1: 客户端套接字
p2: 服务器的监听socket地址(门卫)
p3: 指定serv_addr地址长度
返回值：成功返回0，失败返回-1
```

**客户端调用connect函数后默认会一直阻塞，直到三次握手成功或超时失败才返回。**

- 服务器端接收连接请求（即服务器端把连接请求信息记录到等待队列，而并不意味着调用accept函数，因此连接函数返回后并不立即进行数据交换）
- 发生断网等异常情况而中断连接请求

#### 关闭连接`close()`/`shutdown()`

##### `close()`

关闭连接实际上就是关闭该连接对应的socket，可以通过关闭普通文件描述符的close来完成。

调用close()将向对方传递`EOF`.

不过，close系统调用并非总是立即关闭一个连接，而是将fd的引用计数减1。只有当fd的引用计数为0时，才真正关闭连接。

多进程程序中，一次 fork调用默认将使父进程中打开的socket的引用计数加1，因此我们必须在父进程和子进程中都对该socket执行close调用才能将连接关闭。

默认情况下，当我们使用close系统调用来关闭一个socket时，close将立即返回，TCP模块负责把该socket对应的TCP发送缓冲区中残留的数据发送给对方。我们可以通过`SO_LINGER`选项来控制close系统调用在关闭TCP连接时的行为。

##### `shutdown()`

相对于close来说，shutdown是专门为网络编程设计的。

**shutdown能够分别关闭socket上的读或写，或者都关闭。而close在关闭连接时只能将socket 上的读和写同时关闭。**

如果无论如何都要立即终止连接，可以使用shutdown

#### 断开连接的方式

![image-20231213215958061](https://cdn.jsdelivr.net/gh/808bass666/picBed@main/202312132201334.png)

##### `SHUT_RD`

会断开输入流，无法调用输入相关函数，而且使套接字无法接收数据，**即使输入缓冲收到数据也会抹去。**

#####  `SHUT_WR` 

会断开输出流，无法调用输出相关函数，而且使套接字无法写入数据，**但如果输出缓冲还有未传输的数据，则将传递至目标主机。**

##### `SHUT_RDWR`

### 网络信息api

#### `gethostbyname()`,`gethostbyaddr()`

`gethostbyname()`根据 主机名 获取主机的完整信息；

`gethostbyaddr()`根据 IP地址 获取主机的完整信息。

#### `getservbyname()`,`getservbyaddr()`

`getservbyname()`根据 名称 获取某个服务的完整信息；

`getservbyport()`根据 端口号 获取某个服务的完整信息。

#### `getaddrinfo()`

#### `getnameinfo()`

### 带外标记

![image-20231214195216935](https://cdn.jsdelivr.net/gh/808bass666/picBed@main/202312141952020.png)

### 地址信息函数

`getsockname()`

```c++
int getsockname(int sockfd,struct sockaddr* address,socklen_t* address_len);
//获取sockfd对应的本端socket地址，并将其存储与address参数指定的内存中，该socket地址的长度则存储在address_len参数指向的变量中。
//如果实际socket地址的长度大于address所指内存区的大小，那么该socket地址将被截断。
//成功返回0，失败返回-1并设置errno。
```

`getpeername()`

```cpp
int getsockname(int sockfd,struct sockaddr* address,socklen_t* address_len);
//获取sockfd对应的远端socket地址
//其参数及返回值的含义与getsockname()相同
```

### socket选项

如果说fcntl系统调用是控制文件描述符属性的通用POSIX方法，那么下面两个系统调用则是专门用来读取和设置socket文件描述符属性的方法:

#### `getsockopt()`读取选项

```c++
int getsockopt(int sockfd, int level, int option_name, void* option_value,socklen_t* restrict option_len);
```

####  `setsockopt()`设置选项

用于任意类型、任意状态套接口的设置选项值。

```cpp
int setsockopt(int sockfd，int level, int option_name，const void* option_value, socklen_t option_len) ;
```

#### 参数说明

p1: `sockfd`参数指定被操作的目标socket；

p2: 套接字选项是分层的，`level`指定要操作哪个协议层的选项。一般设成SOL_SOCKET以存取socket层。

- SOL_SOCKET: 通用套接字选项.
- IPPROTO_IP：IPv4选项，IPPROTO_IPV6：IPv6选项
- IPPROTO_TCP：TCP选项

p3: `option_name` 选项名；

p4: `option_value` 存放选项的缓冲区地址

p5: `option_len` ：sizeof(option_value)。（比如option_name是int类型，option_value就是int型地址，option_len就是sizeof(int)）

**返回值：成功返回0，失败返回-1并设置errno**

#### socket选项

有的选项是只读选项。

注意！**对服务器而言，有些socket选项只能在调用listen前**，针对将执行listen调用的socket**设置才有效**。这是因为·······

**而对客户端而言，有些socket选项只能在connect()调用前进行设置**。因为connect()调用成功返回后，TCP三次握手已完成。

![image-20231214203305236](https://cdn.jsdelivr.net/gh/808bass666/picBed@main/202312142033320.png)

##### `SOREUSEADDR`

由于==「主动断开连接」==的一方要经过`TIME_WAIT` 状态，

- 对于客户端程序，通常不用担心会因为`TIME_WAIT` 状态不能立即重启的问题。因为客户端一般使用系统**自动分配的**临时端口号来建立连接。

- 但如果是服务器**主动关闭连接**后**异常终止**（Ctrl+C），由于服务器总是使用同一个服务端口号，因此`TIME_WAIT`状态就导致它不能立即重启。


**为此，我们可以设置`SO_REUSEADDR`选项为==1==来强制进程立即使用处于`TIME_WAIT`状态的连接占用的端口。(**将`TIME_WAIT`状态下的端口号重新分配给新的套接字)

这个套接字选项通知内核，如果端口忙，但TCP状态位于 `TIME_WAIT` ，可以重用端口。如果端口忙，而TCP状态位于其他状态，重用端口时依旧得到一个错误信息，指明"地址已经使用中"。

> 通常情况下，`SO_REUSEADDR`选项是在==调用`bind`之前==设置的。

除此之外，也可以通过修改内核参数`/proc/sys/net/ipv4/tcp_tw_recycle`来快速回收被关闭的socket，使得TCP连接根本就不进入`TIME_WAIT`状态，进而允许应用程序立即重用本地的socket地址。

```c++
int on = 1;
if(setsockopt(serv_sock, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(int)) < 0) error_handling("setsockopt() error");
//或写成
int optlen = sizeof(on);
if(setsockopt(serv_sock, SOL_SOCKET, SO_REUSEADDR, (void*) &on, optlen) < 0) error_handling("setsockopt() error");
```

###### 端口复用

1. 端口复用是什么意思？：允许一个应用程序把 n 个套接字绑在一个端口上。

2. 怎么实现端口复用？：操作系统内核支持通过配置socket参数的方式来实现多个进程绑定同一个端口。即用setsockopt()设置中间程序的监听SOCKET.

3. 端口复用的原理是？：在服务器安装一个中间程序,在客户端发送数据给端口前劫获这个数据,判断这个是不是HACKER发来的数据,如果是把它发给后门程序,如果不是则转发给端口程序,返回信息再发给客户端.

4. 端口复用的用途是？：端口复用最常用的用途应该是 防止服务器重启时 之前绑定的端口还未释放或者程序突然退出而系统没有释放端口。这种情况下如果设定了端口复用，则新启动的服务器进程可以直接绑定端口。如果没有设定端口复用，绑定会失败。

5. 实例：`setsockopt(serv_sock, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(int));`

##### `SO_RCVBUF`,`SO_SNDBUF` 

分别表示TCP接收(输入)缓冲区和发送(输出)缓冲区的大小。
不过，当我们用setsockopt来设置TCP的接收缓冲区和发送缓冲区的大小时。系统都会将其值加倍，并且不得小于某个最小值。

我们可以直接修改内核参数`/proclsys/net/ipv4/tcp_rmem`和 `/proc/sys/net/ipv4/tcp_wmem`来强制TCP接收缓冲区和发送缓冲区的大小没有最小值限制。

##### `SO_RCVLOWAT`,`SO_SNDLOWAT`

分别表示TCP接收缓冲区和发送缓冲区的低水位标记。

它们一般被I/O复用系统调用（见第9章）用来判断socket是否可读或可写。当TCP接收缓冲区中可读数据的总数大于其低水位标记时，I/O复用系统调用将通知应用程序可以从对应的socket 上读取数据；当TCP发送缓冲区中的空闲空间（可以写入数据的空间)大于其低水位标记时，I/O复用系统调用将通知应用程序可以往对应的socke上写入数据。
默认情况下，TCP接收缓冲区的低水位标记和TCP发送缓冲区的低水位标记均为1字节。

##### `TCP_NODELAY`

前面的 TCP协议详解 中讲 TCP交互数据流中时，有详细讲解 延迟确认和Nagle算法。我们能知道，在传输 大文件数据 时，应禁用Nagle算法，通过设置套接字可选项`TCP_NODELAY`为`1`就能禁用。

##### `SO_LINGER`

用于控制close系统调用在关闭TCP连接时的行为。

默认情况下，当我们使用close系统调用来关闭一个socket时，close将立即返回，TCP模块负责把该socket对应的TCP发送缓冲区中残留的数据发送给对方。

··········

#  TCPIP网络编程

## 地址族与数据序列

### 地址信息的表示 

#### 专用socket地址

##### `sockaddr_in`（IPv4）

> 所有 专用socket地址 类型的变量在实际使用时都需要强转为 通用socket地址类型sockaddr，因为所有socket编程接口使用的地址参数的类型都是sockaddr。

```c++
struct sockaddr_in { //用于表示 ipv4 地址信息
    sa_family_t      sin_family; //地址族: AP_INET（AF: Address Family）
    struct in_addr   sin_addr; //ipv4地址结构体（要用htonl()转换为网络字节序）
    uint16_t         sin_port; //端口号（要用htons()转换为网络字节序）
    char             sin_zero[8]; //不使用，填充为0
}
//为什么sockaddr_in是表示ipv4的结构体，但还需要指定地址族信息呢？：只是为了和sockaddr保持一致
struct in_addr {
    In_addr_t        s_addr;//32位ipv4地址（要用网络字节序表示）
}
```

1. **成员sin_family**：地址族。每种**协议族(PF)**使用的**地址族(AF)**均不同。

![image-20231204204622547](https://cdn.jsdelivr.net/gh/808bass666/picBed@main/202312132031074.png)

> 宏`PF_*`和`AF_*`都定义在`<bits/socket.h>头文件`中，且二者有完全相同的值，所以二者通常混用。

2. **成员sin_port** ：以**网络字节序**保存16位端口号

3. **成员sin_addr** ：以**网络字节序**保存32位ip地址

4. **成员sin_zero** ：无特殊含义，只是`sockaddr_in`为了和`sockaddr`结构体保持一致而插入的成员，必须填充为0！

#### 通用socket地址

##### `sockaddr`（旧）

```c++
struct sockaddr { 
    sa_family_t      sin_family; //地址族（AF: Address Family）
    char             sa_data[14]; //地址信息
}
```

因为`sockaddr`结构体其并非只为IPv4设计，要兼容，所以它将IP地址和端口号合并到一起用一个成员`sa_data`表示。这就导致设置与获取ip和端口需要执行繁琐的操作。因此才有了新的结构体`sockaddr_in`。

不同的协议族的地址值具有不同含义和长度，如图：

![image-20231204220059185](https://cdn.jsdelivr.net/gh/808bass666/picBed@main/202312132031075.png)

#### 》bind()参数传递

bind()函数原型：`int bind(int sockfd, struct sockaddr *myaddr, socklen_t addrlen);`
bind()函数调用：

```cpp
struct sockaddr_in serv_addr;
....
if(bind(serv_sock, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) == -1)    ....
```

由前面对`sockaddr`和`sockaddr_in`的讲解，我们就能明白为什么bind原型中第二个参数是`sockaddr`结构体变量, 但调用时传入的却是`sockaddr_in`结构体变量了。

因为各种socket地址结构体的开头都是相同的，因此只要取得某种sockaddr结构体的首地址，不需要知道具体是哪种类型的sockaddr结构体，就可以根据**地址类型(AF)**字段确定结构体中的内容。

因此，socket API可以接受**各种类型的**sockaddr结构体指针做参数，例如bind、accept、connect等函数，这些函数的参数应该设计成void *类型以便接受各种类型的指针，

但是sock API的实现早于ANSI C标准化，那时还没有`void*`类型，因此这些函数原型中的参数都是用的`struct sockaddr *`表示，在传递参数之前要强制类型转换一下。

##### `sockaddr_storage`（新）

这个结构体不仅提供了足够大的空间存放地址值，而且时内存对齐的。

```c++
struct sockaddr { 
    sa_family_t      sin_family; //地址族
    unsign long int __ss_align;
    char __ss_padding[128-sizeof(__ss_align)];
}
```



### 字节序与转换函数

> 注意，即使是同一台及其上的两个进程通信，也要考虑字节序的问题

- **大端序**：高位存低地址 （**网络字节序**：网络传输数据的标准）
- **小端序**：高位存高地址（大部分CPU都是，因此也叫**主机序**）

> 除了向sockaddr_in中填充数据外，其它情况无需考虑字节型问题（自动转换）。

#### 字节序转换函数

h表示host，n表示network，l表示32位长整数（ip），s表示16位短整数（port）。

```c++
//从主机字节序转换为网络字节序
uint32_t htonl(uint32_t hostlong);//ip转换
uint16_t htons(uint16_t hostshort);//port转换
//从网络字节序转换为主机字节序
uint32_t ntohl(uint32_t netlong);//ip转换
uint16_t ntohs(uint16_t netshort);//port转换
```

### 地址的初始化与分配

#### IP地址转换函数

##### 早期（IPv4专用）

> 只能处理IPv4的ip地址（参数是32位的`struct in_addr`）

i表示ip，a表示address，n表示network

```c++
1、将点分十进制字符串表示的ipv4地址转换为32位【网络字节序】整数
//第一种、使用频率不高
in_addr_t inet_addr(const char* s);//成功返回32位大端序整数，失败返回INADDR_NONE
//第二种、会自动把结果存到addr指向的地址结构体
int inet_aton(const char* s, struct in_addr *addr);//成功返回1，失败返回0

2、将网络字节序整数表示的ipv4地址转换为点分十进制字符串表示的ipv4地址
char* inet_ntoa(struct in_addr adr);//成功返回转换的字符串的地址，失败返回-1
//Attention！！！调用完该函数后要立即将字符串复制到其它内存空间！因为该函数内部用一个静态变量存储了转换结果，函数返回值指向该静态内存，因此该函数是不可重入的。若再次调用该函数，则有可能覆盖之前保存的字符串！
```

##### 现在（通用）

> 支持IPv4和IPv6. 其中inet_pton和inet_ntop不仅可以转换IPv4的in_addr，还可以转换IPv6的in6_addr. 因此函数接口是void *addrptr

```c++
//将字符串表示的ip地址src转换成用网络字节序整数表示的ip地址，并把转换结果存储于dst指向的内存中。
int inet_pton(int af, const char *src, void *dst);//成功返回1，否则0
//af参数指定地址族（AF_INET或AF_INET6）
//inet_ntop函数进行相反的转换，最后一个参数size指定目标存储单元的大小
const char *inet_ntop(int af, const void *src, char *dst, socklen_t size);//成功返回目标存储单元的地址，否则返回NULL并设置errno
```

**端口号转换函数**

```c++
//字符串转换成int整型数,但不会自动转换为网络字节序，因此搭配hton()使用
int atoi(const char* s);
```

#### 地址信息初始化

服务器端的地址信息初始化工作通过bind()完成，而客户端通过connect()完成。

服务端将sockaddr_in初始化为服务端套接字的ip和端口号，然后调用bind()；

客户端将sockaddr_in初始化为要连接的服务端套接字的ip和端口号，然后调用connect()；

#### **服务器端套接字初始化**

##### INADDR_ANY

利用`INADDR_ANY`自动获取服务器ip，不必手动输入。（服务器端优先考虑这种方式）

```c++
int serv_sock;//文件(套接字)描述符
struct sockaddr_in serv_addr;//保存地址信息的结构体
char* serv_port = "9190";
//创建服务器端套接字（监听套接字）
serv_sock = socket(PF_INET, SOCK_STREAM, 0);
//地址信息初始化
memset(&serv_addr, 0, sizeof(serv_addr));//给ser_addr的所有字节均初始化为0
serv_addr.sin_family = AF_INET;//地址族
serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);//ip
serv_addr.sin_port = htons(atoi(serv_port));//port
//分配地址信息
bind(serv_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
```

## 基于TCP的服务器端/客户端

### 函数调用顺序

#### TCP服务器端的默认函数调用顺序

> socket() -> bind() -> listen() -> accept() -> write()/read() ->close()

#### TCP客户端的默认函数调用顺序

> socket() -> connect() -> write()/read() ->close()

![image-20231215191913788](https://cdn.jsdelivr.net/gh/808bass666/picBed@main/202312151919860.png)

> 图片来源：xiaolincoding

### 实现TCP迭代回声服务器端/客户端

#### 迭代服务器端`echo_serv.c`

不止为一个客户端提供服务，而是向所有客户端提供服务。

那么就要循环调用accept()，如下图所示

```c++
#include <stdio.h>      // Standard input/output functions
#include <stdlib.h>     // Standard library functions
#include <string.h>     // String handling functions
#include <unistd.h>     // Unix standard functions
#include <arpa/inet.h>  // IP address conversion functions
#include <sys/socket.h> // Socket functions
#define BUF_SIZE 1024   // Define a constant for the buffer size
void error_handling(char* message); // Function prototype for error handling

int main(int argc, char* argv[]) {
    //argv[0] is the program name,argv[1] is port number
    int  serv_sock, clnt_sock;
    //serv_sock: server socket(gatekeeper)，
    //clnt_sock: connection socket to communicate with the client
    char message[BUF_SIZE];
    struct sockaddr_in serv_adr, clnt_adr; 
    socklen_t clnt_adr_sz; //initialize clnt_adr_sz
    
    if(argc != 2) { //Two parameters are required, one is the program name, and the other one is port number
        printf("Usage : %s <port>\n", argv[0]);
        exit(1);
    }   
    
    // Creates a (gatekeeper) socket.
    serv_sock = socket(PF_INET, SOCK_STREAM, 0); 
    if(serv_sock == -1) error_handling("socket() error");
    
    //Initialize the server address structure (gatekeeper)
    memset(&serv_adr, 0, sizeof(serv_adr)); 
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_adr.sin_port = htons(atoi(argv[1])); 
    
    //在调用bind之前设置socket选项SO_REUSEADDR
    //表示允许地址重用, 否则close之后会有一个WAIT_TIME状态，使得该ip和端口仍>被占用，产生bind() error.
    int on = 1;
    if (setsockopt(serv_sock, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(int)) < 0)  error_handling("setsockopt() error");
    
    // Bind the (gatekeeper) socket, also listening socket later.
    if(bind(serv_sock, (struct sockaddr*) &serv_adr, sizeof(serv_adr)) == -1) error_handling("bind() error"); 
    
    //Listen for incoming connections，set backlog queue of size 5.
    if(listen(serv_sock, 5) == -1) error_handling("listen() error");
    
    int i, str_len;
    clnt_adr_sz = sizeof(clnt_adr); //initialize clnt_adr_sz
    // Accept up to 5 client connections
    for (i = 0; i < 5; ++i) {
        // 从全连接队列中取出连接，并返回与对应客户端进行连接的连接socket
        clnt_sock = accept(serv_sock, (struct sockaddr*) &clnt_adr, &clnt_adr_sz); 
        if(clnt_sock == -1) error_handling("accept() error");
        else printf("the %dth connected client \n", i + 1);
        
        // Read data from the client and write it back
        while ((str_len = read(clnt_sock, message, BUF_SIZE)) != 0) 
        	write(clnt_sock, message, str_len);
        close(clnt_sock); //关闭与客户端通信的连接socket
    }
    close(serv_sock); // Close the server socket (gatekeeper)
    return 0;
}
void error_handling(char* message) {
    perror(message);  // 打印错误信息
    exit(1);  // 退出程序
}
```

#### 迭代客户端 `echo_clnt.c`

```c++
#include <stdio.h>      // Standard input/output functions
#include <stdlib.h>     // Standard library functions
#include <string.h>     // String handling functions
#include <unistd.h>     // Unix standard functions
#include <arpa/inet.h>  // IP address conversion functions
#include <sys/socket.h> // Socket functions
#define BUF_SIZE 1024   // Define a constant for the buffer size
void error_handling(char* message); // Function prototype for error handling

int main(int argc, char* argv[]) {
    //argv[0] is the program name,argv[1] and argv[2] are the IP and port for the target server socket (gatekeeper Socket)
    int clnt_sock;
    char message[BUF_SIZE]; //Buffer to hold messages
    struct sockaddr_in serv_adr; //Server address structure
    
    // Check if the correct number of command line arguments is provided
    if(argc != 3) { //Three parameters are required, one is the program name, and the other two are the IP and port number
        printf("Usage : %s <IP> <port>\n", argv[0]);
        exit(1);
    }   
    //Creates a TCP socket.
    clnt_sock = socket(PF_INET, SOCK_STREAM, 0); 
    if(clnt_sock == -1) error_handling("socket() error");
    //Initialize the server address structure
    memset(&serv_adr, 0, sizeof(serv_adr)); //Clears the structure for server address.   
    serv_adr.sin_family = AF_INET; //Sets the address family to IPv4
    serv_adr.sin_addr.s_addr = inet_addr(argv[1]); //Sets the server IP address
    serv_adr.sin_port = htons(atoi(argv[2])); //Sets the server port.
    // Connects to the server,and connect() will also assign address info to client socket        
    if(connect(clnt_sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr)) == -1) error_handling("connect() error");    
    else puts("Connected!");
    int str_len;
    // Main loop for communication:
    while (1) {
        fputs("Input message(Q to quit): ", stdout);//standard output
        fgets(message, BUF_SIZE, stdin);//User input is read with fgets and sent to the server with write.        
        //communication is terminated when the user enters "q" or "Q"
        if (!strcmp(message, "q\n") || !strcmp(message, "Q\n")) break;
        // Send the message to the server
        write(clnt_sock, message, strlen(message));
        // Receive a response from the server
        str_len = read(clnt_sock, message, BUF_SIZE - 1);
        message[str_len] = 0;
        printf("Message from server: %s", message);
    }
    close(clnt_sock);
    return 0;
}
// Function definition for error handling
void error_handling(char* message) {
    perror(message);  // print error info
    exit(1);  // exit the program
}
```

> 关于第42和43行为什么read最大字节数是 `BUF_SIZE - 1` 以及为什么要有`message[str_len] = 0;`？

这是因为在 C 语言中，通常用 null 终止字符 (`'\0'`) 来表示字符串的结束。

而 `read()` 函数从套接字中读取数据时，如果读取的字节数达到 `BUF_SIZE - 1`，那么还需要为字符串的结尾添加 null 终止符。

这就要求在读取数据时留出一个位置来存放 null 终止符，所以使用 `BUF_SIZE - 1` 作为读取的最大字节数，确保在读取完数据后，可以在 `message` 数组的末尾添加 null 终止符，使其成为一个有效的 C 字符串。

随后的 `message[str_len] = 0;` 这行代码就是在读取了数据后，在 `message` 数组中最后一个字符后添加 null 终止符，以确保接收到的数据可以被当作 C 字符串来处理。

这种做法可以保证接收到的数据不会超出 `message` 数组的范围，同时可以方便地将其视为字符串进行处理。

#### 运行结果

```c++
# 服务器端
[root@ECSocar ch04]#gcc echo_serv.c -o echo_serv
[root@ECSocar ch04]#./echo_serv 9190
the 1th connected client 
the 2th connected client 
    
# 客户端
[root@ECSocar ch04]#gcc echo_clnt.c -o echo_clnt
[root@ECSocar ch04]#./echo_clnt 127.0.0.1 9190
Connected!
Input message(Q to quit): hi
Message from server: hi
Input message(Q to quit): q
[root@ECSocar ch04]# ./echo_clnt 127.0.0.1 9190
Connected!
Input message(Q to quit): q
```

### 回声客户端的完美实现 

#### 回声客户端的问题

由第二章：服务器端多次调用（次数自拟）write函数传输数据，客户端调用1次read函数进行读取`tcp_clnt_one_read.c`可知，我们上面的代码存在问题。

因为回声客户端是通过一次调用read来一次性接收服务器传输的所有数据，而TCP数据报是基于流服务的数据报，数据不存在边界，因此一次性read不一定就能读取完全部数据。

要想达到【一次read能读取完全部数据】这一目的，客户端需延迟调用read函数，因为客户端要等待服务器端传输所有数据。那要等多久呢？等10分钟吗？显然不行，因为正常来说客户端应该马上就能收到服务器发送的消息。

#### 解决方法

提前确定之后要接收的数据的大小。

这是可以提前确定的，因为服务器就是将客户端发的数据原封不动发回给客户端，因此若最开始客户端给服务器发了20字节的数据，则之后接收数据时循环调用read()读取20个字节即可。

即 将原来的`echo_clnt.c`中的这三行：

```c++
        write(clnt_sock, message, strlen(message));
        str_len = read(clnt_sock, message, BUF_SIZE - 1);
        message[str_len] = 0;
```

修改为：

```c++
       //先在循环体外定义int recv_len, recv_cnt;

        str_len = write(clnt_sock, message, strlen(message)); //提前确定之后要接收的数据的大小
        recv_len = 0; //read调用的循环变量
        while (recv_len < str_len) { //只要没接收完就一直循环
            recv_cnt = read(clnt_sock, &message[recv_len], BUF_SIZE - 1);
            if (recv_cnt == -1) error_handling("read() error ! ");
            recv_len += recv_cnt;
        }
        message[recv_len] = 0;
```

#### 这样就完了吗？

我们上面的解决办法只适用于客户端可以提前知道接收的数据长度，但大多情况下这是不可能的。既然如此，无法预知接受数据长度时应该如何收发数据呢？

此时就需要的是应用层协议的定义。之前回声服务器端/客户端中定义了如下协议："收到Q(q)就立即终止连接"。同样，收发数据也需要定好规则（协议）以表示数据的边界，或提前告知收发数据的大小。

由此也可以见得，应用层协议只不过是为特点程序的实现而制定的规则。

#### 编写程序以体验应用层协议的定义过程

> 实现：服务器端从客户端获得多个数字和运算符信息，然后对其进行加减乘运算，再把结果传回客户端。
> 例如，向服务器端传递3、5、9的同时请求加法运算，则客户端收到3+5+9的运算结果。其它运算同理。

定义的协议有：

- 客户端连接到服务器端后以**1字节整数**形式传递**操作数个数**。
- 然后传递**操作数**，都是**整型**数据，占用4字节。
- 接着传递**运算符**，运算符占用**1字节**。选择字符+、一、*之一传递。
- 服务器端以**4字节整型数**向客户端传回**运算结果**。
- 客户端得到运算结果后终止与服务器端的连接。

##### `op_serv.c`

```c++
#include <stdio.h>     
#include <stdlib.h>    
#include <string.h>     
#include <unistd.h>
#include <arpa/inet.h>  
#include <sys/socket.h> 
#define BUF_SIZE 1024 
#define OPSZ 4  //每个操作数都是int整型数，都占4字节
void error_handling(char* message);
int calculate(int opcnt, int opinfo[], char operator);
//参数分别为：操作数的个数，保存操作数据的字符数组，操作符

int main(int argc, char* argv[]) {
    int  serv_sock, clnt_sock;
    char opinfo[BUF_SIZE];
    struct sockaddr_in serv_adr, clnt_adr; 
    socklen_t clnt_adr_sz; 
    
    if(argc != 2) { 
        printf("Usage : %s <port>\n", argv[0]);
        exit(1);
    }   
    
    serv_sock = socket(PF_INET, SOCK_STREAM, 0); 
    if(serv_sock == -1) error_handling("socket() error");
    
    memset(&serv_adr, 0, sizeof(serv_adr)); 
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_adr.sin_port = htons(atoi(argv[1])); 
    //允许地址重用
    int on = 1;
    if (setsockopt(serv_sock, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(int)) < 0)  error_handling("setsockopt() error");
    
    if(bind(serv_sock, (struct sockaddr*) &serv_adr, sizeof(serv_adr)) == -1) error_handling("bind() error"); 
    
    if(listen(serv_sock, 5) == -1) error_handling("listen() error");
    
    clnt_adr_sz = sizeof(clnt_adr); 
    int i, res, recv_len, recv_cnt, opcnt;
    // Accept up to 5 client connections
    for (i = 0; i < 5; ++i) {
        // 从监听队列中取出一个连接请求进行连接，并返回与对应客户端进行连接的连接socket
        clnt_sock = accept(serv_sock, (struct sockaddr*) &clnt_adr, &clnt_adr_sz); 
        if(clnt_sock == -1) error_handling("accept() error");
        else printf("the %dth connected client \n", i + 1);
        
        // Read data from the client and write the result back
        opcnt = 0, recv_len = 0; //接收的数据的总字节数
        //首先接收操作数的个数
        read(clnt_sock, &opcnt, 1); //将读到的头个字符保存到opcnt变量中
        //再读操作数以及操作符
        while (recv_len < (opcnt * OPSZ + 1)) { 
            //从第一个操作数开始，将数据读到opinfo数组中，一次最多读BUF_SIZE-1
            recv_cnt = read(clnt_sock, &opinfo[recv_len], BUF_SIZE - 1);
            recv_len += recv_cnt; 
        } 
        //做运算并将结果用res保存，再传给客户端
        res = calculate(opcnt, (int*)opinfo, opinfo[recv_len - 1]);
        //参数分别为：操作数的个数，保存数据的字符数组，数组最后一个元素（操作符）
        write(clnt_sock, (char*) &res, sizeof(res));
        //将指向整型数res的指针转换为指向char型的指针（指针即地址）
        close(clnt_sock); 
    }
    close(serv_sock); 
    return 0;
}
//参数分别为：操作数的个数，保存数据的字符数组，操作符
int calculate(int opcnt, int opinfo[], char operator) {
    int res = opinfo[0], i; //opinfo中没有存操作数的个数，直接从操作数开始存的
    switch(operator) {
        case '+':
            for (i = 1; i < opcnt; ++i) res += opinfo[i];
            break;
        case '-':
            for (i = 1; i < opcnt; ++i) res -= opinfo[i];
            break;
        case '*':
            for (i = 1; i < opcnt; ++i) res *= opinfo[i];
            break;
    }
    return res;
}
void error_handling(char* message) {
    perror(message);  
    exit(1);  
}
```

##### `op_clnt.c`

```c++
#include <stdio.h>     
#include <stdlib.h>    
#include <string.h>     
#include <unistd.h>
#include <arpa/inet.h>  
#include <sys/socket.h> 
#define BUF_SIZE 1024 
#define RLT_SIZE 4 //结果是4字节int整型数，占4字节
#define OPSZ 4  //每个操作数都是int整型数，都占4字节
void error_handling(char* message);

int main(int argc, char* argv[]) {
    int clnt_sock;
    char opMsg[BUF_SIZE]; 
    //要想在同一数组保存并传输多种数据类型，应当把数组声明为char类型，并且之后需要额外做一些指针转换
    struct sockaddr_in serv_adr; 
    
    if(argc != 3) { 
        printf("Usage : %s <IP> <port>\n", argv[0]);
        exit(1);
    }   
    
    clnt_sock = socket(PF_INET, SOCK_STREAM, 0); 
    if(clnt_sock == -1) error_handling("socket() error");
    
    memset(&serv_adr, 0, sizeof(serv_adr)); 
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_addr.s_addr = inet_addr(argv[1]); 
    serv_adr.sin_port = htons(atoi(argv[2])); 
    
    if(connect(clnt_sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr)) == -1) error_handling("connect() error");    
    else puts("Connected!");
    
    //从用户那边读取数据到opMsg
    int res, opcnt, i; 
    fputs("Operand count : ", stdout); 
    scanf("%d", &opcnt); //用户输入操作数的个数
    //`scanf` 用于读取用户输入的数据，而 `fputs` 用于向标准输出打印提示信息或其他字符串
    //因为协议第一条规定客户端以1字节整数形式传递操作数个数，
    opMsg[0] = (char) opcnt; //因此要将int强转成char存到字符数组，占一个字节
    
    for (i = 0; i <opcnt ; ++i) { //输入多个操作数
        printf("Operand %d: ", i + 1);
        //将用户输入的操作数保存到opMsg中
        scanf("%d", (int*)&opMsg[i * OPSZ + 1]); //协议第二条规定操作数为整型数，占用4字节，因此强转(char*)为(int*)
        //scanf 函数中 %d 格式说明符 要求传递一个指向整数的指针，以便将输入的整数值存储到该指针指向的内存位置。而 opMsg 是一个字符数组
    }
    fgetc(stdin); 
    //读取并丢弃标准输入缓冲区中的一个字符。这通常是为了消耗掉用户输入中的换行符或其他潜在的剩余字符。
    //在这里是为了处理用户在输入操作数后按下 Enter 键留下的换行符'\n'。
    fputs("Operator : ", stdout); 
    //协议第二条规定操作符占用1字节(char)，不用转换
    scanf("%c", &opMsg[opcnt * OPSZ + 1]); 
    //传输opMsg中的数据到服务器
    write(clnt_sock, opMsg, opcnt * OPSZ + 2); 
    //将从服务器端得到的结果(4字节)读到res变量中
    read(clnt_sock, &res, RLT_SIZE); //RLT_SIZE=4
    
    printf("Operation result: %d", res);    
    close(clnt_sock);
    return 0;
}
void error_handling(char* message) {
    perror(message); 
    exit(1);  
}
```

##### 运行结果

```c++
# 服务器端
[root@ECSocar ch05]# gcc op_serv.c -o op_serv
[root@ECSocar ch05]# ./op_serv 9190
the 1th connected client 
^C
# 客户端 
[root@ECSocar ch05]# gcc op_clnt.c -o op_clnt
[root@ECSocar ch05]# ./op_clnt 127.0.0.1 9190
Connected!
Operand count : 3
Operand 1: 2
Operand 2: 4
Operand 3: 3
Operator : *
Operation result: 24 
```

## 基于UDP的服务器端/客户端

### 客户端地址分配

> UDP客户端什么时候分配IP和端口号？

由于`bind()`不区分TCP还是UDP，因此UDP中也可以使用`bind()`来分配地址。
若调用`sendto()`函数时发现尚未分配地址，则`sendto()`函数会自动为其分配IP和端口号。因此，UDP客户端通常无需额外的地址分配过程。

### 实现UDP回声服务器/客户端

#### `uecho_serv.c` 

```c++
#include <stdio.h>     
#include <stdlib.h>   
#include <string.h>     
#include <unistd.h>    
#include <arpa/inet.h>
#include <sys/socket.h> 
#define BUF_SIZE 30
void error_handling(char* message); 

int main(int argc, char* argv[]) {
    int  serv_sock;
    char message[BUF_SIZE]; //Buffer for storing received and to-be-sent messages
    struct sockaddr_in serv_adr, clnt_adr; 
    socklen_t clnt_adr_sz; 
    
    if(argc != 2) { 
        printf("Usage : %s <port>\n", argv[0]);
        exit(1);
    }   
    
    // Creates a UDP socket.
    serv_sock = socket(PF_INET, SOCK_DGRAM, 0); 
    if(serv_sock == -1) error_handling("UDP socket() creation error");
   
    // Initialize the server address structure 
    memset(&serv_adr, 0, sizeof(serv_adr)); 
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_adr.sin_port = htons(atoi(argv[1])); 
   
    // Set socket option SO_REUSEADDR to allow the reuse of the address.
    int on = 1;
    if (setsockopt(serv_sock, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(int)) < 0)  error_handling("setsockopt() error");
    
    // Bind the socket to the specified address and port.
    if(bind(serv_sock, (struct sockaddr*) &serv_adr, sizeof(serv_adr)) == -1) error_handling("bind() error"); 
     
    int str_len;
    // this UDP server as it is designed to run indefinitely, 
    // Enter an infinite loop to continuously receive and send data.
    while (1) { 
        clnt_adr_sz = sizeof(clnt_adr); //initialize clnt_adr_sz
        //不限制数据传输对象，因此第一个参数不是clnt_sock
        // Use recvfrom to receive data from any client (clnt_adr is populated with client's address).
        str_len = recvfrom(serv_sock, message, BUF_SIZE, 0, (struct sockaddr*)&clnt_adr, &clnt_adr_sz);
        //recvfrom调用会获取数据传输端的地址，并填充到clnt_adr（因此serv端知道是从哪个客户端接收的数据），正是利用该地址将接收的数据逆向传输 
        // Send the received data back to the same client
        sendto(serv_sock, message, str_len, 0, (struct sockaddr*)&clnt_adr, clnt_adr_sz);
    }
    //没有break，也就是说是无限循环的，因此close()不会执行，没有意义
    close(serv_sock); 
    return 0;
}
void error_handling(char* message) {
    perror(message);  
    exit(1);  
}
```

#### `uecho_clnt.c`

```c++
#include <stdio.h>     
#include <stdlib.h>   
#include <string.h>     
#include <unistd.h>    
#include <arpa/inet.h>
#include <sys/socket.h> 
#define BUF_SIZE 30
void error_handling(char* message); 

int main(int argc, char* argv[]) {
    int clnt_sock;
    char message[BUF_SIZE];
    struct sockaddr_in serv_adr, from_adr;
    socklen_t src_adr_sz;//接收数据时，数据发送端的socket地址的长度
    if(argc != 3) { 
        printf("Usage : %s <IP> <port>\n", argv[0]);
        exit(1);
    }   

    clnt_sock = socket(PF_INET, SOCK_DGRAM, 0); 
    if(clnt_sock == -1) error_handling("socket() error");

    memset(&serv_adr, 0, sizeof(serv_adr)); 
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_addr.s_addr = inet_addr(argv[1]); 
    serv_adr.sin_port = htons(atoi(argv[2])); 
    
    int str_len;
    while (1) {
        fputs("Insert message(Q to quit): ", stdout);
        fgets(message, sizeof(message), stdin);
        //communication is terminated when the user enters "q" or "Q"
        if (!strcmp(message, "q\n") || !strcmp(message, "Q\n")) break;
        
        // Send the message to the server
        sendto(clnt_sock, message, strlen(message), 0, (struct sockaddr*)&serv_adr, sizeof(serv_adr));
        // Receive a response from src
        src_adr_sz = sizeof(src_adr);
        str_len = recvfrom(clnt_sock, message, BUF_SIZE , 0, (struct sockaddr*)&src_adr, &src_adr_sz));
        message[str_len] = 0;
        printf("Message from server: %s", message);
    }
    close(clnt_sock);
    return 0;
}
void error_handling(char* message) {
    perror(message);  
    exit(1); 
}
```

#### 运行结果

```c++
# 服务器端
[root@ECSocar ch06]# gcc uecho_serv.c -o uecho_serv
[root@ECSocar ch06]# ./uecho_serv 9190
^C
    
# 客户端
[root@ECSocar ch06]# gcc uecho_clnt.c -o uecho_clnt
[root@ECSocar ch06]# ./uecho_clnt 127.0.0.1 9190
Insert message(Q to quit): hi,nihaoya
Message from server: hi,nihaoya
Insert message(Q to quit): how are you?
Message from server: how are you?
Insert message(Q to quit): q
```



### TCP_serv & UDP_serv的差异

![image-20231219090704174](https://cdn.jsdelivr.net/gh/808bass666/picBed@main/202312190907336.png)

将中间部分放在一起对比能发现：

- tcp_serv只需要对`clnt_adr_sz`初始化一次，且是在循环前就进行了初始化；而udp_serv每一次传输数据前都需要初始化`clnt_adr_sz`，且是在循环内部一开始就进行初始化。**但是私以为这并不是TCP_serv和UDP_serv的差异，tcp_serv也应该在for内部每次accept调用前对`clnt_ad_sz`进行一次初始化。**
    因为accept的第三个参数addrlen是value-result argument(传入传出参数)，每次传入的都应该是sockaddr结构体的大小，传出客户端地址的实际大小（accept调用会修改`clnt_adr_sz`的值）。
    如果与下一个客户端进行连接之前不重新初始化，那么再调用accept时传入的`clnt_adr_sz`就是上一个客户端地址的实际大小，可能会小于我们要传出(返回)的、当前这个客户端的地址的实际大小，导致返回的客户端地址信息被截断。
- TCP连接需要`listen() + accept()`先listen监听连接然后accept从全连接队列中取出连接，也就是需要【建立连接】的这个过程，而UDP不需要建立连接就可以直接通信。
- TCP接收数据时不需要指定客户端地址和地址长度，而UDP需要指定。且tcp_serv是直接从 前面调用accept()时取得的客户端socket（`clnt_sock`） 读取数据，而udp_serv是 直接从`serv_sock`中读取数据，在recvfrom()调用的过程中取得客户端socket。
- 传输数据和接收数据差不多同理，就不赘述了

### UDP的数据传输特性 (存在数据边界)

#### `bound_recv.c` 接收三次 代码类服务器端

```c++
#include <stdio.h>    
#include <stdlib.h>   
#include <string.h>    
#include <unistd.h>    
#include <arpa/inet.h>
#include <sys/socket.h> 
#define BUF_SIZE 30
void error_handling(char* message); 

int main(int argc, char* argv[]) {
    int sock;
    char message[BUF_SIZE];
    struct sockaddr_in my_adr, your_adr;
    socklen_t adr_sz;
    if(argc != 2) { 
        printf("Usage : %s <port>\n", argv[0]);
        exit(1);
    }   

    sock = socket(PF_INET, SOCK_DGRAM, 0); 
    if(sock == -1) error_handling("socket() error");

    memset(&my_adr, 0, sizeof(my_adr)); 
    my_adr.sin_family = AF_INET;
    my_adr.sin_addr.s_addr = htonl(INADDR_ANY); 
    my_adr.sin_port = htons(atoi(argv[1])); 
    
    if (bind(sock, (struct sockaddr*) &my_adr, sizeof(my_adr)) == -1) error_handling("bind() error");
    
    int str_len, i;
    for (i = 0; i < 3; ++i) { //接收三次
        sleep(5); //delay 5 sec
        // 为了验证UDP传输特性，每隔5秒调用一次recvfrom()。
        // 对方调用3次sendto()发送数据，本端调用3次recvfrom()接收数据，因为每次recvfrom的调用都延迟了5秒，因此调用recvfrom前对方已经调用完了3次sendto，也就是说此时数据已经>传输到了本端
        // 如果是TCP，则只需要调用一次recvfrom就可以读取所有数据；UDP则必须要对应地调用3次recvfrom才能接收完全部数据
        adr_sz = sizeof(your_adr);
        str_len = recvfrom(sock, message, BUF_SIZE , 0, (struct sockaddr*)&your_adr, &adr_sz);
        printf("Message %d: %s \n", i + 1, message);
    }   
    close(sock);
    return 0;
}
void error_handling(char* message) {
    perror(message);  
    exit(1); 
}
```

#### `bound_send.c` 发送三次  代码类客户端

```c++
#include <stdio.h>    
#include <stdlib.h>   
#include <string.h>    
#include <unistd.h>    
#include <arpa/inet.h>
#include <sys/socket.h> 
#define BUF_SIZE 30
void error_handling(char* message); 

int main(int argc, char* argv[]) {
    int sock;
    char msg1[] = "Hi!";
    char msg2[] = "I'm another UDP host!";
    char msg3[] = "Nice to meet you";
    struct sockaddr_in your_adr; //要接收本端数据的主机的socket地址
    socklen_t your_adr_sz;
    
    if(argc != 3) { 
        printf("Usage : %s <IP> <port>\n", argv[0]);
        exit(1);
    }   

    sock = socket(PF_INET, SOCK_DGRAM, 0); 
    if(sock == -1) error_handling("socket() error");

    memset(&your_adr, 0, sizeof(your_adr)); 
    your_adr.sin_family = AF_INET;
    your_adr.sin_addr.s_addr = inet_addr(argv[1]); 
    your_adr.sin_port = htons(atoi(argv[2])); 
    
    sendto(sock, msg1, sizeof(msg1), 0, (struct sockaddr*) &your_adr, sizeof(your_adr));
    sendto(sock, msg2, sizeof(msg2), 0, (struct sockaddr*) &your_adr, sizeof(your_adr));
    sendto(sock, msg3, sizeof(msg3), 0, (struct sockaddr*) &your_adr, sizeof(your_adr));
    close(sock);
    return 0;
}
void error_handling(char* message) {
    perror(message);  
    exit(1); 
}
```

#### 运行结果

由`bound_recv.c`的运行结果知，共调用了recvfrom()三次。这就证明了UDP通信过程中收发次数必须保存一致。

```c++
# recv端 类服务器端
[root@ECSocar ch06]# gcc bound_recv.c -o bound_recv
[root@ECSocar ch06]# ./bound_recv 9190
Message 1: Hi! 
Message 2: I'm another UDP host! 
Message 3: Nice to meet you 
# send端 类客户端
[root@ECSocar ch06]# gcc bound_send.c -o bound_send
[root@ECSocar ch06]# ./bound_send 127.0.0.1 9190
```

### `connected / unconnected`UDP套接字

TCP套接字中需注册待传输数据的目标IP和端口号，而UDP中则无需注册。因此，通过sendto函数传输数据的过程大致可分为以下3个阶段。

- 第1阶段: 向UDP套接字注册目标IP和端口号。
- 第2阶段: 传输数据。
- 第3阶段: 删除UDP套接字中注册的目标地址信息。

由于每次都变更目标地址，因此可以重复利用同一UDP套接字向不同目标传输数据。这种未注册目标信息的套接字称为未连接套接字（`unconnected`套接字）。反之，注册了目标地址的套接字称为连接套接字（`connected`套接字）。

显然，UDP套接字 **默认** 属于 未连接套接字。

但是，在需要向同一主机进行长时间通信时，应该让UDP套接字变成连接套接字，少去每次都要注册和删除目的地址信息的步骤，提高效率和性能。

#### 创建 `connected`UDP套接字

只需要针对UDP套接字调用`connect()`即可让其变为已连接套接字。

当然，针对UDP套接字调用connect函数并不意味着要与对方UDP套接字连接，这只是向UDP套接字注册目标P和端口信息。

之后就与TCP套接字一样，每次调用sendto函数时只需传输数据。因为已经指定了收发对象，所以不仅可以使用sendto、recvfrom函数，还可以使用write、read函数进行通信。

下列示例将之前的`uecho_clnt.c`程序改成`基于已连接UDP套接字`的程序，因此可以结合`uecho_serv.c`程序运行。另外，为便于说明,未直接删除uecho_clnt.c的I/O函数，而是添加了注释。代码中用`write、read`函数代替了`sendto、recvfrom`函数。

#### `uecho_con_clnt.c`

```c++
#include <stdio.h>     
#include <stdlib.h>   
#include <string.h>     
#include <unistd.h>    
#include <arpa/inet.h>
#include <sys/socket.h> 
#define BUF_SIZE 30
void error_handling(char* message); 

int main(int argc, char* argv[]) {
    int clnt_sock;
    char message[BUF_SIZE];
    //struct sockaddr_in serv_adr, from_adr; //不再需要from_adr！
    struct sockaddr_in serv_adr; 
    //socklen_t src_adr_sz; //多余变量！
    if(argc != 3) { 
        printf("Usage : %s <IP> <port>\n", argv[0]);
        exit(1);
    }   

    clnt_sock = socket(PF_INET, SOCK_DGRAM, 0); 
    if(clnt_sock == -1) error_handling("socket() error");

    memset(&serv_adr, 0, sizeof(serv_adr)); 
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_addr.s_addr = inet_addr(argv[1]); 
    serv_adr.sin_port = htons(atoi(argv[2])); 
    
    //添加connect函数使之变为【已连接套接字】！
    connect(clnt_sock, (struct sockaddr*) &serv_adr, sizeof(serv_adr));
    
    int str_len;
    while (1) {
        fputs("Insert message(Q to quit): ", stdout);
        fgets(message, sizeof(message), stdin);
        if (!strcmp(message, "q\n") || !strcmp(message, "Q\n")) break;

        /*sendto(clnt_sock, message, strlen(message), 0, (struct sockaddr*)&serv_adr, sizeof(serv_adr)); 
        可以直接用write()了 */
        write(clnt_sock, message, strlen(message));

        /*src_adr_sz = sizeof(src_adr);
        str_len = recvfrom(clnt_sock, message, BUF_SIZE , 0, (struct sockaddr*)&src_adr, &src_adr_sz));
        可以直接用read()了 */
        str_len = read(clnt_sock, message, strlen(message) - 1);
        
        message[str_len] = 0;
        printf("Message from server: %s \n", message);
    }
    close(clnt_sock);
    return 0;
}
void error_handling(char* message) {
    perror(message);  
    exit(1); 
}
```

## 优雅地断开套接字连接

### shutdown

### 半关闭

![image-20231219203314560](https://cdn.jsdelivr.net/gh/808bass666/picBed@main/202312192033620.png)

### 基于半关闭的文件传输程序

#### `halfclose_serv`

```c++
#include <stdio.h>     
#include <stdlib.h>    
#include <string.h>     
#include <unistd.h>
#include <arpa/inet.h>  
#include <sys/socket.h> 
#define BUF_SIZE 30 
void error_handling(char* message);

int main(int argc, char* argv[]) {
    int  serv_sock, clnt_sock;
    FILE *fp;
    struct sockaddr_in serv_adr, clnt_adr; 
    socklen_t clnt_adr_sz; 
    char buf[BUF_SIZE];
    
    if(argc != 2) { 
        printf("Usage : %s <port>\n", argv[0]);
        exit(1);
    }   

    if((fp = fopen("halfclose_serv.c", "rb"))== NULL) printf("File Open Failed !\n");
    else printf("File Open Success !\n");
    
    serv_sock = socket(PF_INET, SOCK_STREAM, 0); 
    if(serv_sock == -1) error_handling("socket() error");

    memset(&serv_adr, 0, sizeof(serv_adr)); 
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_adr.sin_port = htons(atoi(argv[1])); 
    
    int on = 1;
    if (setsockopt(serv_sock, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(int)) < 0)  error_handling("setsockopt() error");
    
    if(bind(serv_sock, (struct sockaddr*) &serv_adr, sizeof(serv_adr)) == -1) error_handling("bind() error"); 
    
    if(listen(serv_sock, 5) == -1) error_handling("listen() error");
    
    clnt_adr_sz = sizeof(clnt_adr); 
    clnt_sock = accept(serv_sock, (struct sockaddr*) &clnt_adr, &clnt_adr_sz); 
    if(clnt_sock == -1) error_handling("accept() error");
    
    // Read data from the file and write the file to client
    int read_cnt = 0;
    while (1) {
        read_cnt = fread((void*)buf, 1, BUF_SIZE, fp);
                //从fp文件描述符中读数据到buf
                if (read_cnt < BUF_SIZE)  {
                    write(clnt_sock, buf, read_cnt); //将从文件中读到的read_cnt个字节写入客户端
                    break; //写完就退出
                }
                //如果read_cnt >= BUF_SIZE，也只能先将BUF_SIZE大小的数据写到客户端，之后继续循环传剩下的
                write(clnt_sock, buf, BUF_SIZE); 
    }
    shutdown(clnt_sock, SHUT_WR);//关闭写的那一半
    read(clnt_sock, buf, BUF_SIZE);//继续从客户端接收数据
    printf("Message from client: %s \n", buf);

    fclose(fp);
    close(clnt_sock); 
    close(serv_sock); 
    return 0;
}
void error_handling(char* message) {
    perror(message);  
    exit(1);  
}
```

#### `halfclose_clnt`

```c++
#include <stdio.h>     
#include <stdlib.h>    
#include <string.h>     
#include <unistd.h>
#include <arpa/inet.h>  
#include <sys/socket.h> 
#define BUF_SIZE 30 
void error_handling(char* message);

int main(int argc, char* argv[]) {
    int clnt_sock;
    struct sockaddr_in serv_adr; 
    FILE *fp;
    char buf[BUF_SIZE]; //存放从服务器端接收过来的数据
    
    if(argc != 3) { 
        printf("Usage : %s <IP> <port>\n", argv[0]);
        exit(1);
    }   
   
    fp = fopen("received.dat","wb");//创建新文件以保存服务器端传输的文件数据 （"w"没有则创建）
    clnt_sock = socket(PF_INET, SOCK_STREAM, 0); 
    if(clnt_sock == -1) error_handling("socket() error");
    
    memset(&serv_adr, 0, sizeof(serv_adr)); 
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_addr.s_addr = inet_addr(argv[1]); 
    serv_adr.sin_port = htons(atoi(argv[2])); 
    
    if(connect(clnt_sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr)) == -1) error_handling("connect() error");    
    else puts("Connected!");
	
    int read_cnt = 0;
    while ((read_cnt = read(clnt_sock, buf, BUF_SIZE)) != 0) fwrite((void*)buf, sizeof(char), read_cnt, fp);
    puts("received!");
    write(clnt_sock, "Thank you", 10);
    fclose(fp);
    close(clnt_sock);
    return 0;
}
void error_handling(char* message) {
    perror(message); 
    exit(1);  
}
```

#### 运行结果

```c++
# 服务器端
[root@ECSocar ch07]# gcc halfclose_serv.c -o halfclose_serv
[root@ECSocar ch07]# ./halfclose_serv 9190
File Open Success !
Message from client: Thank you 
# 客户端
[root@ECSocar ch07]# gcc halfclose_clnt.c -o halfclose_clnt
[root@ECSocar ch07]# ./halfclose_clnt 127.0.0.1 9190
Connected!
received!
```

![image-20231219202821720](https://cdn.jsdelivr.net/gh/808bass666/picBed@main/202312192028827.png)

## 域名及网络地址

> 对于gethostbyname和gethostbyaddr，要获取错误信息应该使用herror这个函数。

### `gethostbyname`利用域名获取IP

[【gethostbyname尽量少用】](https://www.cnblogs.com/anfflee/p/3460595.html)

gethostbyname函数不仅支持IPv4，还支持IPv6.

> `**`是表示指针的指针，或者说二维数组，或者说字符串集合。

```c++
//传递字符串格式的域名获取IP
struct hostent* gethostbyname(const char *hostname);
//成功时返回hostent结构体地址（返回时地址信息装入hostent结构体），失败时返回NULL指针
struct hostent {
    char * h_name; 	    // 存有official domain name官方域名（代表某一主页）
    char ** h_aliases;    // alias list（可以通过多个域名访问同一主页（同一IP可以绑定多个域名），因此，除官方域名外还可指定其他域名，这些都存在h_aliases中）
    int h_addrtype;       // host address type（可以通过此变量获取保存在h_addr_list的IP地址的地址族信息，比如如果是IPV4则该变量就是AF_INET）
    int h_length; 	      // address length（保存IP地址长度。IPv4地址是4，IPv6是16）
    char ** h_addr_list; // address list 指向字符串指针数组（由多个字符串地址构成的数组），这是最重要的成员。通过此变量以整数形式保存域名对应的IP地址（用户较多的网站有可能分配【多个IP给同一域名】，利用【多个服务器】进行【负载均衡】，因此会有多个对应的IP地址）
}
```

调用gethostbyname函数后返回的hostent结构体的变量结构如图所示：

![image-20231219230559729](https://cdn.jsdelivr.net/gh/808bass666/picBed@main/202312192305914.png)

![image-20231219231717811](https://cdn.jsdelivr.net/gh/808bass666/picBed@main/202312192317960.png)

#### `getHostByName.c`

```c++
#include <stdio.h>     
#include <stdlib.h>   
//#include <string.h>     
#include <unistd.h>    
#include <arpa/inet.h>
//#include <sys/socket.h> 
#include <netdb.h>
void error_handling(char* message); 

int main(int argc, char* argv[]) {
        //argv[1]是字符串格式的域名
        struct hostent *host; 

        if(argc != 2) { 
            printf("Usage : %s <addr>\n", argv[0]); //提供正确运行参数指导
            exit(1);
        }   

        //传递字符串格式的域名获取IP
        host = gethostbyname(argv[1]);
        if(!host) error_handling("gethost... error");
	
	//打印地址结构体中的每个变量
	// (1) 打印h_name官方域名
	printf("Official name: %s \n", host->h_name);
        int i;
	// (2) 打印h_aliases除官方域名外的其它域名
	for(i = 0; host->h_aliases[i]; ++i) printf("Aliases %d: %s \n", i + 1, host->h_aliases[i]);
        // (3) 打印h_addr_list中的ip地址的地址族
	printf("Address Type: %s \n", (host->h_addrtype == AF_INET) ? "AF_INET" : "AF_INET6");
	// (4) 省略打印h_length ip地址长度,IPv4地址是4，IPv6是16
	// (5) 打印h_addr_list 域名多个对应的IP地址
	for(i = 0; host->h_addr_list[i]; ++i) printf("IP addr %d: %s \n", i + 1, inet_ntoa(*(struct in_addr*)host->h_addr_list[i]));
        /* in_addr 是 struct sockaddr_in 中表示 ipv4 地址的结构体
            struct in_addr {
                In_addr_t        s_addr;//32位ipv4地址（要用网络字节序表示）
            }
        */
	return 0;
}
void error_handling(char* message) {
    herror(message);  
    exit(1);  
}

```

##### 运行结果

```c++
[root@ECSocar ch08]# gcc getHostByName.c -o getHostByName
[root@ECSocar ch08]# ./getHostByName www.baidu.com
Official name: www.a.shifen.com 
Aliases 1: www.baidu.com 
Address Type: AF_INET 
IP addr 1: 183.2.172.185 
IP addr 2: 183.2.172.42 
```

```c++
[root@ECSocar ch08]# ./getHostByName eol.sicau.edu.cn
Official name: eol.sicau.edu.cn 
Address Type: AF_INET 
IP addr 1: 113.54.15.40
```

### `gethostbyname`利用IP获取域名

```c++
struct hostent* gethostbyaddr(const char *addr, socklen_t len, int family);
//成功时返回hostent结构体变量地址值，失败时返回NULL 指针。
p1: 含有IP地址信息的in_addr结构体指针。为了能兼容除IPv4地址之外的其他地址，该变量的类型声明为char指针而不是sockaddr_in。
p2: 向第一个参数传递的ip地址字节数，IPv4为4，IPv6为16。
p3: 传递地址族信息，IPv4为AF_INET，IPv6为AF_INET6。
```



## 套接字的多种可选项



----

> **《ch10 ch11 ch12》同时向多个客户端提供服务的并发服务器 的实现模型和方法：**
>
> 1. 多进程服务器：创建多个进程
> 2. 多路复用服务器：捆绑并统一管理I/O对象
> 3. 多线程服务器：生成与客户端等量的线程

----

## 多进程服务器端

### 进程概念及应用

#### 进程相关概念

- 进程：占用内存的正在运行的程序。
- 进程ID：进程号，PID。可通过`ps -au`查看所有进程的详细信息（有PID）
- 进程阻塞（等待、封锁、休眠）：
    - 正在进行的进程由于发生某事件而暂时无法继续执行，要等待相应的事件出现后才被唤醒，这就称为进程阻塞。
    - 比如程序运行到sleep或者recv时，程序会从运行状态变为阻塞（等待）状态。
    - **进程阻塞时会被加入该socket的等待队列中，并不会出现在内核中，也不会往下执行代码，因此是不占用cpu资源的**

#### fork创建进程

创建进程的方式有很多种，这里只介绍用于创建多进程服务器端的fork函数.

```c++
pid_t fork(void);
//成功返回PID，失败返回-1
```

fork将创建调用的==进程副本==，也就是**并非根据不同的程序创建进程**，**而是复制正在运行的、调用fork()的那个进程。**另外，**两个进程都将执行fork()函数调用后的语句**。

> 那两个进程看上去长得一模一样，要如何区分这两个进程呢？————根据fork函数返回值

- 父进程（原进程，调用fork的那个）：返回值为子进程ID
- 子进程（进程副本）：返回值为0

![image-20231222204027160](https://cdn.jsdelivr.net/gh/808bass666/picBed@main/202312222040247.png)

进程复制完成后根据fork函数的返回类型区分父子进程。

注意！父进程将lval的值加1，但这不会影响子进程的lval值。同样，子进程将gval的值加1也不会影响到父进程的gval。**因为fork函数调用后分成了完全不同的进程，只是二者共享同一代码而已。**

##### `fork.c`

```c++
#include <stdio.h>
#include <unistd.h>

int gval = 10;
int main(int argc, char* argv[]) {
	pid_t pid;
	int lval = 20;
	gval++, lval += 5; //gval = 11, lval = 25

	pid = fork();//创建子进程

	if (pid == 0) { // if child process
		gval += 2, lval += 2; 
		printf("Child Proc: [%d, %d] \n", gval, lval);//[13,27]
	}
	else {
		gval -= 2, lval -= 2;//if parent process
		printf("Parent Proc: [%d, %d] \n", gval, lval);//[9,23]
	}
	return 0;
}
```

###### 执行结果

```c++
[root@ECSocar tcpip]# gcc fork.c -o fork
[root@ECSocar tcpip]# ./fork 
Parent Proc: [9, 23] 
Child Proc: [13, 27] 
```

### 僵尸进程

如果进程完成工作后未被及时销毁，那该状态下的进程就是"僵尸进程"，会占用系统的重要资源。

#### 产生原因 

先说说 子进程的几种终止方式：

- 调用`exit(参数);`
- main中`return 返回值;`
- 运行时发生致命错误或收到终止信号

【exit的参值数、main的return的返回值】都会传递给OS，**但OS不会销毁子进程，子进程只能由父进程来销毁（回收）。**

**因此在子进程完成工作后，一直到被父进程销毁的过程中，子进程都是处于僵尸进程状态。**

那么父进程怎么销毁（回收）子进程呢？

**父进程要调用`wait()或waitpid()`向OS主动发起请求**回收子进程，OS才会传递这些值；否则OS将一直保存，此间子进程就一直处于僵尸进程状态。

如果父进程不主动回收，则处于僵尸状态的子进程，会在父进程终止时被销毁。

但是如果该进程的父进程已经先结束了，那么该进程就不会变成僵尸进程。因为每个进程结束的时候,系统都会扫描当前系统中所运行的所有进程，看看有没有哪个进程是刚刚结束的这个进程的子进程，如果是的话，就由Init进程来接管他，成为他的父进程，从而保证每个进程都会有一个父进程。而Init进程会自动wait其子进程,因此被Init接管的所有进程都不会变成僵尸进程。

> 孤儿进程被init进程接管，由于init进程会循环调用wait()回收资源，因此并没有什么危害

##### `zombie`

```c++
int main(int argc, char* argv[]) {
	pid_t pid = fork();//创建子进程

	if (pid == 0) { // if child process
		puts("Hi, I am a child process");
	}
	else {
		printf("child process ID: %d \n", pid);
		sleep(30); // sleep 30 sec
	}
	if (pid == 0) puts("End child process");
	else puts("End parent process");
	return 0;
}
```

###### 分析

![image-20231222203356851](https://cdn.jsdelivr.net/gh/808bass666/picBed@main/202312222039465.png)

###### 执行结果

```c++
[root@ECSocar ch10]# gcc zombie.c -o zombie
[root@ECSocar ch10]# ./zombie 
child process ID: 6279 
Hi, I am a child process
End child process
//hold 30s
End parent process
```

若在这30s内查看子进程的状态，可以看到PID为6279的进程状态为Z+(僵尸进程)。经过30s等待时间后，父进程和子进程同时销毁。

![image-20231222211122489](https://cdn.jsdelivr.net/gh/808bass666/picBed@main/202312222111553.png)

#### 销毁方式

有两种方式：父进程调用`wait() / waitpid()`收集子进程。

父进程一旦调用了`wait() / waitpid()`，就立即阻塞自己（挂起HANG ON）（当然也可以自己实现不阻塞）；由`wait() / waitpid()`自动分析 是否当前进程的某个子进程 已经退出，

- 如果找到了这样的已经变成僵尸的子进程，`wait() / waitpid()`就会收集这个子进程的信息，把它彻底销毁后返回。
- 如果没有找到，
    - 如果是用`wait()`，则父进程就会阻塞，直到有一个子进程结束为止（因此需谨慎调用wait）
    - 如果是用`waitpid()`，那么即使没有终止的子进程也不会阻塞，而是返回0并退出函数

##### wait()

```c++
pid_t wait(int *status);
//如果成功找到一个，返回被收集的终止子进程的PID；如果没找到，返回-1，同时errno被置为ECHILD
status用于保存 被收集的子进程 退出时的一些状态，包括 返回值(exit的参值数、main的return的返回值) 和 一些用宏表示的状态信息。
但如果我们并不关心这些，则可以设置该参数为NULL
```

如果需要查看这些信息（子进程的退出状况），我们可以用一些宏来查看：

![image-20240107111422305](https://cdn.jsdelivr.net/gh/808bass666/picBed@main/202401071114406.png)

举例：

```c++
if (WIFEXITED(status)) { //如果子进程正常结束
    puts("Normal termination!"); //打印提示信息
    printf("Child pass num: %d \n", WEXITSTATUS(status)); //打印子进程的返回值 
}
```

###### `wait.c`

```c++
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char* argv[]) {
	int status; 
	pid_t pid = fork();//创建第一个子进程
	if (pid == 0) return 3;//第一个子进程在main中执行return并返回值给操作系统，此时父进程还未向OS请求获取子进程返回值(exit的参值数、main的return的返回值)，该状态下的子进程就是僵尸进程
	else {
		printf("child PID: %d \n", pid);
		pid = fork();//创建第二个子进程
		if (pid == 0) exit(7);//第二个子进程调用exit()并传参给操作系统，此时父进程还未向OS···，该状态下的子进程就是僵尸进程
		else {
			printf("child PID: %d \n", pid);
			//利用wait()，父进程主动向OS请求获取子进程的返回值，销毁僵尸进程
			//销毁第一个子进程(僵尸进程)
			wait(&status); 
			if (WIFEXITED(status)) printf("Child send one: %d \n", WEXITSTATUS(status));
			//销毁第二个子进程(僵尸进程)
			wait(&status); 
			if (WIFEXITED(status)) printf("Child send two: %d \n", WEXITSTATUS(status));
			sleep(30); // sleep 30 sec
		}
	}
	return 0;
}
```

###### 执行结果

```c++
[root@ECSocar ch10]# gcc wait.c -o wait
[root@ECSocar ch10]# ./wait 
child PID: 31514 
child PID: 31515 
Child send one: 3 
Child send two: 7 
//过程中用ps -au是查不到这两个子进程的，因为调用wait()销毁了它们
```

##### waitpid()

wait与waitpid区别：

1. wait()可能导致程序阻塞，而waitpid 有一个选项参数，可使调用者不阻塞。
2. 而且waitpid()不仅可以等待子进程，还可以等待进程组中的任意一个进程
3. waitpid并不等待第一个终止的子进程——它有若干个选择项，可以控制它所等待的特定进程。
4. 实际上wait函数是waitpid函数的一个特例。waitpid(-1, &status, 0);

因此`waitpid()`会是更好的选择。

```c++
pid_t waitpid(pid_t pid, int *status, int options);
//成功返回终止的子进程PID，失败返回-1
```

**参数详解：**

p1: pid，要终止的进程的PID，不同参数代表的意思也不同，当pid 

- \< -1 只等待一个指定进程中的任何子进程，这个进程组的ID等于pid的绝对值
- = -1 等待任意子进程终止，此时等同于`wait()
- = 0 只等待同一个进程组中的任一子进程
- \> 0 只等待PID=pid的子进程。

p2: status, 同前面

p3: options，目前在linux中只支持常量`WNOHANG`(WAIT NO HANG)和`WUNTRACED`这两个选项（声明在<sys/wait.h>中），可用"|"将它们连接起来使用。如果不想使用它们，也可以传0.

- 若传递常量WNOHANG, 那么即使没有终止的子进程也不会进入阻塞状态，而是立即返回0

###### `waitpid.c`

```c++
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char* argv[]) {
	int status; 
        pid_t child_pid;
	pid_t pid = fork();//创建子进程
	if (pid == 0){
                printf("我是子进程，我的PID=%d\n",getpid());
		sleep(10);//暂停10s后子进程再执行return并返回值给OS
		return 24;//子进程返回值
	}
	else {
		//父进程不会等待子进程，若没有已终止的子进程则返回0，由于父进程用了while循环不断的收集子进程，所以会一直收集直到有终止的子进程
            printf("我是父进程，我要等的进程id是%d\n",pid);
            do {
		child_pid = waitpid(pid, &status, WNOHANG);
                if (child_pid == 0) {
                        puts("没有收集到子进程! sleep 3sec.");
			sleep(3); 
		}
            }while(child_pid == 0);//等不到就继续等 
            if (pid == child_pid) {
                puts("等到了子进程！！！");
                if (WIFEXITED(status)) printf("我是父进程，我收集的子进程的返回值是: %d \n", WEXITSTATUS(status));//应该显示24
            }
            else puts("出错了");
	}
	return 0;
}
```

###### 运行结果

```c++
[root@ECSocar ch10]# gcc waitpid.c -o waitpid
[root@ECSocar ch10]# ./waitpid 
sleep 3sec.
sleep 3sec.
sleep 3sec.
sleep 3sec.
sleep 3sec.
Child send: 24 
//执行了5次sleep(子程序在return前sleep了15s)，也证明了waipid函数并未阻塞
    
[root@ECSocar ch10]# gcc waitpid2.c -o waitpid2
[root@ECSocar ch10]# ./waitpid2
我是父进程, 我要等的进程id是22340
没有收集到子进程! sleep 3sec.
我是子进程, 我的PID=22340
没有收集到子进程! sleep 3sec.
没有收集到子进程! sleep 3sec.
没有收集到子进程! sleep 3sec.
等到了子进程！！！
我是父进程，我收集的子进程的返回值是: 24 
```

### 信号处理 

#### 向OS求助

我们已经知道了父进程怎么回收子进程，那么父进程要什么时候回收子进程呢？总不能像上面的代码一样不断循环探测是否有终止的子进程吧？

要是能让OS直接告诉正忙于工作的父进程"你的子进程终止了!"，那就省事多了！而信号处理机制就能实现这个作用！

#### 信号与signal函数

"信号": 在**特定事件发生时**由OS向进程发送的消息。

#### 利用sigaction函数来进行信号处理 



#### 利用信号处理技术消灭僵尸进程

当子进程结束时，将产生`SIGCHLD`信号，OS就用信号告诉父进程"嘿，你的子进程结束了！"，从而父进程能及时回收自己的子进程，使之不至于变成僵尸进程。

##### `kill_zombie.c`

```c++
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

void read_childproc(int sig){ 
    int status;
    pid_t pid = waitpid(-1, &status, WNOHANG);
    if (WIFEXITED(status)) {
        printf("Removed child_PID: %d \n", pid);
        printf("child return: %d \n", WEXITSTATUS(status));
    }
}
int main(int argc, char *argv[]) {
    pid_t pid;
    //声明sigaction结构体变量act
    struct sigaction act;
    //初始化结构体变量act
    act.sa_handler = read_childproc;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    
    //通过sigaction函数注册SIGCHLD信号对应的handler
    sigaction(SIGCHLD, &act, 0);
    
    pid = fork(); //fork第一个子进程A
    if(pid == 0) {
        printf("Hi！I'm first child %d \n", getpid());
        sleep(10);
        return 12;
    }
    else {
        printf("first child_PID: %d \n", pid);
        pid = fork(); //fork第二个子进程B
        if(pid == 0) {
            printf("Hi！I'm second child %d \n", getpid());
            sleep(10);
            exit(24);
        }
        else {
            int i;
            printf("second child_PID: %d \n", pid);
            for (i = 0; i < 5; ++i) {
                puts("wait...");
                sleep(5);
            }
        }
    }
    return 0;
}
```

###### 分析

![image-20240110180714613](https://cdn.jsdelivr.net/gh/808bass666/picBed@main/202401101807834.png)

###### 运行结果

```c++
[root@ECSocar ch10]# gcc kill_zombie.c -o kill_zombie
[root@ECSocar ch10]# ./kill_zombie 
first child_PID: 5829 
Hi！I'm first child 5829 
second child_PID: 5830 
wait...
Hi！I'm second child 5830 
wait...
Removed child_PID: 5829 
child return: 12 
wait...
Removed child_PID: 5830 
child return: 24 
wait...
wait...
```

### 基于多任务的并发服务器

##### `echo_mpserv.c`

```c++
#include <stdio.h>       
#include <stdlib.h>     
#include <string.h>      
#include <unistd.h>      
#include <signal.h>
#include <sys/wait.h>     
#include <arpa/inet.h>   
#include <sys/socket.h>  
#define BUF_SIZE 1024   
void read_childproc(int sig);
void error_handling(char* buf); 

int main(int argc, char* argv[]) { 
    int  serv_sock, clnt_sock; 
    char buf[BUF_SIZE];
	struct sockaddr_in serv_adr, clnt_adr; 
    socklen_t clnt_adr_sz;  
    pid_t pid;
    int str_len, state;
    
	if(argc != 2) {  
        printf("Usage : %s <port>\n", argv[0]);
        exit(1);
    }   
    // 这一段是注册SIGCHLD信号对应的handler，以在子进程结束产生SIGCHLD信号时系统能通过handler处理回收子进程，防止产生僵尸进程
    //(1)声明sigaction结构体变量act
    struct sigaction act;
    //(2)初始化结构体变量act
    act.sa_handler = read_childproc;
    sigemptyset(&act.sa_mask);
    act.sa_flags = SA_RESTART; 
    //若这里设置为0而不是设置为SA_RESTART，将会报错accept() error: Interrupted system call。
    //原因是:进程被信号中断导致系统调用被中断。具体在这里是:当信号对应的handler返回时，会中断进程的系统调用，如果此时的系统调用没有使用SA_RESTART标志，那么在信号返回后，它将无法恢复运行
    //(3)通过sigaction函数注册SIGCHLD信号对应的handler
    state = sigaction(SIGCHLD, &act, 0);
    
    serv_sock = socket(PF_INET, SOCK_STREAM, 0); 
    if(serv_sock == -1) error_handling("socket() error");
    
    memset(&serv_adr, 0, sizeof(serv_adr)); 
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_adr.sin_port = htons(atoi(argv[1])); 
    
    //允许地址重用
    int on = 1;
    if (setsockopt(serv_sock, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(int)) < 0)  error_handling("setsockopt() error");
    
    if(bind(serv_sock, (struct sockaddr*) &serv_adr, sizeof(serv_adr)) == -1) error_handling("bind() error"); 
     
    if(listen(serv_sock, 5) == -1) error_handling("listen() error");

    while(1) {
        clnt_adr_sz = sizeof(clnt_adr); //initialize clnt_adr_sz
        // 从监听队列中取出一个连接请求进行连接，并返回与对应客户端进行连接的连接socket
        clnt_sock = accept(serv_sock, (struct sockaddr*) &clnt_adr, &clnt_adr_sz); 
        if(clnt_sock == -1) continue;
        else puts("new client connected...");

        pid = fork();
        if (pid == -1) {
            close(clnt_sock);
            continue;
        }
        if (pid == 0) {
            close(serv_sock);  
            while ((str_len = read(clnt_sock, buf, BUF_SIZE)) != 0) write(clnt_sock, buf, str_len);
            close(clnt_sock); 
            puts("client disconnected..");
            return 0;
        }
        else close(clnt_sock);//如果是父进程则关闭clnt_sock（因为和客户端的通信全在子进程中进行）
    }
    close(serv_sock);  
    return 0;
}
void read_childproc(int sig){ 
    int status;
    pid_t pid = waitpid(-1, &status, WNOHANG);
    printf("Removed child_PID: %d \n", pid);
}
void error_handling(char* buf) {
    perror(buf);  // 打印错误信息
    exit(1);  // 退出程序
}
```

##### `echo_clnt.c` ch04

##### 执行结果

###### `echo_mpserv.c`

```c++
[root@ECSocar ch10]# gcc echo_mpserv.c -o echo_mpserv
[root@ECSocar ch10]# ./echo_mpserv 9190
new client connected...
new client connected...
client disconnected..
Removed child_PID: 29101 
client disconnected..
Removed child_PID: 29056 
```

###### `echo_clnt.c` first_client

```c++
[root@ECSocar ch04]# ./echo_clnt 127.0.0.1 9190
Connected!
Input message(Q to quit): hi! i'm first client~
Message from server: hi! i'm first client~
Input message(Q to quit): bye~
Message from server: bye~
Input message(Q to quit): q
```

###### `echo_clnt.c` second_client

```c++
[root@ECSocar ch04]# ./echo_clnt 127.0.0.1 9190
Connected!
Input message(Q to quit): hi! i'm second client~
Message from server: hi! i'm second client~
Input message(Q to quit): nihao
Message from server: nihao
Input message(Q to quit): q
```



### 分割TCP的I/O程序



#### `echo_mpclnt.c`

```c++
#include <stdio.h>       
#include <stdlib.h>     
#include <string.h>      
#include <unistd.h>      
#include <arpa/inet.h>   
#include <sys/socket.h>  
#define BUF_SIZE 30
void read_routine(int sock, char* buf); 
void write_routine(int sock, char* buf); 
void error_handling(char* message); 

int main(int argc, char* argv[]) {
    int clnt_sock;
    char buf[BUF_SIZE];  
    struct sockaddr_in serv_adr; 
    
    if(argc != 3) {  
        printf("Usage : %s <IP> <port>\n", argv[0]);
        exit(1);
    }   
    
    clnt_sock = socket(PF_INET, SOCK_STREAM, 0); 
    if(clnt_sock == -1) error_handling("socket() error");
    
    memset(&serv_adr, 0, sizeof(serv_adr));  
    serv_adr.sin_family = AF_INET;  
    serv_adr.sin_addr.s_addr = inet_addr(argv[1]);  
    serv_adr.sin_port = htons(atoi(argv[2])); 
    
    if(connect(clnt_sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr)) == -1) error_handling("connect() error");    
    else puts("Connected!");

    pid_t pid = fork();
    //下面就是I/O分离的实现
    if (pid == 0) write_routine(clnt_sock, buf); //子进程负责写(发送)
    else read_routine(clnt_sock, buf); //父进程负责读(接收)
    
    close(clnt_sock);
    return 0;
}
void read_routine(int sock, char *buf) {
    while (1) {
        int str_len = read(sock, buf, BUF_SIZE - 1);
        if(str_len == 0) return;
        buf[str_len] = 0;
        printf("Message from server: %s", buf);
    }
}
void write_routine(int sock, char *buf) {
    while (1) {
        fgets(buf, BUF_SIZE, stdin); 
        if (!strcmp(buf, "q\n") || !strcmp(buf, "Q\n")) {
            shutdown(sock, SHUT_WR);
            return;
        }
        write(sock, buf, strlen(buf));
    }
}
void error_handling(char* message) {
    perror(message); 
    exit(1);  
}
```

##### 执行结果

###### `echo_mpserv.c`

```c++
[root@ECSocar ch10]# ./echo_mpserv 9190
new client connected...
new client connected...
client disconnected..
Removed child_PID: 30821 
client disconnected..
Removed child_PID: 30851 
```

###### `echo_mpclnt.c` first_client

```c++
[root@ECSocar ch10]# ./echo_mpclnt 127.0.0.1 9190
Connected!
hi! I'm first client
Message from server: hi! I'm first client
bye~
Message from server: bye~
q
```

###### `echo_clnt.c` second_client

```c++
[root@ECSocar ch10]# ./echo_mpclnt 127.0.0.1 9190
Connected!
hi! I'm second client
Message from server: hi! I'm second client
bye bye~
Message from server: bye bye~
q
```



## `ch11`.进程间通信

进程间通信机制：管道通信、消息队列、共享内存、信号量、信号等。

##### `pipe1.c`

```c++
#include <stdio.h>
#include <unistd.h>
#define BUF_SIZE 30

int main(int argc, char *argv[]){
	int fds[2];
	char str[]="Who are you?";
	char buf[BUF_SIZE];
	pid_t pid;

	pipe(fds);

	pid=fork();

	if(pid==0)
		write(fds[1], str, sizeof(str));
	else{
		read(fds[0], buf, BUF_SIZE);
		puts(buf);
	}
	return 0;
}
```

##### `pipe2.c`

```c++
#include <stdio.h>
#include <unistd.h>
#define BUF_SIZE 30

int main(int argc, char *argv[]){
	int fds[2];
	char str[]="Who are you?";
	char str2[]="Thank you for your message";
	char buf[BUF_SIZE];
	pid_t pid;

	pipe(fds);

	pid=fork();

	if(pid==0){
		write(fds[1], str, sizeof(str));
		sleep(2);//若不加这一行则子进程会把本来要通过管道传给父进程的数据先取走,导致父进程取不到
		read(fds[0], buf, BUF_SIZE);
		printf("Child proc output: %s \n", buf);
	}
	else{
		read(fds[0], buf, BUF_SIZE);
		printf("Parent proc output: %s \n", buf);
		write(fds[1], str2, sizeof(str2));
		sleep(3);//这里和前面子进程中sleep(2)的作用不一样，这里只是为了防止父进程先结束，导致父进程输出完到子进程打印输出(终止前)的这中间，会出现终端的命令提示符显示（也就是并非必要但是加上最好）
	}
	return 0;
}
```

###### 运行结果

```c++
Parent proc output: Who are you? 
Child proc output: Thank you for your message 
```

##### `pipe3.c`

```c++
#include <stdio.h>
#include <unistd.h>
#define BUF_SIZE 30

int main(int argc, char *argv[]){
	int fds1[2], fds2[2];
	char str[]="Who are you?";
	char str2[]="Thank you for your message";
	char buf[BUF_SIZE];
	pid_t pid;

	pipe(fds1), pipe(fds2);

	pid=fork();

	if(pid==0){
		write(fds1[1], str, sizeof(str));
		read(fds2[0], buf, BUF_SIZE);
		printf("Child proc output: %s \n", buf);
	}
	else{
		read(fds1[0], buf, BUF_SIZE);
		printf("Parent proc output: %s \n", buf);
		write(fds2[1], str2, sizeof(str2));
		sleep(3);//为了延迟父进程终止
	}
	return 0;
}
```

###### 运行结果

```c++
Parent proc output: Who are you? 
Child proc output: Thank you for your message 
```

##### `echo_storeserv.c`

```c++
#include <stdio.h>       
#include <stdlib.h>     
#include <string.h>      
#include <unistd.h>      
#include <signal.h>
#include <sys/wait.h>     
#include <arpa/inet.h>   
#include <sys/socket.h>  
#define BUF_SIZE 100  
void read_childproc(int sig);
void error_handling(char* buf); 

//将回声客户端传输的字符串按序保存到文件中(在子进程中实现)
//数据的流动是从客户端->服务器端的子进程B->服务器端的子进程A->文件

int main(int argc, char* argv[]) { 
    int  serv_sock, clnt_sock; 
    char buf[BUF_SIZE];
	struct sockaddr_in serv_adr, clnt_adr; 
    socklen_t clnt_adr_sz;  
    int str_len, state;
    pid_t pid;
    int fds[2];
    
	if(argc != 2) {  
        printf("Usage : %s <port>\n", argv[0]);
        exit(1);
    }   

    struct sigaction act;
    act.sa_handler = read_childproc;
    sigemptyset(&act.sa_mask);
    act.sa_flags = SA_RESTART; 
    state = sigaction(SIGCHLD, &act, 0);
    
    serv_sock = socket(PF_INET, SOCK_STREAM, 0); 
    if(serv_sock == -1) error_handling("socket() error");
    
    memset(&serv_adr, 0, sizeof(serv_adr)); 
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_adr.sin_port = htons(atoi(argv[1])); 
    
    int on = 1;
    if (setsockopt(serv_sock, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(int)) < 0)  error_handling("setsockopt() error");
    
    if(bind(serv_sock, (struct sockaddr*) &serv_adr, sizeof(serv_adr)) == -1) error_handling("bind() error"); 
     
    if(listen(serv_sock, 5) == -1) error_handling("listen() error");

    pipe(fds);
    pid = fork();//第一个子进程A

    if (pid == 0){
		FILE *fp = fopen("echomsg.txt", "wt");
		char msgbuf[BUF_SIZE];
		int i, len;
		for(i = 0; i < 3; ++i) { //共3次的fwrite调用完成后，可以打开echomsg.txt验证保存的字符串,无论有多少个客户端链接，echomsg中都只会有三次传输的结果
			len = read(fds[0], msgbuf, BUF_SIZE);
			fwrite((void*)msgbuf, 1, len, fp);//将从管道读出的信息(保存在msgbuf中)写入文件
			//fwrite第二个参数是要写出数据的 基本单元 的字节大小，这里是sizeof(char)
		}
		fclose(fp);
	}
    
    while(1) {
        clnt_adr_sz = sizeof(clnt_adr); //initialize clnt_adr_sz
        // 从监听队列中取出一个连接请求进行连接，并返回与对应客户端进行连接的连接socket
        clnt_sock = accept(serv_sock, (struct sockaddr*) &clnt_adr, &clnt_adr_sz); 
        if(clnt_sock == -1) continue;
        else puts("new client connected...");

        pid = fork();//第二个子进程B
       
        if (pid == 0) {
            close(serv_sock);  
            while ((str_len = read(clnt_sock, buf, BUF_SIZE)) != 0) {
                write(clnt_sock, buf, str_len);
                write(fds[1], buf, str_len);//通过管道将传输的数据传输给子进程A
            }
            close(clnt_sock); 
            puts("client disconnected..");
            return 0;
        }
        else close(clnt_sock);
    }
    close(serv_sock);  
    return 0;
}
void read_childproc(int sig){ 
    int status;
    pid_t pid = waitpid(-1, &status, WNOHANG);
    printf("Removed child_PID: %d \n", pid);
}
void error_handling(char* buf) {
    perror(buf);   
    exit(1);   
}
```

###### 运行结果

服务器端

```c++
[root@ECSocar ch11]# ./echo_storeserv 9190
new client connected...
client disconnected..
Removed child_PID: 7659 
new client connected...
client disconnected..
Removed child_PID: 7669 
new client connected...
client disconnected..
Removed child_PID: 7678 
new client connected...
client disconnected..
Removed child_PID: 7688 
new client connected...
client disconnected..
Removed child_PID: 7695 
^C
```

客户端

```c++
[root@ECSocar ch10]# ./echo_mpclnt 127.0.0.1 9190
Connected!
one
Message from server: one
q
[root@ECSocar ch10]# ./echo_mpclnt 127.0.0.1 9190
Connected!
two
Message from server: two
q
[root@ECSocar ch10]# ./echo_mpclnt 127.0.0.1 9190
Connected!
three
Message from server: three
q
[root@ECSocar ch10]# ./echo_mpclnt 127.0.0.1 9190
Connected!
four
Message from server: four
q
[root@ECSocar ch10]# ./echo_mpclnt 127.0.0.1 9190
Connected!
five
Message from server: five
q
```

`echomsg.txt`

```c++
one
two
three
```





## I/O多路复用

### 概念

IO多路复用机制就是一个进程通过记录跟踪每一个I/O描述符的状态来同时管理多个I/O流。

与多进程/多线程技术相比，I/O多路复用技术的最大优势时系统开销小，系统不必为每一个I/O事件创建一个进程/线程，这就减少了不必要的进程/线程上下文切换的系统开销，也避免了因维护这些进程/线程带来的系统开销。

### 实现原理

先构造一张有关描述符的表，每一个描述符对应着一个I/O事件，

然后进程将这些I/O事件告知内核，使得内核一旦发现进程指定的这些描述符中的一个或多个I/O事件就绪（一般而言是读就绪或者写就绪），内核就通知该进程进行相应的读写操作。

简单来说就是，单个进程/线程，通过记录跟踪每个I/O描述符的状态，来同时管理多个I/O事件。

### 使用场景

（1）当客户处理多个描述符时（一般是交互式输入和网络套接字），必须使用I/O复用。

（2）当一个客户端同时处理多个套接字时，而这种情况是可能的，但很少出现。

（3）如果一个TCP服务端既要处理监听套接字，又要处理已连接套接字，一般也要用到I/O复用。

（4）如果一个服务端即要处理TCP，又要处理UDP，一般要使用I/O复用。

（5）如果一个服务端要处理多个服务或多个协议，一般要使用I/O复用。

### **select函数**

进程调用 select系统调用 告知内核对哪些描述符（就读、写或异常条件）感兴趣以及等待多长时间。内核监控的描述符不局限于套接字，任何描述符都可以使用select来测试。

```cpp
#include <sys/select.h>
#include <sys/time.h>

int select(int maxfd, 
           fd_set *readset, fd_set *writeset, fd_set *exceptset, 
           const struct timeval *timeout)
//返回值：
    成功，返回已就绪描述符的数目；
    超时，返回值为0；同时未就绪的描述符会被置为0。
    失败（出错），返回值为-1，并设置相应的错误码给errno全局变量。
```

#### 函数参数

1. 在linux上，maxfd是**需要检测事件的所有 fd 中的最大值加 1**，从0、1、2...maxfd-1的fd均会被检测。

    > 所以每新产生一个 clientfd，都要与当前最大的 **maxfd** 作比较，如果大于当前的 **maxfd** ，则将 **maxfd** 更新成这个新的最大值。

2. readset、writeset和exceptset指定需要检测 **读、写和异常条件** 事件的fd。如果对某一个的条件不感兴趣，就可以把它设为空指针。

    - select使用描述符集`struct fd_set`，通常是一个整型数组，其中每个数组元素中的每一位对应一个文件描述符，即通过**位图**的方式来存储文件描述符。可通过以下四个宏进行设置：

    - ```c++
        void FD_ZERO(fd_set *fdset);      //清空集合（所有位全置0）
        void FD_SET(int fd, fd_set *fdset);  //在集合中注册文件描述符fd的信息
        void FD_CLR(int fd, fd_set *fdset);  //在集合中删除文件描述符fd的信息
        int FD_ISSET(int fd, fd_set *fdset);  // 检查集合中指定的文件描述符fd是否发生变化
        ```

3. timeout告知内核**等待所指定描述字中的任何一个就绪可花多少时间，这个参数有三种可能：**

    - ```c++
        struct timeval{
          long tv_sec;  //seconds
          long tv_usec; //microseconds
        };
        ```

    - 1. 永远等待下去：仅在有一个描述字准备好I/O时才返回。此时该参数要置为空指针NULL。
        2. 等待一段固定时间：有一个描述字准备好I/O就返回（等待时间不超过指定值）
        3. **根本不等待：检查描述字后立即返回，这称为轮询**。此时该参数必须指向一个timeval结构，而且其中的定时器值必须为0。

    - 【扩展】

        - 如果将select函数的中间3个参数readfds、writefds和exceptfds均置为空指针，我们就有了一个比sleep()函数更为精确的定时器（sleep以秒为最小单位）
          

#### 注意事项

1）调用select函数时，我们指定所关心的描述符的值，该函数返回时，**结果将指示哪些描述符已就绪**。该函数返回后，我们**使用FD_ISSET宏来测试**set描述符集中的描述符。

2）描述符集内任何与**未就绪描述符**对应的位在select函数返回时均会被清成0。为此，我们**每次重新调用select函数时，都得再次把所有描述符内所关心的描述符位置为1。**

3）使用select时最常见的两个编程错误是：

1. 忘了对最大描述符加1；
2. 忘了描述符集是值-结果参数。导致调用select函数时，描述符集内我们认为是1的位却被置为0了。

#### 文件描述符就绪条件

​        哪些情况下文件描述符可以被认为是可读、可写或者发生异常，对于 select 的使用非常关键。

- 下列情况下 socket 可读：
    - socket 内核接收缓冲区中的字节数大于或等于其低水位标记 `SO_RCVLOWAT`。此时我们可以无阻塞地读该socket，并且读操作返回的字节数大于0。
    - socket 通信对端关闭连接。此时对该 socket 的读操作将返回 0。
    - 监听 socket 上有新的连接请求。
    - socket 上有未处理的错误。此时我们可以使用 `getsockopt()` 函数来读取和清楚该错误。
- 下列情况下 socket 可写：
    - socket 内核发送缓冲区中的可用字节数大于或等于其低水位标记 `SO_SNDLOWAT`。此时我们可以无阻塞地写该socket，并且写操作返回的字节数大于0。
    - socket 的写操作被关闭。对写操作被关闭的 socket 执行写操作将触发一个 `SIGPIPE` 信号。
    - socket 使用非阻塞 connect 连接成功或者失败（超时）之后。也就是，将 socket 设置为非阻塞模式下，调用 connect 系统调用建立连接。
    - socket 上有未处理的错误。此时我们可以使用 `getsockopt() `函数来读取和清楚该错误。
- 网络编程中，select 能处理的异常情况只有一种：
    - socket 上接收到带外数据。

### select多路复用流程图

服务端有两个socket描述符，一个是listening sockt，另一个是accept成功的client读写的socket。
当有client连接成功后，就将这个clnt_fd添加到描述符集set中，
然后服务端接收客户端的消息，并返回消息，
之后关闭tcp连接，从描述符集中删除这个client描述符。

如果select函数返回超时，则关闭所有的socket描述符，结束服务端进程。

![img](https://cdn.jsdelivr.net/gh/808bass666/picBed@main/202401160929676.png)

### select 的缺陷

1. 单个进程可监视的描述符数量有限。32位机器默认是1024个，64位默认是2048。

2. 对描述符进行扫描的方式是**线性扫描，即采用轮询的方式，效率较低**。当套接字比较多的时候，每次调用select都需要在**内核**遍历传递进来的所有`FD_SETSIZE`个描述符来完成调度，不论那个描述符是不是活跃的。这会浪费很多CPU时间。

    如果能给套接字**注册**某个**回调函数，当他们活跃时，自动完成相关操作，那就避免了轮询，这正是epoll与kqueue做的**。

3. 需要维护一个用来存放描述符集的数据结构 (fd_set结构体)。**每次调用select时，都需要把它从用户空间拷贝到内核空间**，这样会使得用户空间和内核空间在传递该结构时**复制开销大**。

4. 每次重新调用select函数时，都得再次把所有描述符内所关心的位置为1，这个是非常不友好的。

5. 当有描述符就绪时，select仅仅会返回成功，但是并不会告诉你是哪个描述符准备好了，于是你只能一个个地找：即在循环中使用`if(宏函数FD_ISSET)`逐个判断寻找。

6. select函数**不是线程安全的函数**。如果你在线程1中将一个描述符加入到select的描述符集中，然后在线程2中，却将这个描述符给回收了，即close掉这个描述符，这会导致不可预测的后果。

### 代码

#### `select.c`

```c++
#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/select.h>
#define BUF_SIZE 30

int main(int argc, char *argv[]){
  int res, str_len;
  char buf[BUF_SIZE];

  fd_set reads, temps;
  struct timeval timeout;

  FD_ZERO(&reads);
  FD_SET(0, &reads);//fd=0表示标准输入的文件描述符

  while (1) {
    temps = reads;//复制fd_set变量
    
    timeout.tv_sec = 5;//设置超时时间为5 s + 0 ms
    timeout.tv_usec = 0;
    
    res = select(1, &temps, 0, 0, &timeout);
    // 将所有需要关注"是否存在待读取数据"的fd注册到tmp(这里第一个参数为1，只有一个要关注的文件描述符:fd=0)

    
    if (res == -1) { //发生错误select返回-1
      puts("select() error!");
      break;
    }
    //超时select返回0
    else if (res == 0) puts("Time-out!");
    else { //引关注的事件返回时select返回>0的值
      if (FD_ISSET(0, &temps)) { //若temps中文件描述符fd=0(标准输入)发生变化(这里的变化即存在待读取数据)
        // 则从fd=0(标准输入)中读取数据并输出
        str_len = read(0, buf, BUF_SIZE); 
        buf[str_len] = '\0';//read不会在读取的数据后自动添加字符串结束符，因此我们要在读取到的数据后加上字符串结束符(\0)，确保输出的字符串正确结束。
        printf("message from console: %s", buf);
      }
    }
  }
}
```

运行结果

```c++
[root@ECSocar ch12]# ./select 
hi~
message from console: hi~
nihao~
message from console: nihao~
Time-out!
Time-out!
Time-out!
^C
```

#### `echo_SelectServ.c`

```c++
#include <stdio.h>    
#include <stdlib.h>      
#include <string.h>     
#include <unistd.h>    
#include <arpa/inet.h>  
#include <sys/socket.h>  
#define BUF_SIZE 100
void error_handling(char*buf);  

int main(int argc, char* argv[]) { 
    int serv_sock, clnt_sock; 
    char buf[BUF_SIZE];
  	struct sockaddr_in serv_adr, clnt_adr; 
    socklen_t clnt_adr_sz;  

    fd_set reads, cpy_reads;
    struct timeval timeout;
    int fd_max, fd_num, i, str_len;
  
  	if(argc != 2) {  
      printf("Usage : %s <port>\n", argv[0]);
      exit(1);
    }   
     
    serv_sock = socket(PF_INET, SOCK_STREAM, 0); 
    if(serv_sock == -1) error_handling("socket() error");
     
    memset(&serv_adr, 0, sizeof(serv_adr)); 
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_adr.sin_port = htons(atoi(argv[1])); 
     
    int on = 1;
    if (setsockopt(serv_sock, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(int)) < 0)  error_handling("setsockopt() error");
     
    if(bind(serv_sock, (struct sockaddr*) &serv_adr, sizeof(serv_adr)) == -1) error_handling("bind() error"); 
     
    if(listen(serv_sock, 5) == -1) error_handling("listen() error");

    FD_ZERO(&reads);//给read所有位初始化为0
    FD_SET(serv_sock, &reads);//在reads中注册文件描述符serv_sock的信息
    fd_max = serv_sock;//表示fd监视范围的右端点

    while (1) {
      cpy_reads = reads;

      timeout.tv_sec = 5;
      timeout.tv_usec = 5000;

      fd_num = select(fd_max + 1, &cpy_reads, 0, 0, &timeout);
      if (fd_num == -1) break;//发生错误返回1
      if (fd_num == 0) continue;//超时返回0
      
      for (i = 0; i < fd_max + 1; ++i) {
        if (FD_ISSET(i, &cpy_reads)) { //若fd=i的文件描述符发生变化
          if (i == serv_sock) { //若服务器端套接字发生变化，则受理连接请求
            clnt_adr_sz = sizeof(clnt_adr); 
            clnt_sock = accept(serv_sock, (struct sockaddr*) &clnt_adr, &clnt_adr_sz); 
            if(clnt_sock == -1) error_handling("accept() error");

            FD_SET(clnt_sock, &reads);//注册与客户端连接的套接字文件描述符
          
            if (fd_max < clnt_sock) fd_max=clnt_sock;
            printf("connected client: %d \n", clnt_sock);
          }
          else { //发生变化的并非服务器端套接字，此时需要确认接收的数据是字符串还是代表断开连接的EOF
            str_len = read(i, buf, BUF_SIZE);
            if (str_len == 0) { //接收的是EOF时，close request!
              FD_CLR(i, &reads);//从reads中删除该套接字的信息
              close(i);//关闭套接字
              printf("closed client: %d \n", i);
            }
            else write(i, buf, str_len);
          }
        }
      }
    }
    close(serv_sock); 
    return 0;
}
void error_handling(char *buf) {
    perror(buf);   
    exit(1);  
}
```







## epoll多线程服务器端

epoll相关有三个函数：`epoll_create`, `epoll_ctrl`, `epoll_wait`.

#### `epoll_create` 

```cpp
/*
* 创建一个epoll实例对象(表示epoll内核事件表的文件描述符)，
* 该描述符将用作其他epoll系统调用的第一个参数epfd;
*   size不起作用; 
*/
int epoll_create(int size)
```

#### `epoll_ctrl`

```cpp
/*
* 操作 内核事件表 监控的文件描述符上的事件：注册、修改、删除;
*   epfd: epoll实例对象(表示epoll内核事件表的文件描述符)
*   op：表示操作，用3个宏来表示;
*	fd: 需要注册/修改/删除操作的fd
*   event：epoll事件结构体，用于告诉内核需要监听的事件。存储op3的检测事件events以及data用户数据(通常就是文件描述符的值fd)
*/
int epoll_ctl(int epfd, int op, int fd, struct epoll_event *event)
    
struct epoll_event {
    __uint32_t events; /* 事件类型, 如EPOLLIN、EPOLLOUT、EPOLLET */
    epoll_data_t data; /* 用户自定义的数据类型 */
};
```

#### `epoll_wait`获取有事件发生的fd集合(就绪队列)

```cpp
/*
* 成功返回就绪的文件描述符个数，时间到时返回0，出错返回-1；
*   epfd: epoll实例对象(表示epoll内核事件表的fd)
*   events：就绪队列, 它是一个epoll_event结构体数组，存储了所有已就绪的fd的信息，调用epoll_wait()目的就是为了得到这个集合
*   maxevents：可供返回的最大事件大小，一般是events的大小
*   timeout：最大等待时间，设置为-1表示一直等待
*/
int epoll_wait(int epfd, struct epoll_event *events, int maxevents, int timeout)
```

### epoll 使用实例

可以看[epoll c和c++的简单例子（纯代码）_c++epoll具体实例-CSDN博客](https://blog.csdn.net/weixin_43326322/article/details/108288633)

```cpp
//创建epoll实例对象(表示epoll 内核事件表 的文件描述符)
int epfd = epoll_create(100);
//将用于监听的套接字添加到epoll实例中
struct epoll_event events[MAX_EVENTS], ev;
ev.events = EPOLLIN;    // 检测lfd读读缓冲区是否有数据
ev.data.fd = lfd;		//lfd即socket创建的
int ret = epoll_ctl(epfd, EPOLL_CTL_ADD, lfd, &ev);
/*
    op3: 要进行操作的fd
    op4: 存储op3的检测事件events以及data用户数据(通常就是文件描述符的值fd)
*/
//检测添加到epoll实例中的文件描述符是否已就绪，并将这些已就绪的文件描述符进行处理
int num = epoll_wait(epfd, &events, size, -1);
/*
    op1: epoll_create () 函数的返回值，表示epoll实例
    op2: 就绪队列
    op2: 修饰op2, 是op2的size
*/
//如果是监听的文件描述符，和新客户端建立连接，将得到的文件描述符添加到epoll实例中
int cfd = accept(curfd, NULL, NULL);
ev.events = EPOLLIN;
ev.data.fd = cfd;
epoll_ctl(epfd, EPOLL_CTL_ADD, cfd, &ev);	// 新得到的文件描述符添加到epoll模型中, 下一轮循环的时候就可以被检测了
//如果是通信的文件描述符，则和对应的客户端通信，如果连接已断开，将该文件描述符从epoll实例中删除
int len = recv(curfd, buf, sizeof(buf), 0);
if(len == 0) { // 将这个文件描述符从epoll模型中删除
    epoll_ctl(epfd, EPOLL_CTL_DEL, curfd, NULL);
    close(curfd);
} else if(len > 0) {
    send(curfd, buf, len, 0);
}
```



# 部分未写在前面的代码

## ch02 read,write

> 6. 实现：
>
>     1. 客户端多次调用read，以读取服务器调用1次write函数传递的字符串。
>
>     2. 客户端调用1次read，以读取服务器端多次调用（次数自拟）write函数传递的字符串。
>
>         为达到这一目的，客户端需延迟调用read函数，因为客户端要等待服务器端传输所有数据。

### 客户端多次调用read函数，以读取服务器调用1次write函数传递的字符串

#### `tcp_serv_one_write.c`

```c++
#include <stdio.h>  // 包含标准输入输出库
#include <stdlib.h>  // 包含标准库
#include <string.h>  // 包含字符串处理函数库
#include <unistd.h>  // 包含Unix标准函数库
#include <arpa/inet.h>  // 包含IP地址转换函数
#include <sys/socket.h>  // 包含套接字函数库
void error_handling(char* message); //错误处理函数

int main(int argc, char* argv[]) {
    //argv[0]是程序名，argv[1]是端口号
    int serv_sock, clnt_sock; //serv_sock是服务端socket(门卫), clnt_sock是要与客户端通信的连接socket
    struct sockaddr_in serv_addr, clnt_addr; //保存地址信息的结构体变量
    socklen_t clnt_addr_size = sizeof(clnt_addr); //初始化 与客户端通信的连接socket的地址结构体的大小
    char message[] = "hello world!"; //要写入（传输）的数据
    if (argc != 2) { // 如果参数数量不为2（程序名称也是一个参数，还要有端口号参数）
        printf("Usage: %s <port>\n", argv[0]);
        exit(1); // 退出程序
    }
    //创建门卫socket
    serv_sock = socket(PF_INET, SOCK_STREAM, 0); 
    if (serv_sock == -1) error_handling("socket() errror");
    //初始化门卫socket的地址信息
    memset(&serv_addr, 0, sizeof(serv_addr)); //初始化serv_addr结构体为0
    serv_addr.sin_family = AF_INET; // 设置地址族
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY); //设置IP
    serv_addr.sin_port = htons(atoi(argv[1])); //设置端口，argv[1]是传进来的第一个参
    //在门卫socket调用bind之前设置socket选项SO_REUSEADDR
    //表示允许地址重用, 否则close之后会有一个WAIT_TIME状态，使得该ip和端口仍被占用，产生bind() error.
    int on = 1;
    if (setsockopt(serv_sock, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(int)) < 0)  error_handling("setsockopt() error");
    //命名socket（绑定地址）
    if (bind(serv_sock, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) == -1)  error_handling("bind() error");
    //准备好了之后，门卫socket设置监听队列长度，开始监听连接请求
    if (listen(serv_sock, 5) == -1) error_handling("listen() error"); 
    //接受客户端连接请求，创建新的套接字clnt_sock用于与客户端通信
    //这个新的套接字负责和【特定的】客户端进行数据交换，而原始的服务器套接字(serv_sock)继续监听新的连接请求
    clnt_sock = accept(serv_sock, (struct sockaddr*) &clnt_addr, &clnt_addr_size);
    //clnt_addr是成功与服务器建立连接的客户端socket地址结构体，clnt_addr_size是其长度（传入时为sizeof(clnt_addr)，传出为实际大小，会被填入客户端socket地址结构体中）
    if (clnt_sock == -1) error_handling("accept() error");
    //向clnt_sock中一次write()写入（传输）数据
    write(clnt_sock, message, sizeof(message));
    close(clnt_sock); //关闭与客户端通信的连接socket
    close(serv_sock); //关闭服务端socket(门卫)
    return 0;
}
void error_handling(char* message) {
    perror(message);  // 打印错误信息
    exit(1);  // 退出程序
}
```

#### `tcp_clnt_many_read.c`

```c++
#include <stdio.h>  // 包含标准输入输出库
#include <stdlib.h>  // 包含标准库
#include <string.h>  // 包含字符串处理函数库
#include <unistd.h>  // 包含Unix标准函数库
#include <arpa/inet.h>  // 包含IP地址转换函数
#include <sys/socket.h>  // 包含套接字函数库
void error_handling(char* message); //错误处理函数

int main(int argc, char* argv[]) {
    //argv[0]是程序名，argv[1]和argv[2]是目标服务器端套接字（门卫socket）的ip和端口号
    int clnt_sock;
    struct sockaddr_in serv_addr; //地址信息
    char message[30]; //保存从服务器端读取（接收）过来的数据
    if (argc != 3) { // 如果参数数量不为3（程序名称也是一个参数，还需要服务器门卫socket的ip和端口号两个参数）
        printf("Usage: %s <IP> <port>\n", argv[0]);
        exit(1);  // 退出程序
    }
    //创建客户端socket
    clnt_sock = socket(PF_INET, SOCK_STREAM, 0);
    if (clnt_sock == -1) error_handling("socket() errror");
    //初始化目标服务器套接字（门卫socket）的地址信息信息
    //不用初始化客户端套接字的地址信息，因为它们在之后调用connect()时会自动分配
    memset(&serv_addr, 0, sizeof(serv_addr)); //初始化serv_addr结构体为0
    serv_addr.sin_family = AF_INET; // 设置地址族
    serv_addr.sin_addr.s_addr = inet_addr(argv[1]); //设置IP
    serv_addr.sin_port = htons(atoi(argv[2])); //设置端口
    //向服务器端发送连接请求，同时connect()自动会给客户端socket分配ip和端口号
    if (connect(clnt_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1) error_handling("connect() error");
    //完成连接后，从与之对应的服务器连接socket中读取（接收）数据
    int read_len = 0, idx = 0, str_len = 0;
    while (read_len = read(clnt_sock, &message[idx++], 1)){
        //read() p1: 要接收数据的socket，p2：要把数据读到哪里，p3: 要接收的最大字节数
        if(read_len == -1) error_handling("read() error");
        str_len += read_len;//统计read调用次数
    }
    printf("Message from server: %s \n", message); //打印读取（接收）的数据
    printf("Function read call count: %d \n", str_len); //打印read()调用次数
    close(clnt_sock);
    return 0;
}
void error_handling(char* message) {
    perror(message);  // 打印错误信息
    exit(1);  // 退出程序
}
```

#### 运行与结果显示：

```c++
# 服务器端
gcc   tcp_serv_one_write.c   -o   serv_one_wr
./serv_one_wr 9190  # 正常情况下程序将停留在此状态，因为服务器端调用的accept()还未返回，接下来运行客户端
# 客户端
gcc   tcp_clnt_many_read.c   -o   clnt_many_rd
./clnt_many_rd  127.0.0.1  9190
# 下面是客户端显示的结果
Message from server: hello world! 
Function read call count: 13 
```

### 服务器端多次调用（次数自拟）write函数传输数据，客户端调用1次read函数进行读取

#### `tcp_serv_many_write.c` 

```c++
#include <stdio.h>  // 包含标准输入输出库
#include <stdlib.h>  // 包含标准库
#include <string.h>  // 包含字符串处理函数库
#include <unistd.h>  // 包含Unix标准函数库
#include <arpa/inet.h>  // 包含IP地址转换函数
#include <sys/socket.h>  // 包含套接字函数库
void error_handling(char* message); //错误处理函数

int main(int argc, char* argv[]) {
    //argv[0]是程序名，argv[1]是端口号
    int serv_sock, clnt_sock; //serv_sock是服务端socket(门卫), clnt_sock是要与客户端通信的连接socket
    struct sockaddr_in serv_addr, clnt_addr; //保存地址信息的结构体变量
    socklen_t clnt_addr_size = sizeof(clnt_addr); //初始化 与客户端通信的连接socket的地址结构体的大小
    char message[] = "hello world!"; //要写入（传输）的数据
    if (argc != 2) { // 如果参数数量不为2（程序名称也是一个参数，还要有端口号参数）
        printf("Usage: %s <port>\n", argv[0]);
        exit(1); // 退出程序
    }
    //创建门卫socket
    serv_sock = socket(PF_INET, SOCK_STREAM, 0); 
    if (serv_sock == -1) error_handling("socket() errror");
    //初始化门卫socket的地址信息
    memset(&serv_addr, 0, sizeof(serv_addr)); //初始化serv_addr结构体为0
    serv_addr.sin_family = AF_INET; // 设置地址族
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY); //设置IP
    serv_addr.sin_port = htons(atoi(argv[1])); //设置端口，argv[1]是传进来的第一个参
    //在门卫socket调用bind之前设置socket选项SO_REUSEADDR
    //表示允许地址重用, 否则close之后会有一个WAIT_TIME状态，使得该ip和端口仍被占用，产生bind() error.
    int on = 1;
    if (setsockopt(serv_sock, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(int)) < 0)  error_handling("setsockopt() error");
    //命名socket（绑定地址）
    if (bind(serv_sock, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) == -1)  error_handling("bind() error");
    //准备好了之后，门卫socket设置监听队列长度，开始监听连接请求
    if (listen(serv_sock, 5) == -1) error_handling("listen() error"); 
    //接受客户端连接请求，创建新的套接字clnt_sock用于与客户端通信
    //这个新的套接字负责和【特定的】客户端进行数据交换，而原始的服务器套接字(serv_sock)继续监听新的连接请求
    clnt_sock = accept(serv_sock, (struct sockaddr*) &clnt_addr, &clnt_addr_size);
    //clnt_addr是成功与服务器建立连接的客户端socket地址结构体，clnt_addr_size是其长度（传入时为sizeof(clnt_addr)，传出为实际大小，会被填入客户端socket地址结构体中）
    if (clnt_sock == -1) error_handling("accept() error");    
    //向客户端套接字中多次write()写入（传输）数据
    //write(clnt_sock, message, 4);
    //write(clnt_sock, message + 4, 4);
    //write(clnt_sock, message + 8, 4);
    //write(clnt_sock, message + 12, sizeof(message) - 12);

    //或者写成：
    int inc = 0, len = strlen(message);
    while (len--) 
        if (write(clnt_sock, message + inc++, 1) == -1) error_handling("write() error");
    //如果第二个参数写成message++则会报错error: lvalue required as increment operand，因为++左侧被赋值的数必须是变量，而不能是常量，除了显而易见的常量之外，数组的首地址（比如这里的message），函数地址...都是常量
    close(clnt_sock); //关闭与客户端通信的连接socket
    close(serv_sock); //关闭服务端socket(门卫)
    return 0;
}
void error_handling(char* message) {
    perror(message);  // 打印错误信息
    exit(1);  // 退出程序
}
```

#### `tcp_clnt_one_read.c`

```c++
#include <stdio.h>  // 包含标准输入输出库
#include <stdlib.h>  // 包含标准库
#include <string.h>  // 包含字符串处理函数库
#include <unistd.h>  // 包含Unix标准函数库
#include <arpa/inet.h>  // 包含IP地址转换函数
#include <sys/socket.h>  // 包含套接字函数库
void error_handling(char* message); //错误处理函数

int main(int argc, char* argv[]) {
    //argv[0]是程序名，argv[1]和argv[2]是目标服务器端套接字（门卫socket）的ip和端口号
    int clnt_sock;
    struct sockaddr_in serv_addr; //地址信息
    char message[30]; //保存从服务器端读取（接收）过来的数据
    if (argc != 3) { // 如果参数数量不为3（程序名称也是一个参数，还需要服务器门卫socket的ip和端口号两个参数）
        printf("Usage: %s <IP> <port>\n", argv[0]);
        exit(1);  // 退出程序
    }
    //创建客户端socket
    clnt_sock = socket(PF_INET, SOCK_STREAM, 0);
    if (clnt_sock == -1) error_handling("socket() errror");
    //初始化目标服务器套接字（门卫socket）的地址信息信息
    //不用初始化客户端套接字的地址信息，因为它们在之后调用connect()时会自动分配
    memset(&serv_addr, 0, sizeof(serv_addr)); //初始化serv_addr结构体为0
    serv_addr.sin_family = AF_INET; // 设置地址族
    serv_addr.sin_addr.s_addr = inet_addr(argv[1]); //设置IP
    serv_addr.sin_port = htons(atoi(argv[2])); //设置端口
    //向服务器端发送连接请求，同时connect()自动会给客户端socket分配ip和端口号
    if (connect(clnt_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1) error_handling("connect() error");
    //完成连接后，从与之对应的服务器连接socket中读取（接收）数据
    int i;
    for (i = 0; i < 100; ++i) printf("Wait time %d \n", i); 
    //由于服务器端是多次调用write()传输数据，客户端调用一次read()读取，而TCP套接字传输的数据是不存在数据边界的，因此客户端需延迟调用read()等待服务器端传输所有数据
    //这种让CPU执行多余认为以延迟代码运行的方式称为"Busy Waiting",使用得当就能成功延迟客户端read()函数的调用
    read(clnt_sock, &message, sizeof(message));
    printf("Message from server: %s \n", message); //打印读取（接收）的数据
    close(clnt_sock);
    return 0;
}
void error_handling(char* message) {
    perror(message);  // 打印错误信息
    exit(1);  // 退出程序
}
```

#### 运行与结果显示：

```c++
# 服务器端
gcc tcp_serv_many_write.c -o serv_many_wr
./serv_many_wr 9190  # 正常情况下程序将停留在此状态，因为服务器端调用的accept()还未返回，接下来运行客户端
# 客户端
gcc tcp_clnt_one_read.c -o clnt_one_rd
./clnt_one_rd 127.0.0.1 9190
# 下面是客户端显示的结果
Wait time 0 
Wait time 1 
····················
Wait time 99
Message from server: hello world! 
```

## ch04 TCP套接字



## ch05 UDP套接字

源自本章作业7.请参考本章给出的uecho_sev.c和uecho_clnt.c，编写示例使服务器端和客户端轮流收发消息。收发的消息均要输出到控制台窗口。

> 感觉有点像聊天室的雏形了？

#### `uecho_serv2.c` 

```c++
#include <stdio.h>     
#include <stdlib.h>   
#include <string.h>     
#include <unistd.h>    
#include <arpa/inet.h>
#include <sys/socket.h> 
#define BUF_SIZE 30
void error_handling(char* message); 

int main(int argc, char* argv[]) {
    int  serv_sock;
    char message[BUF_SIZE];
    struct sockaddr_in serv_adr, clnt_adr; 
    socklen_t clnt_adr_sz; 
    
    if(argc != 2) { 
        printf("Usage : %s <port>\n", argv[0]);
        exit(1);
    }   
    
    // Creates a UDP socket.
    serv_sock = socket(PF_INET, SOCK_DGRAM, 0); 
    if(serv_sock == -1) error_handling("UDP socket() creation error");
   
    //Initialize the server address structure 
    memset(&serv_adr, 0, sizeof(serv_adr)); 
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_adr.sin_port = htons(atoi(argv[1])); 
   
    int on = 1;
    if (setsockopt(serv_sock, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(int)) < 0)  error_handling("setsockopt() error");
    
    if(bind(serv_sock, (struct sockaddr*) &serv_adr, sizeof(serv_adr)) == -1) error_handling("bind() error"); 
     
    int str_len;
    clnt_adr_sz = sizeof(clnt_adr); //initialize clnt_adr_sz
    while (1) {
        //不限制数据传输对象，因此第一个参数不是clnt_sock而是serv_sock
        str_len = recvfrom(serv_sock, message, BUF_SIZE, 0, (struct sockaddr*)&clnt_adr, &clnt_adr_sz);
        message[str_len] = 0;
        printf("Message from clnt: %s", message);
        
        //通过recvfrom的调用同时获取数据传输端的地址，正是利用该地址将接收的数据逆向传输
        fputs("Insert message(Q to quit): ", stdout);
        fgets(message, sizeof(message), stdin);

        if (!strcmp(message, "q\n") || !strcmp(message, "Q\n")) break;

        sendto(serv_sock, message, strlen(message), 0, (struct sockaddr*)&clnt_adr, clnt_adr_sz);
    }
    close(serv_sock); 
    return 0;
}
void error_handling(char* message) {
    perror(message);  
    exit(1);  
}
```

#### `uecho_clnt.c` 不需要变

```c++
#include <stdio.h>     
#include <stdlib.h>   
#include <string.h>     
#include <unistd.h>    
#include <arpa/inet.h>
#include <sys/socket.h> 
#define BUF_SIZE 30
void error_handling(char* message); 

int main(int argc, char* argv[]) {
    int clnt_sock;
    char message[BUF_SIZE];
    struct sockaddr_in serv_adr, from_adr;
    socklen_t src_adr_sz;//接收数据时，数据发送端的socket地址的长度
    if(argc != 3) { 
        printf("Usage : %s <IP> <port>\n", argv[0]);
        exit(1);
    }   

    clnt_sock = socket(PF_INET, SOCK_DGRAM, 0); 
    if(clnt_sock == -1) error_handling("socket() error");

    memset(&serv_adr, 0, sizeof(serv_adr)); 
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_addr.s_addr = inet_addr(argv[1]); 
    serv_adr.sin_port = htons(atoi(argv[2])); 
    
    int str_len;
    while (1) {
        fputs("Insert message(Q to quit): ", stdout);
        fgets(message, sizeof(message), stdin);
        //communication is terminated when the user enters "q" or "Q"
        if (!strcmp(message, "q\n") || !strcmp(message, "Q\n")) break;
        // Send the message to the server
        sendto(clnt_sock, message, strlen(message), 0, (struct sockaddr*)&serv_adr, sizeof(serv_adr));
        // Receive a response from src
        src_adr_sz = sizeof(src_adr);
        str_len = recvfrom(clnt_sock, message, BUF_SIZE , 0, (struct sockaddr*)&src_adr, &src_adr_sz));
        message[str_len] = 0;
        printf("Message from server: %s", message);
    }
    close(clnt_sock);
    return 0;
}
void error_handling(char* message) {
    perror(message);  
    exit(1); 
}
```

# 遇到的报错



## 1. 服务器端`bind() error: Address already in use`

### 原因
close之后会有一个WAIT_TIME状态，使得该ip和端口号仍被占用，不能立即重用。
### 解决方法
#### 1. 杀死之前的程序，再重新启动。

1. `lsof -i:端口号`或`netstat -nap`或`ps -aux | grep 程序名`查看PID进程名

	（如果没有 lsof 就用`yum install lsof`安装）

2. 杀死进程 `kill -9 你要结束的PID进程号` （参数-9是强制杀死的意思）

> 但是这种方法有很大的局限性，每次都需要你手动杀掉进程重启

#### 2. 在TCP服务器端套接字调用bind之前设置SO_REUSEADDR选项实现端口复用.
通过setsockopt()函数的选项SO_REUSEADDR进行设置
```cpp
	int on = 1;
	if(setsockopt(serv_sock, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(int)) < 0)  error_handling("setsockopt() error");
```

## 2. 客户端`connect() error: Invalid argument`

自己代码写错了。。
- 一个是引号位置：`printf("Usage: %s <port>\n", argv[0])`写成了`printf("Usage: %s <port>\n, argv[0]")`
- 一个是括号位置：`if(connect(clnt_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1) `写成了`if(connect(clnt_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr) == -1)) `

# `errno`

`errno`是全局变量

### 根据`errno`变量查看具体出错原因

1. 直接输出`errno`，根据输出的错误码自行Google搜索解决方案

2. 直接翻man手册
3. 借助`strerror()`或`perror()`函数，得到相应的文字说明。

#### strerror函数

- 函数原型：`char * strerror(int errno)`

- 使用方法：`fputs(stderr, "%s", strerror(errno));`

- ```c++
    void error_handling(char* message) {
    	fputs(message, stderr);
            fputc('\n', stderr);
    	exit(1);
    }
    ```

#### perror函数

- 函数原型：`void perror(const char *s)`

- 使用说明：参数s指定的字符串是要先打印出来的信息，可以由我么自己定义，**系统会在s字符串后加上错误原因的字符串。**

- ```c++
    void error_handling(char* message) {
    	perror(message);//比如显示"bind() error: Address already in use"
    	exit(1);
    }
    ```

### errno的线程/进程安全性

errno是全局变量，如果在多线程或者进程编程中，子线程想要查看errno确定自己的错误类型，可惜该errno被另一个线程更改了，这就出问题了。errno的线程/进程安全性指的就是这点，在多线程和多进程编程中，errno会不会变为线程或进程的私有变量？

## 复习一下C语言

## 输入输出

### `scanf()` 

```c++
char opMsg[BUF_SIZE]; 
scanf("%d", (int*)&opMsg[i * OPSZ + 1]); //将用户输入的操作数读到opMsg中
//scanf 函数中 %d 格式说明符 要求传递一个指向整数的指针，以便将输入的整数值存储到该指针指向的内存位置。由于 opMsg 是一个字符数组，而 scanf 需要整数类型的地址，因此需要类型转换。(int*)将指向char型的指针强转为指向int型的指针。也就是说，它将字符数组中特定位置的地址解释为一个整数类型的地址。
```

### `feof()`

检测流上的文件结束符，如果文件结束，则返回非0值，否则返回0，文件结束符只能被clearerr()清除。

## 文件操作

文件操作方式：

- 文本模式

- 二进制模式

> **`fread / fwrite` 函数 既可以操作 二进制文件 , 又可以操作 文本文件 ;**
>
> `getc / putc `函数 , `fscanf / fprintf `函数 , `fgets / fputs` 函数 , 只能用于操作 文本文件 ;

### 操作文件

读写单个字符：读`fgetc()`，写`fputc()`

读写字符串：读`fgets()`，写`fputs()`

格式化读写：读`fscanf()`，写`fprintf()`

读写······：`fread()` ，`fwrite()` 

```c++
fgetc(stdin); //读取并丢弃标准输入缓冲区中的一个字符。这通常是为了消耗掉用户输入中的换行符或其他潜在的剩余字符。
```



`fgets()`每次调用只会读取一行，因此要读取多行需要循环。



`fputs` 和 `scanf` 的区别：

1. **`fputs`**：
    - `fputs` 用于将字符串写入文件流或标准输出。
    - 它的原型是 `int fputs(const char *str, FILE *stream)`。
2. **`scanf`**：
    - `scanf` 用于从标准输入（或其他输入流）中读取格式化输入。
    - 它的原型是 `int scanf(const char *format, ...)`。



`fwrite()` 

函数原型 :

`size_t fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream);`

参数说明 :

- const void *ptr : 指针指向要写出数据的内存首地址 ;
- size_t size : 要写出数据的 基本单元 的字节大小（单位大小）；（比如sizeof(char)）
- size_t nmemb : 要写出数据的 基本单元 的个数 ;
- FILE *stream : 打开的文件指针 ;

返回值说明 : size_t 返回值返回的是实际写出到文件的 基本单元 个数 ;

### 打开关闭文件

`fopen()`、`fclose()`

### 指针

#### `**` 

当使用 `**` 时，通常涉及到**二维数组或指针的指针**，字符串集合就属于二维数组（其中每一维是一个字符串（字符数组））。

使用 `**` 的情况通常发生在处理动态分配的多维数组时，其中每个元素是指向其他数组的指针。在这个例子中，`matrix` 包含指向每行（一行是一个字符数组）的指针。

```c++
#include <stdio.h>
#include <stdlib.h>

int main() {
    // 定义行和列的数量
    int rows = 3;
    int cols = 5;

    // 分配内存以存储二维字符数组
    char **matrix = (char **)malloc(rows * sizeof(char *));
    //matrix 是一个指向指针的指针，用于存储一个二维字符数组
    
    // 分配内存以存储每行的字符数组
    for (int i = 0; i < rows; ++i) matrix[i] = (char *)malloc(cols * sizeof(char));
  
    // 初始化二维字符数组
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) matrix[i][j] = 'A' + i * cols + j;
    }

    // 访问并打印二维字符数组的内容
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            printf("%c ", matrix[i][j]);
        }
        printf("\n");
    }

    // 释放分配的内存
    for (int i = 0; i < rows; ++i) {
        free(matrix[i]);
    }
    free(matrix);

    return 0;
}
```

