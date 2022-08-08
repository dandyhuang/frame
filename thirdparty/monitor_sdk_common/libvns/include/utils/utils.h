//
// Created by 吴婷 on 2020-06-16.
//

#ifndef COMMON_LIBVNS_VNSUTILS_H
#define COMMON_LIBVNS_VNSUTILS_H

#include <map>
#include <vector>
#include <string>

class VnsTimeUtils{
public:
    static long GetCurrentTimeMills();      //毫秒级
    static long GetCurrentTimeMicros();     //微秒级
};

class VnsFileUtils{
public:
    /**
     * 获取文件最后一次修改时间（秒）
     * @param full_filename 完整文件名
     * @return
     */
    static long GetFileLastModifyTime(const std::string &full_filename);

    /**
     * 判断目录是否存在（可创建多级目录）
     * 存在，直接返回。不存在，新建目录，新建成功就返回
     * @param dir
     * @return
     */
    static bool AccessDirExist(const std::string &dir);

private:
    /**
     * 删除最后一级目录
     * @param dir
     * @return 删除后的前面的目录路径
     */
    static std::string RemoveLastPathDir(const std::string &dir);
};

class NetUtils
{
public:
    static std::string localIP();  // 获取本地ip
};

class VnsStrUtils{
public:
    static std::string BoolToString(bool val);
    static std::string mapToKeyValueStr(const std::map<std::string, std::string> &param_map);    //将参数值转换为k-v字符串，比如key1=val1&key2=val2&key3=val3

};

class NamingUtils{
public:
    static std::string getGroupedName(const std::string &service_name, const std::string &group_name);
};

#endif //COMMON_LIBVNS_VNSUTILS_H
