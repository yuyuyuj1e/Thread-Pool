# Thread-Pool

## 一、线程池模块
1. 多种工作模式
   - ```FIXED_THREAD```: 线程数量固定 (线程池开始时给定的参数，但是不能超过超过硬件支持的数量，超过则改为硬件支持的数量) —— 不会随任务多少而改变。
   - ```MUTABLE_THREAD```: 线程数量可变 (线程池开始时给定的参数作为下限，其二倍作为上限；但是不能超过超过硬件支持的数量，超过则改为硬件支持的数量；如果上下限全部超过，则行为等同于 ```FIXED_THREAD``` 模式) —— 当任务数量超过线程数量时，增加线程 (不超过线程上限)；当线程超过一定时间无法接到任务时，释放线程 (不低于线程下线)。
2. 可接受任意返回类型和任意参数的任务函数，可以将有返回值有参函数转换为无返回值无参函数
3. 提交的任务存储在任务队列中，并由线程池进行管理
4. 提交任务时，可以在提交任务的函数第一个参数设置任务优先级，也可以不设置任务优先级使用线程池默认的任务优先级
5. 线程池相关配置存放在 `threadpool.json` 文件中

## 二、工作线程模块
1. 是线程池类的内部类，可当作友元类，直接使用线程池类的私有成员
2. 不断尝试从线程池持有的任务队列中取任务，并执行
3. 如果线程池是 ```MUTABLE_THREAD``` 模式，当线程超过一定时长没有接到新任务会自动退出，直到线程下限
  
## 三、任务队列模块
1. 由 `vector` 实现的最小堆结构，充当任务优先级队列
2. 优先执行优先级最高的(优先级数值最小)任务

## 四、日志模块
1. 单例模式
2. 异步写入日志内容
3. 当文档超过一定大小会自动进行备份
4. 当关闭日志模块或者文档备份后才会关闭日志文件流，避免频繁打开关闭文件流
5. 日志相关配置放在 `log.json` 文件中

## 五、构建及运行
1. 构建 ```bash build.sh```
2. 运行 ```bash run.sh```

## 六、项目结构
``` bash
├── bin
│   ├── libjsoncpp.so
│   ├── libthreadpool.a
│   ├── libthreadpool.so
│   ├── normal_test
│   └── shared_test
├── build.sh
├── CMakeLists.txt
├── conf
│   ├── log.json
│   └── threadpool.json
├── include
│   ├── CppLog.h
│   ├── HeapSafeQueue.h
│   ├── SafeQueue.h
│   └── ThreadPool.h
├── lib
│   └── json
│       ├── allocator.h
│       ├── assertions.h
│       ├── config.h
│       ├── forwards.h
│       ├── json_features.h
│       ├── json.h
│       ├── reader.h
│       ├── value.h
│       ├── version.h
│       └── writer.h
├── LICENSE
├── Log
│   └── log.txt
├── README.md
├── run.sh
├── src
│   ├── CppLog.cpp
│   ├── HeapSafeQueue.cpp
│   ├── ThreadPool.cpp
│   └── Worker.cpp
└── test
    ├── CMakeLists.txt
    └── test.cpp
```