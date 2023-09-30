# 1. summary

## 1.1. FlConc features

FlConc is an asynchronous RPC framework based on C++11,     
designed to be efficient and concise while maintaining extremely high performance.    
Its architecture pattern is based on the master-slave Reactor architecture,     
and the underlying layer uses epoll to implement IO multiplexing.    
The application layer is based on protobuf custom rpc communication protocol,     
and also supports simple HTTP protocol.

**The core idea of FlConc** is twofold:

1. Make building high-performance RPC services simple
2. Make asynchronous RPC calls simple

## 1.2. Protocol packets supported by FlConc

**The FlConc** framework currently supports two types of protocols:

1. Pure HTTP protocol: FlConc implements simple and basic encoding and decoding of the HTTP (1.1) protocol,
2. It is completely possible to build an RPC service using the HTTP protocol.
2. FlconcPB protocol: A custom protocol based on * * Protobuf * *, belonging to the binary protocol.

# 2. performance testing

FlConc uses the Reactor architecture at the bottom and combines multithreading, ensuring its performance.

The results of several simple performance tests are as follows:

## 2.1. HTTP echo testing QPS

Test machine configuration information: Centos **virtual machine**, memory **6G**, CPU **4 cores**

Testing tool: **wrk**: https://github.com/wg/wrk.git

Deployment information: WRK and FlConc services are deployed on the same virtual machine, and FlConc logs are closed

Test command:

```
//- c is the number of concurrent connections, modify them in order according to the table data
wrk -c 1000 -t 8 -d 30 --latency ' http://127.0.0.1:19999/qps?id=1 '
```

```
Test results:
|QPS                            | WRK 1000   | WRK 2000   | WRK  5000  | WRK concurrent 10000         
|IO thread count is 1           | 27000 QPS  | 26000 QPS  | 20000 QPS  | 20000 QPS
|IO threads are 4               | 140000 QPS | 130000 QPS | 123000 QPS | 118000 QPS
|IO thread count is 8           | 135000 QPS | 120000 QPS | 100000 QPS | 100000 QPS
|The number of IO threads is 16 | 125000 QPS | 127000 QPS | 123000 QPS | 118000 QPS 
```

```
//Test results for IO thread 4 and concurrent connection of 1000
[mach@localhost bin]$ wrk -c 1000 -t 8 -d 30 --latency 'http://127.0.0.1:19999/qps?id=1'
Running 30s test @ http://127.0.0.1:19999/qps?id=1
  8 threads and 1000 connections
  Thread Stats   Avg      Stdev     Max   +/- Stdev
    Latency     9.79ms   63.83ms   1.68s    99.24%
    Req/Sec    17.12k     8.83k   97.54k    72.61%
  Latency Distribution
     50%    4.37ms
     75%    7.99ms
     90%   11.65ms
     99%   27.13ms
  4042451 requests in 30.07s, 801.88MB read
  Socket errors: connect 0, read 0, write 0, timeout 205
Requests/sec: 134442.12
Transfer/sec:     26.67MB
```

Based on the above test results, the QPS of the FlConc framework can reach around 14W.

## 3 FlConc scaffolding (FlConc_generator)

FlConc provides a code generation tool that is simple enough to generate all framework code with just one protobuf file,
As a user, all you need to do is write business logic, without worrying about the principles of the framework, and
without having to worry about writing tedious repetitive code or considering how to link the FlConc library.
Next, use an example to illustrate how to use ` FlConc_generator `

### 3.1 Preparing the protobuf file

For example, we need to build an order service: ` order_server ` It provides some simple order operations: querying
orders,
generating orders, deleting orders, etc.
Firstly, define 'order_server. protocol' as follows:

```
syntax = "proto3";
option cc_generic_services = true;

message queryOrderDetailReq {
  int32 req_no = 1;         
  string order_id = 2;      
}

message queryOrderDetailRsp {
  int32 ret_code = 1;     
  string res_info = 2;    
  int32 req_no = 3; 
  string order_id = 4;      
  string goods_name = 5;   
  string user_name = 6;     
}

message makeOrderReq {
  int32 req_no = 1;
  string user = 2;
  string goods_name = 3;   
  string pay_amount = 4;   
}

message makeOrderRsp {
  int32 ret_code = 1;
  string res_info = 2;
  int32 req_no = 3;
  string order_id = 4;      
}

message deleteOrderReq {
  int32 req_no = 1;         
  string order_id = 2;      
}

message deleteOrderRsp {
  int32 ret_code = 1;
  string res_info = 2;
  int32 req_no = 3;
  string order_id = 4;      
}


service OrderService {

  //Query orders
  rpc query_order_detail(queryOrderDetailReq) returns (queryOrderDetailRsp);

  //Generate order
  rpc make_order(makeOrderReq) returns (makeOrderRsp);

  //Delete order
  rpc delete_order(deleteOrderReq) returns (deleteOrderRsp);

}
```

### 3.2 Generating FlConc Framework

This step is very simple, so simple that it only requires one line of command:

```
python3 flconc/generator/flconc_generator.py -o ./ -i order_server.proto -p 10001
```

其项目结构如下：

```
order_server:
  - bin
    - run.sh
    - shutdown.sh
    - order_server
  - conf
    - order_server.xml
  - log
  - obj
  - order_server
    - comm
    - interface
    - pb
    - service
      - order_server.cc
      - order_server.h
    - main.cc
    - makefile
  - test_client
```

Enter the 'order_server/order_server' directory, execute 'make j4', and the entire project is built.

Next, enter the 'order_server/bin' directory and execute:

```
bash run.sh order_server
```

At this point, the FlConc service has successfully started running.
Next, let's conduct a simple test by entering the 'order_server/test_client' directory and executing the client testing
tool, such as:

```
./test_query_order_detail
```

If the FlConc service starts successfully, you will see the following output:

```
[mach@localhost test_client]$ ./test_query_order_detail_client 
Send to FlConc server 0.0.0.0:10001, requeset body: 
Success get response frrom FlConc server 0.0.0.0:10001, response body: res_info: "OK"
```

Otherwise, you will see the specific reason for the failure, please troubleshoot according to the error code.
For example, the error displayed here is peer closed, which is likely due to the service not being started, causing no
one to listen on this port.

```
[mach@localhost test_client]$ ./test_query_order_detail_client 
Send to FlConc server 0.0.0.0:10001, requeset body: 
Failed to call FlConc server, error code: 10000000, error info: connect error, peer[ 0.0.0.0:10001 ] closed.
```

### 3.3 Business logic development

`FlConc_generator ` generates an interface for each rpc method in the Protobuf file,
These interfaces are located in the 'order_server/interface/' directory

For example, the 'test_query_order_detail' method here allows us to find these two files in the 'interface' directory:
`Query_order_detail. cc ` and ` query_order_detail. h`

```c++
// interface/query_order_detail.cc

#include
"FlConc/comm/log.h"
#include
"order_server/interface/query_order_detail.h"
#include
"order_server/pb/order_server.pb.h"

namespace order_server {

QueryOrderDetailInterface::QueryOrderDetailInterface(const::queryOrderDetailReq& request,::queryOrderDetailRsp& response)
: m_request(request),
m_response(response) {

// M request: The structure of the client request, 
// from which request information can be extracted

// M-response: The server-side response structure, 
// simply set the result here, and FlConc will be responsible for sending the result to the client
}

QueryOrderDetailInterface::~QueryOrderDetailInterface() {

}

void QueryOrderDetailInterface::run() {
//
// Run your business at here
// m_reponse.set_ret_code(0);
// m_reponse.set_res_info("OK");
//
}

```

So writing business logic is very simple, just implement the specific 'QueryOrderDetailInterface:: run()' method,
No other logic needs to be cared for at all, FlConc has already handled everything.

### 3.4 FlConc_generator Options Explanation

`Flconc_generator ` is a simple script implemented in Python language, which provides several simple command-line input
options,
You can also use the '- h' or '-- help' options to obtain help documents:

```
Options:
  -h, --help
      Print Help Documents
  -i xxx.proto, --input xxx.proto
      Specify the source protobuf file, note that only porotbuf3 is supported
  
  -o dir, --output dir
      Specify project generation path
  
  -p port, --input port
      Specify the port on which the FlConc service listens (default is 39999)
  
  -h x.x.x.x, --host x.x.x.x
      Specify the IP address bound to the FlConc service (default is 0.0.0.0)
```

# 4. Overview design

**The main modules of the FlConc** framework include asynchronous logging, coroutine encapsulation, Reactor
encapsulation, Tcp encapsulation, FlConcPB protocol encapsulation, HTTP protocol encapsulation, and RPC encapsulation
modules.

## 4.1. Asynchronous logging module

Started a thread responsible for printing logs.

**The log characteristics of FlConc**:

-**Asynchronous Logging**: Asynchronous printing of logs without blocking the current thread. Producers only need to
put the log information into the buffer, and the consumer thread will automatically synchronize the logs to the disk
file at a certain time frequency.

-**Log Level**: The log is printed in different levels. When the set level is higher than the level of the log to be
printed, log printing is an empty operation and does not consume any performance energy.

-**File Output**: Log support can be output to a file, especially in production environments, printing logs to the
console is not a good method.

-**Rolling Log**: The log files will scroll on their own. When **cross days** or **a single file exceeds a
certain size**, a new file will be automatically created to write log information.

-**Crash handling**: FlConc's log library handled * * sudden program crash**
In simple terms, it means synchronizing log information to disk files before the program crashes and exits. This is very
important, if the log content at the moment of the crash is missing, it will be difficult to identify the specific
cause.

-**Log Classification**: FlConc provides two types of logs: **RPC framework logs** and **APP application
logs**. RPC framework logs end with the rpc suffix and are FlConc
The log information printed by the framework during runtime is usually used to monitor the running status of the
framework itself. The APP application log ends with the suffix **app** and is specifically used to process user
requests. For each client request, the APP
The log will print out the requested msg as an identifier. Overall, if you are only using FlConc, simply follow the app
logs.

For example, macro definitions such as **DebugLog** and **AppDebugLog** can be used to print these two types of
logs separately:

```
DebugLog << "11";
AppDebugLog << "11";
```

## 4.2. Collaborative module

### 4.2.1. Collaborative packaging

The low-level switch of FlConc's coroutine uses Tencent's open-source coroutine
library [libco]（ https://github.com/Tencent/libco ）At the context switching point of the protocol,
In addition to protocol switching, FlConc provides hooks for some basic functions, such as read, write, connect, and so
on.

## 4.3 Reactor module

The Reactor module is the core module of the FlConc framework, which is an IO multiplexing module based on epoll
implementation,
Used to listen for readable, writable, and exception events for multiple file descriptors.

## 4.4. Tcp module

### 4.4.1. TcpServer

### 4.4.2. TcpConnection

## 4.5. FlconcPB protocol

FlconcPB is a lightweight protocol type customized by the Flconc framework, based on Google's protobuf

### 4.5.1. FlconcPB protocol message format decomposition

**The FxcPB** protocol packet message is described in c++pseudocode as follows:

```c++
/*
**   min of package is: 1 + 4 + 4 + 4 + 4 + 4 + 4 + 1 = 26 bytes
**
*/
Char start;                        //represents the beginning of the message, usually 0x02
int32_t pk_len { 0 };              //  The entire length of the package, in bytes
int32_t msg_req_len { 0 };         //  Msg-req string length
std::string msg_req;               // msg_req, Identify an RPC request or response. Generally speaking, the request and response use the same msg-req
int32_t service_name_len { 0 };    //  Servicename length
Std::string service_fullname;      //The complete rpc method name, such as QueryService.queryname
int32_t err_code { 0 };            //  Framework level error code 0 represents normal call, non 0 represents call failure
int32_t err_info_len { 0 };        //  Err_info length
Std::string err_info;              //Detailed error information, this field value will be set when err_code is not 0
Std::string pb_data;               //Business protobuf data, obtained by serializing Google's protobuf
int32_t check_num { 0 };           //  Package inspection, used to check if the package data is damaged
Char end;                          //represents the end of the message, usually 0x03
```

The annotation information is already very complete.
The other fields that require special explanation are as follows:

**Err_code**: err_code is a framework level error code,
It represents errors that occur during the RPC call process, such as peer closure, call timeout, etc. A err_code of 0
indicates that the RPC call is normal,
It means sending data normally and receiving a reply packet. A non zero value indicates a call failure, and err_info
will be set as detailed error information.

**Service_fullname**: refers to the complete method name of the call.
That is servicename.methodname.
Generally speaking, a **FlConcPB** protocol's **FlConc** service needs to register at least one **Service** (where
Service refers to inheriting Google:: protobuf::
The class of Service, and a Service contains multiple methods.

**Pk_len**: pk_len represents the length of the entire protocol packet, measured in 1 byte,
And it includes **[strat]** characters and **[end]** characters.

**The FlconcPB** protocol message contains multiple len fields, mainly to exchange space for time,
The receiver, knowing the length in advance, makes it more convenient to decode various fields, thereby improving the
efficiency of decoding.

In addition, all int type fields in the **FlconcPB** protocol will be converted to **network byte order** during
encoding!

## 4.6. Http module

The HTTP module of FlConc is actually an imitation of Java's Servlet concept,
Every HTTP request will find the corresponding HttpServlet object,
Execute its pre registered business logic function to process HTTP requests and receive HTTP responses.

# 5. Error code

## 5.1. Error code judgment specification

**The FlconcPB** protocol uses error codes to identify uncontrollable errors during RPC calls.
These error codes are framework level error codes, and when these error codes appear, it indicates that there is a
problem with the RPC call link. Naturally, this RPC call failed.
Generally speaking, when calling RPC, two error codes need to be determined, such as:

```c++
stub.query_name(&rpc_controller, &rpc_req, &rpc_res, NULL);
//Error code for judging framework level
if (rpc_controller.ErrorCode() != 0) {
  ErrorLog << "failed to call QueryServer rpc server";
  // ....
  return;
}
//Determine business error codes
if (rpc_res.ret_code() != 0) {
  // ...
  return;
}
```

Rpc_controller.ErrorCode is an RPC **framework level error code**, which refers to the lock description in this
document.
The enumeration values for this error code have been defined in the following table and will not change under normal
circumstances.
When this error code is not 0, please check if there are any issues with the RPC communication link and if there are any
abnormalities in the network connection.
Of course, the err_info field in the FlconcPB protocol will also provide a detailed description of error information.

Another error code is the **Business Error Code**, which is usually defined in the first field of the RPC method
return structure.
The occurrence of this error code is usually due to unexpected results during business processing on the other end,
At this point, the corresponding error code and error information will be returned. The enumeration value of this error
code should be agreed upon by both RPC communication parties.

## 5.2. Error code definition document

The detailed description of err_code is shown in the table below:

```
|Error Code|
|------------------------------|----------|-----------------------------------------|
|ERROR-PEER-CLOSED             | 10000000 | When connected, the peer is closed, usually because there is no process listening to this port on the peer         
|ERROR-FAILED CONNECT          | 10000001 | Connect failed|
|ERROR-FAILED_Get-REPLY        | 10000002 | RPC call did not receive packet data from the other end|
|ERROR-FAILED DESERIALIZE      | 10000003 | Deserialization failed, usually due to issues with pb_data in FlconcPB|
|ERROR-FAILED_SeriALIZE        | 10000004 | Serialization failed|
|ERROR-FAILED_ENCODE           | 10000005 | Encoding failed|
|ERROR-FAILED DECODE           | 10000006 | Decoding failed|
|ERRORRRP_CALL_TIMEOUT         | 10000007 | RPC call timeout. Please check if the RPC timeout is too short in this case|
|ERROR-SERVICE-NOT-FOUND       | 10000008 | Service does not exist, meaning the other party has not registered this service|
|ERROR-METHOD-NOT-FOUND        | 10000009 | Method does not exist, the other party does not have this method|
|ERROR-PARSE-SERVICE-NAME      | 10000010 | Parsing servicename failed|
|ERROR-NOT_SET-ASYNCPRE-CALL   | 10000011 | Object not saved before non blocking coroutine RPC call|
```

