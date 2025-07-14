## 1. 往systemd 中添加开机自启动服务
这不是规范的做法， 参考下面帖子：
https://www.reddit.com/r/archlinux/comments/boglso/does_anyone_actually_use_systemd_user_services_to/?tl=zh-hans



## 2. 扩展
 商用终端通常这样做：
1. **使用嵌入式Linux发行版** (如Yocto, Buildroot)
2. **自定义init系统**，直接启动应用
3. **使用framebuffer直接渲染**，绕过X11


```shell
# 重载配置
sudo systemctl daemon-reload
# 重启服务
sudo systemctl restart kiosh.serivce
# 配合开机自启动
sudo systemctl enable kiosh.service
# 查看状态
sudo systemctl status kiosh.service
# 调试
sudo journalctl -u kiosh.service -f
```