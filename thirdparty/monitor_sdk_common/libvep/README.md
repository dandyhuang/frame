## Vep实验平台C++ SDK

当前C++版本sdk支持V1.0（场景）和V2.1版本（流量位、实验层、launch层），同时支持最新的V2.2版本（圈定字段）。
---
提示：

1.确认清楚创建实验的环境：测试环境/开发环境。
务必选择正确的环境，否则会查询不到对应的实验数据。

2.用户在调用的时候，要确认好自己实验的类型，选择对应的查询方法，否则会找不到实验，拉取数据失败。
同时也提供统一的api通用接口，详细使用见链接内容。


### 使用前必读：
"C++版本vep-sdk使用介绍"（详细）
<http://km.vivo.xyz/pages/viewpage.action?pageId=133038872>

目录：
```
注意：
1.C++在参数返回的时候，以三元组tuple字符串返回，返回值带有类型
2.开发环境的启动参数必须配置
3.日志类
4.查询返回结果类字段具体说明
    V1.0返回字段说明：【结果类-VepLabParamVo】
    V2.0返回字段说明：【结果类-VepLabHitInfo】
5.不同api选择说明 & 通用接口api
6.V2初始化支持同时传入多个layer_code

一、vep-sdk 1.0 接入手册
例子：vep_client_test.cpp
1. sdk业务
2. 使用

二、vep-sdk 2.0 接入手册
例子：vep_thirdparty_client_test.cpp
2.1  第三方流量位+实验层 sdk业务
2.2 第三方流量位+实验层 sdk使用

例子：vep_scene_client_test.cpp
2.3  场景+实验层 sdk业务
2.4 场景+实验层 sdk使用

```

### 1. 接口介绍
本文提供3个客户端

| 客户端 | 含义 |
|---|---|
| vep_client | V1.0版本 |
| vep_scene_client | V2+版本 场景+实验层|
| vep_thirdparty_client | V2+版本 第三方流量位+实验层|

具体使用可以参数对应的client_test文件，或者查看上面的"C++版本vep-sdk使用介绍"。


### 2. 查询返回结果
返回结果在result文件夹中，其中resultVo是表示查询返回结果成功与否。
vep_lab_hitInfo和vep_lab_paramVo是具体返回参数。

| 版本 | 适用客户端 | 结果类 |
|---|---|---|
| V1.0版本 | vep_client | vep_lab_paramVo |
| V2+版本 | vep_scene_client 或 vep_thirdparty_client| vep_lab_hitInfo |

各个返回参数的详细介绍，见"C++版本vep-sdk使用介绍"中的"abValue()查询返回结果说明"。

### 3. 注意事项
- libvep引用了thirdparty里面的curl，如果业务在使用libvep存在二次依赖的情况（一个BUILD引用了一次libvep，另一个BUILD又引用了这个BUILD），可能会出现编译失败的情况。
建议若存在二次依赖的情况，在二次传递的BUILD里面增加相应层级的incs引用curl。