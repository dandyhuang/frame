//
// Created by 吴婷 on 2020-06-17.
//

#ifndef COMMON_LIBVNS_DISK_CACHE_H
#define COMMON_LIBVNS_DISK_CACHE_H

#include <memory>
#include <unordered_map>
#include "object/service_info.h"

class DiskCache {
public:
    /**
     * 将单个服务信息，写入指定路径文件
     * @param dom
     * @param dir 目录
     */
    static bool write(std::shared_ptr<ServiceInfo> dom, const std::string &dir);

    /**
     * 从缓存地址中取出全部的服务信息
     * @param cache_dir 缓存目录地址，含多个服务信息文件
     * @param service_map 返回值
     * @return
     */
    static bool read(const std::string &cache_dir, std::unordered_map<std::string, std::shared_ptr<ServiceInfo>> &service_map);

private:
    /**
     * 只将服务的ip和port信息，写入指定ip路径文件
     * @param dom
     * @return
     */
    static bool writeIpPort(std::shared_ptr<ServiceInfo> dom);

private:
    static std::string _ip_cache_path;  // 仅储存ip信息的路径
};


#endif //COMMON_LIBVNS_DISK_CACHE_H
