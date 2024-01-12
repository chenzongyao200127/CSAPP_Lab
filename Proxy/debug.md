~~~shell
curl -v --proxy http://localhost:15213 http://localhost:8080/home.html
~~~

1. `curl`：这是一个常用的命令行工具，用于发送或接收数据，并支持多种协议，包括 HTTP、HTTPS、FTP 等。
2. `-v`：这是一个选项，表示“verbose”，用于输出详细的调试信息。包括发送的请求头、接收的响应头以及其他通信细节。
3. `--proxy http://localhost:15213`：这指定了代理服务器的地址和端口。在这个例子中，它告诉 `curl` 通过运行在本地计算机上、监听15213端口的HTTP代理服务器发送请求。
4. `http://localhost:8080/home.html`：这是 `curl` 请求的目标URL。在此示例中，它是请求本地计算机上8080端口的Web服务器上的 `home.html` 页面。

czy@czy-ThinkCentre:pts/6-> /home » czy » new_space » CSAPP_Lab » Proxy » proxylab-handout (139)
> ./proxy 15213
Accepted connection from (localhost, 40054)
[1]    361473 segmentation fault (core dumped)  ./proxy 15213


# 不适用代理时
Client:
~~~shell
(base) ╭─czy@czy-ThinkCentre in ~/new_space/CSAPP_Lab on master ✔ (origin/master +1)
╰$ curl -v http://localhost:8080/home.html 
*   Trying 127.0.0.1:8080...
* Connected to localhost (127.0.0.1) port 8080 (#0)
> GET /home.html HTTP/1.1
> Host: localhost:8080
> User-Agent: curl/7.81.0
> Accept: */*
> 
* Mark bundle as not supporting multiuse
* HTTP 1.0, assume close after body
< HTTP/1.0 200 OK
< Server: Tiny Web Server
< Content-length: 120
< Content-type: text/html
< 
<html>
<head><title>test</title></head>
<body> 
<img align="middle" src="godzilla.gif">
Dave O'Hallaron
</body>
</html>
* Closing connection 0
~~~

Server:
~~~shell
(base) [10.208.28.31] czy@czy-ThinkCentre:~/new_space/CSAPP_Lab/Proxy/proxylab-handout/tiny (master) $ ./tiny 8080                 130
Accepted connection from (localhost, 58302)
GET /home.html HTTP/1.1
Host: localhost:8080
User-Agent: curl/7.81.0
Accept: */*
~~~