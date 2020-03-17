# TAlib

#### 介绍


#### 运行DEMO

```bash
git clone https://github.com/trialley/TAlib.git
# 或者git clone https://gitee.com/trialley/TAlib.git

cd TAlib
mkdir build
cd build
cmake ..
make
../WebServer
```

#### 支持特性

1. 异步日志系统
2. TCP协议封装
3. HTTP协议封装
4. 内存池
5. 线程池
6. Reactor模式


#### TODO
- [ ] 重新添加内存池
- [ ] 进行内存池性能测试
- [ ] 封装UDP协议
- [ ] 简化日志系统
- [ ] 封装WebSocket协议
- [ ] 内存池
- [ ] 数据库连接池
- [ ] 数据库接口
- [ ] 即时通讯例子
<!-- - [ ] 封装WebSocket协议 -->
<!-- - [ ] 封装WebSocket协议 -->
<!-- - [ ] 封装WebSocket协议 -->



#### 参与贡献
1.  Fork 本仓库
2.  新建 Feat_xxx 分支
3.  提交代码
4.  新建 Pull Request

邮箱：lg139@139.com


#### 开发日志

- 2020/03/09 0.1 经过多次推翻重来，本网络库终于初步成型啦
- 2020/03/16 0.2 加深对Reactor的理解，代码重构