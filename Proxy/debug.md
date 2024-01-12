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