# curl

`curl` 是一个强大的命令行工具，用于传输数据，支持多种协议，包括 HTTP、HTTPS、FTP 等。它常用于下载文件、测试API接口、上传数据等。
以下是 `curl` 的一些常见用法：

### 1. 下载文件

- **基本下载**: 使用 `curl [URL]` 命令下载文件。默认情况下，`curl` 将下载的内容输出到标准输出（屏幕）。
  ```bash
  curl http://example.com/file.txt
  ```

- **保存文件**: 使用 `-o` 或 `--output` 选项将下载的内容保存到文件。
  ```bash
  curl -o savedfile.txt http://example.com/file.txt
  ```

### 2. 发送请求

- **GET 请求**: `curl` 默认执行 GET 请求。
  ```bash
  curl http://example.com
  ```

- **POST 请求**: 使用 `-X POST` 发送 POST 请求。可用 `-d` 或 `--data` 发送数据。
  ```bash
  curl -X POST -d "param1=value1&param2=value2" http://example.com/post
  ```

- **上传文件**: 使用 `-F` 上传文件。
  ```bash
  curl -F "file=@/path/to/local/file" http://example.com/upload
  ```

### 3. 自定义请求头

- 使用 `-H` 或 `--header` 添加自定义头部。
  ```bash
  curl -H "Content-Type: application/json" -H "Authorization: Bearer token" http://example.com
  ```

### 4. 跟踪重定向

- 使用 `-L` 或 `--location` 跟随重定向。
  ```bash
  curl -L http://example.com
  ```

### 5. 使用代理

- 使用 `-x` 或 `--proxy` 通过指定的代理服务器进行请求。
  ```bash
  curl -x http://proxyserver:port http://example.com
  ```

### 6. 身份验证

- 使用 `-u` 或 `--user` 进行身份验证。
  ```bash
  curl -u username:password http://example.com
  ```

### 7. 限制速率和时间

- 使用 `--limit-rate` 限制下载速度。
  ```bash
  curl --limit-rate 100k http://example.com/file.txt
  ```

- 使用 `-m` 或 `--max-time` 设置最大传输时间。
  ```bash
  curl -m 60 http://example.com
  ```

### 8. 使用证书

- 使用 `-E` 或 `--cert` 使用证书进行 HTTPS 请求。
  ```bash
  curl -E /path/to/cert.pem https://example.com
  ```

### 9. 显示请求信息

- 使用 `-v` 或 `--verbose` 显示请求和响应的详细信息。
  ```bash
  curl -v http://example.com
  ```

### 10. 使用 Cookie

- 使用 `-b` 或 `--cookie` 发送 Cookie。
  ```bash
  curl -b "name=value" http://example.com
  ```

`curl` 的用法非常灵活，这些只是一些常见的用法。你可以通过 `man curl` 或 `curl --help` 查看更多的选项和用法。