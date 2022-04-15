# jyyOS2022

2022南大jyy操作系统设计与实现课程相关代码

## M1 pstree

[[M1]pstree实验要求](http://jyywiki.cn/OS/2022/labs/M1)

实现仿pstree命令行工具的效果

- -p, --show-pids: 打印每个进程的进程号。
- -n --numeric-sort: 按照pid的数值从小到大顺序输出一个进程的直接孩子。
- -V --version: 打印版本信息。

其实没有要求输出严格的树形结构，只需要展现出层级划分就可以了，不过这里还是做了树形结构输出的尝试。

效果展示如下：

```bash
qizidog@ubuntu-m1:~/jyyOS2022/M1$ gcc pstree.c
qizidog@ubuntu-m1:~/jyyOS2022/M1$ ls
a.out  pstree.c  strutils.c
qizidog@ubuntu-m1:~/jyyOS2022/M1$ ./a.out -p
systemd(1)─┬─systemd-journal(380)
           ├─lvmetad(397)
           ├─auditd(568)───audispd(29877)───sedispatch(29878)
           ├─polkitd(655)
           ├─iscsid(1092)
           ├─tuned(1095)
           ├─containerd(1109)
           ├─rhsmcertd(1110)
           ├─sshd(1324)───sshd(17354)───sshd(17358)───bash(17365)───bash(17407)
           ├─atd(1379)
           ├─pure-ftpd(1438)
           ├─jsvc(1459)───jsvc(1461)
           ├─agetty(1475)
           ├─agetty(1476)
           ├─redis-server(1504)
           ├─master(1727)─┬─qmgr(1766)
           │              └─pickup(21332)
           ├─dockerd(1824)─┬─docker-proxy(28428)
           │               └─docker-proxy(28434)
           ├─tat_agent(1846)
           ├─sgagent(11703)
           ├─barad_agent(19123)─┬─barad_agent(19130)
           │                    └─barad_agent(19131)
           ├─YDService(20488)───sh(20717)
           ├─YDLive(20510)
           ├─sh(21034)───node(21046)─┬─node(21083)─┬─bash(3305)
           │                         │             ├─bash(26111)
           │                         │             └─bash(27682)───pstree-64(2790)
           │                         ├─node(21386)─┬─node(21479)
           │                         │             ├─cpptools(21678)───cpptools-srv(24740)
           │                         │             └─node(22620)
           │                         ├─node(21398)
           │                         ├─node(27559)───cpptools(29464)───cpptools-srv(2307)
           │                         └─node(27570)
           ├─vncserver_wrapp(24930)───sleep(2771)
           ├─Xvnc(24980)
           ├─xstartup(25010)───gnome-session-b(25011)─┬─ssh-agent(25133)
           │                                          ├─gnome-shell(25205)───ibus-daemon(25284)─┬─ibus-dconf(25288)
           │                                          │                                         ├─ibus-engine-sim(25806)
           │                                          │                                         └─ibus-engine-lib(27737)
           │                                          ├─gsd-power(25449)
           │                                          ├─gsd-print-notif(25450)
           │                                          ├─gsd-rfkill(25452)
           │                                          ├─gsd-screensaver(25457)
           │                                          ├─tracker-extract(25619)
           │                                          ├─gsd-disk-utilit(25623)
           │                                          ├─tracker-miner-u(25662)
           │                                          └─tracker-miner-f(25687)
           ├─dbus-launch(25023)
           ├─dbus-daemon(25024)
           ├─imsettings-daem(25071)
           ├─gvfsd(25075)─┬─gvfsd-trash(25596)
           │              └─gvfsd-burn(25855)
           ├─gvfsd-fuse(25082)
           ├─at-spi-bus-laun(25157)───dbus-daemon(25162)
           ├─at-spi2-registr(25164)
           └─crond(31037)
```

⚠️当前的内存管理存在问题，一旦free就error，暂时没能排查出问题所在，搁置一下，以后处理。

