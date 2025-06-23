终端输入命令：
```shell
sudo raspi-config     
```
选择`Interface Opctions` -> `VNC` -> 启动

下载VNC客户端：[https://www.realvnc.com/en/connect/download/viewer/](https://www.realvnc.com/en/connect/download/viewer/)
输入IP地址-> 登录即可


**vscode连接树莓派远程开发**
首先要确认树莓派有没有安装`Openssh-server`，如果没有需要先安装
```shell
# 查看是否安装
ssh 用户名

# 重启 
sudo systemctl enable ssh
sudo systemctl start ssh
```

在vscode 安装扩展`Remote - SSH` -> 输入IP地址 -> 连接即可



