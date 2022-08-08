//
// Created by 吴婷 on 2019-12-02.
//

#ifndef COMMON_VEP_LOGGER_H
#define COMMON_VEP_LOGGER_H

/**
 * vep日志
 */
class VepLogger{
public:
    VepLogger(){}
    virtual ~VepLogger(){}

    virtual void LogInfo(const std::string &msg) {}
    virtual void LogError(const std::string &msg) {}
    virtual void LogWarn(const std::string &msg) {}
    virtual void LogDebug(const std::string &msg) {}
};

#endif //COMMON_VEP_LOGGER_H
