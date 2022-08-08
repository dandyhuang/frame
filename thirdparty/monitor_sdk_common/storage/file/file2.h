// Copyright (c) 2015, Vivo Inc. All rights reserved.
/**
 * @file file.h
 * @brief
 * @author chenzheng05@baidu.com
 * @date 2015-01-20
 */
#ifndef COMMON_FILE_FILE_H
#define COMMON_FILE_FILE_H

#include <string.h>

#include <map>
#include <string>
#include <vector>
#include "thirdparty/monitor_sdk_common/base/class_register.h"
#include "thirdparty/monitor_sdk_common/base/closure2.h"
#include "thirdparty/monitor_sdk_common/storage/file/file_define.h"
#include "thirdparty/monitor_sdk_common/storage/file/file_fwd.h"

namespace common {

class File {
public:
    static const char kPathSeparator = '/';
    // 异步操作的默认超时时间（单位:s）
    static const uint32_t kDefaultAsyncTimeout = 10 * 60 * 60;

    /// @beirf       传给Open中的flag参数
    enum FILE_FLAG {
        kOpenModeRead = 0x01,
        kOpenModeWrite = 0x02,
        kOpenModeAppend = 0x04,
        kIoAsync = 0X1000,   ///< 不设置则是同步,设置则是异步

        // compatible old names
        ENUM_FILE_OPEN_MODE_R     = kOpenModeRead,
        ENUM_FILE_OPEN_MODE_W     = kOpenModeWrite,
        ENUM_FILE_OPEN_MODE_A     = kOpenModeAppend,
        ENUM_FILE_TYPE_RA_FILE    = 0x200,
        ENUM_FILE_IO_NON_BLOCKING = kIoAsync,
        ENUM_FILE_TYPE_CIPHER     = 0x400,
    };

    /// @beirf      初始化SDK模块,在进程中初始化调用一次,
    ///              不需要每个File对象都调用
    ///
    ///              不再传入identity,进行文件访问的时候会使用用户的某一个role,
    ///              默认为和用户名同名的一个role.
    ///              使用--identity=xxx或者当前的环境变量获得identity
    ///
    /// @return     true, 成功
    ///              false,失败
    static bool     Init();

    /// @beirf      打开一个文件
    /// @param      file_path   要打开的文件全路径,可以是本地文件或/xfs/网络文件
    ///                          可以下述形式附加选项.
    ///                          file_path:k1=v1:k2=v2
    ///                          选项具体由文件实现解释.
    ///                          当附加的选项与传入的options重复时，以此处为准.
    /// @param      flags       要打开文件的模式,包括打开普通文件或者记录型文件,
    ///                          读写权限，异步或同步读写
    /// @param      options     选项，包括是否使用客户端cache，副本数量等
    /// @param      error_code  用于返回出错代码
    /// @return     表示分配的文件指针; NULL 表示出错; 用户必须delete返回的指针以避免内存泄漏.
    ///
    /// @note       flags各个模式的意义：
    /// W:           如果不存在会建立文件;如果文件存在，都会把之前文件删除，
    ///              建立一个新的文件;文件指针在文件头;
    ///
    /// A:           如果文件存在,在文件尾追加;若不存在,则建立一个文件,
    ///              文件读写指针在最后;
    ///
    /// R:           只能打开已存在的文件,文件指针在文件头;
    /// R|W:         不允许;
    ///
    /// W|A:         不允许;
    /// R|A:         不允许;
    /// R|W|A:       不允许;
    static File*    Open(const std::string& file_path,
                         uint32_t flags,
                         const OpenFileOptions& options = OpenFileOptions(),
                         uint32_t* error_code = NULL);

    /// @beirf      打开一个文件另一接口, 不处理错误，要不成功
    ///              要不程序死掉（可根据需要选用）
    /// @param      file_path   要打开的文件全路径,可以是本地文件或/xfs/网络文件
    /// @param      flags       要打开文件的模式,包括打开普通文件或者记录型文件,
    ///                          读写权限，异步或同步读写
    /// @param      options     读写文件的选项
    /// @return     表示分配的文件指针; NULL 表示出错
    ///
    /// @note       flags各个模式的意义：
    /// W:           如果不存在会建立文件;如果文件存在，都会把之前文件删除，
    ///              建立一个新的文件;文件指针在文件头;
    ///
    /// A:           如果文件存在,在文件尾追加;若不存在,则建立一个文件,
    ///              文件读写指针在最后;
    ///
    /// R:           只能打开已存在的文件,文件指针在文件头;
    /// R|W:         不允许;
    ///
    /// W|A:         不允许;
    /// R|A:         不允许;
    /// R|W|A:       不允许;
    static File*    OpenOrDie(const std::string& file_path,
                              uint32_t flags,
                              const OpenFileOptions& options = OpenFileOptions());

    /// @beirf      打开一个文件，只能同步打开
    /// @param      file_path   要打开的文件全路径,可以是本地文件或/xfs/网络文件
    /// @param      mode        r,w,a方式打开
    /// @return     表示分配的文件指针; NULL 表示出错
    static File* Open(const std::string& file_path, const char* mode,
                      uint32_t* error_code = NULL);

    /// @beirf      同步从文件读取数据
    /// @param      buffer      表示存放数据的本地内存空间；
    /// @param      size        表示需要读取的最大数据长度；
    /// @param      error_code  存放错误信息；
    /// @return     -1 出错
    ///              0  读到了文件结尾
    ///              >0 实际读取到的文件长度
    virtual int64_t Read(void*     buffer,
                         int64_t   size,
                         uint32_t* error_code = NULL) = 0;

    /// @beirf      同步从文件读取数据
    /// @param      result      表示存放数据的string；
    /// @return     false 出错
    ///             true 成功
    static bool LoadToString(const std::string& file_path, std::string* result);

    /// @brief:      同步从文件读取一行数据, 最多max_size - 1个字节
    ///              以'\0'结束
    ///              '\n'作为行结束标志，并且'\n'不会被丢弃，会被读取到buffer中
    ///              如果函数返回值ret > 0, 那么buffer[ret] = '\0'；
    ///              如果0 < ret < max_size - 1， 那么buffer[ret - 1] = '\n'；
    ///              如果ret = max_size - 1， 那么buffer[ret - 1] = '\n'表明读取到一行的结尾，
    ///              否则表明buffer太小没有读取到一行的结尾
    /// @param      buffer      表示存放数据的本地内存空间；
    /// @param      max_size    表示需要读取的最大数据长度；
    /// @return     -1 出错
    ///              0  读到了文件结尾
    ///              >0 实际读取到的文件长度
    virtual int32_t ReadLine(void* buffer, int32_t max_size) = 0;

    int32_t ReadLine(std::string* result);

    /// @beirf      同步方式向打开的文件写入数据，文件通常缓存在本地,
    ///              调用Flush或Close时刷新到网络文件. 函数实现可能有重试，具体策略请参考各实现。
    ///
    /// @param      buffer      指向待写入的数据
    /// @param      size        待写入的文件长度
    /// @param      error_code  存放出错信息。返回成功时，实际写入的字节数等于buf_size。
    ///
    /// @return     >=0 实际成功写入的长度;
    ///              -1  失败
    virtual int64_t Write(const void* buffer,
                          int64_t     size,
                          uint32_t*   error_code = NULL) = 0;

    /// @beirf      从文件的指定位置异步读取数据
    /// @param      buffer          表示存放数据的本地内存空间；
    /// @param      size            表示需要读取的最大数据长度；
    /// @param      start_position  表示从该位置开始读取数据；
    /// @param      callback        与当前数据相对应的回调函数
    ///                              (void:表示回调函数的返回值,后两个是参数:
    ///                              第一个表示成功传输的长度;
    ///                              第二个为errorcode)
    ///
    /// @param      timeout         超时时间,seconds
    /// @param      error_code      保存错误信息
    /// @return     =0 成功
    ///              -1 失败
    virtual int32_t AsyncReadFrom(void* buffer,
                                  int64_t size,
                                  int64_t start_position,
                                  Closure<void, int64_t, uint32_t>* callback,
                                  uint32_t  timeout = kDefaultAsyncTimeout,
                                  uint32_t* error_code = NULL)=0;

    /// @beirf      异步方式向打开的文件写入数据，文件通常缓存在本地,
    ///              调用Flush或Close时刷新到网络文件
    ///
    /// @param      buffer      指向待写入的数据
    /// @param      size        待写入的文件长度
    /// @param      callback    数据写入成功后该回调函数将被调用
    ///                          (void:表示回调函数的返回值,后两个是参数:
    ///                          第一个表示成功传输的长度;
    ///                          第二个为errorcode)
    ///
    /// @param      timeout     超时时间,seconds
    /// @param      error_code  存放出错信息,
    ///
    /// @return     =0 成功(写入的长度)
    ///              -1 失败
    virtual int32_t AsyncWrite(const void* buffer,
                               int64_t     size,
                               Closure<void, int64_t, uint32_t>* callback,
                               uint32_t    timeout = kDefaultAsyncTimeout,
                               uint32_t*   error_code = NULL) = 0;

    /// @beirf      The file implementation support asynchronous operations?
    ///              For file implementation doesn't support aync operaton,
    ///                  the behavior of calling async operation is undefined.
    /// @return     return true if support async operations.
    virtual bool SupportAsync() = 0;

    /// @beirf      将本地数据刷新至网络服务器或磁盘
    /// @param      error_code  存放出错信息
    /// @return     0  成功
    ///              <0 失败
    virtual int32_t Flush(uint32_t* error_code = NULL) = 0;

    /// @beirf      关闭一个文件
    /// @param      error_code  存放出错信息
    /// @return     0 成功; -1 失败
    ///
    /// @note       关闭之前会主动调用Flush一次
    virtual int32_t Close(uint32_t* error_code = NULL) = 0;

    /// @beirf      拷贝文件数据,暂不支持含通配符的大量拷贝操作
    /// @param      src_file_path   目标源文件全路径
    /// @param      dest_file_path  目标文件全路径
    /// @param      error_code      保存出错信息
    /// @return     0 成功
    ///              -1 失败
    static int32_t  Copy(const std::string& src_file_path,
                         const std::string& dest_file_path,
                         uint32_t*      error_code = NULL);

    /// @beirf      移动文件或子目录
    /// @param      src_name    源文件或目录全路径
    /// @param      dest_name   目标目录全路径
    /// @param      error_code  保存出错信息
    /// @return     0 成功
    ///              -1 失败
    static int32_t  Move(const std::string& src_name,
                         const std::string& dest_name,
                         uint32_t*   error_code = NULL);

    /// @beirf      修改文件或子目录名字
    /// @param      old_path_name   旧的文件或目录全路径名
    /// @param      new_path_name   新的文件或目录名
    /// @param      error_code      保存出错信息
    /// @return     0 成功
    ///              -1 失败
    static int32_t  Rename(const std::string& old_path_name,
                           const std::string& new_path_name,
                           uint32_t*   error_code = NULL);

    /// @beirf      修改文件或子目录名字
    /// @param      old_path_name   旧的文件或目录全路径名
    /// @param      new_path_name   新的文件或目录名
    /// @param      rename_option   rename操作的选项
    /// @param      error_code      保存出错信息
    /// @return     0 成功
    ///              -1 失败
    static int32_t  Rename(const std::string& old_path_name,
                           const std::string& new_path_name,
                           const RenameOptions& rename_option,
                           uint32_t*   error_code = NULL);

    /// @beirf      删除文件或子目录
    /// @param      path_name       待删除的文件或目录全路径名,目录名以/结尾
    /// @param      is_recursive    是否递归删除其子文件和子目录,
    ///                              部分文件实现不支持.
    /// @param      error_code      保存出错信息
    /// @return     0 成功
    ///              -1 失败
    ///
    /// 备注：回收站功能：
    /// 在文件路径后面加上":recycle=true"，可以将文件放入回收站，而不是马上删除。
    /// 回收站内的文件会放到原文件相同目录下，加上'~'和时间戳以示区别。
    /// xfs会在7天之后删除回收站内的文件。
    /// 7天之内，用户可以用fu ls -y来看到回收站中的文件，并且可以用rename的方式恢复文件。
    /// 注意：回收站中的文件任然会计算至用户的quota.
    /// 如果你quota不足，请直接删除回收站内的文件以释放quota。
    ///
    static int32_t  Remove(const std::string& path_name,
                           bool        is_recursive = false,
                           uint32_t*   error_code = NULL);

    /// @beirf      添加子目录
    /// @param      path_name   目录全路径名,目录名以/结尾
    /// @param      error_code  保存出错信息
    /// @return     0 成功
    ///              -1 失败
    static int32_t  AddDir(const std::string& file_path, uint32_t* error_code = NULL);

    /// @beirf      递归添加子目录
    /// @param      path_name   目录全路径名,
    /// @param      error_code  保存出错信息
    /// @return     true 成功
    ///             false 失败
    /// Modify from xfs/tools/file_utility/make_dir.h
    static bool AddDirRecursively(const std::string& path, uint32_t* error_code = NULL);


    /// @beirf      查看指定目录或文件的属性信息
    /// @param      pattern     要查看的目录或文件，支持* ? [].
    /// @param      list_option 具体的含义参看ListOptions定义.
    /// @param      buffer      放返回的属性信息,应用层分配的缓冲区.
    ///                          某些字段可能由于获取信息失败而保留为其默认值.
    /// @param      error_code  保存出错信息
    /// @return     0 成功; -1 失败
    static int32_t List(const std::string& pattern,
                        const ListOptions& list_option,
                        std::vector<AttrsInfo>* attrs_info,
                        uint32_t* error_code = NULL);

    /// @beirf      查看指定目录或文件的属性信息
    /// @param      name        要查看的目录或文件，不支持* ? [].
    /// @param      mask        需要获取的属性信息掩码
    /// @param      attrs_info  返回的属性信息
    /// @param      error_code  保存出错信息
    /// @return     true 成功; false 失败
    static bool  GetAttrs(const std::string& name,
                          const AttrsMask& mask,
                          AttrsInfo* attrs_info,
                          uint32_t* error_code = NULL);

    /// @beirf      改变文件的当前偏移量
    /// @param      offset      相对于origin的偏移量
    /// @param      whence      偏移量的相对位置
    ///              可选参数:
    ///                  SEEK_SET    文件开始
    ///                  SEEK_CUR    文件的当前位置
    ///                  SEEK_END    文件结尾
    /// @param      error_code  返回出错信息
    /// @return     >=0 成功(返回当前文件的偏移量)
    ///              -1 出错
    virtual int64_t Seek(int64_t   offset,
                         int32_t   whence,
                         uint32_t* error_code = NULL) = 0;

    /// @beirf      查看文件或子目录占用的空间
    /// @param      path_name   查看的文件或目录全路径名
    /// @param      error_code  保存出错信息
    /// @return     >=0 成功(返回文件或子目录大小)
    ///              -1 失败
    static int64_t  Du(const std::string& path_name, uint32_t* error_code = NULL);

    /// @beirf      查看文件或子目录的汇总后的统计信息
    /// @param      path_name   查看的文件或目录全路径名
    /// @param      mask        in/out参数，传入想要获取的信息的掩码.
    ///                         运行后被设置成文件系统实际支持的信息的掩码.
    /// @param      summary     保存汇总的统计信息
    /// @param      error_code  保存出错信息
    /// @return     true 成功, false 失败
    static bool GetContentSummary(const std::string& path_name,
                                  FileContentSummaryMask* mask,
                                  FileContentSummary* summary,
                                  uint32_t* error_code = NULL);

    /// @beirf      返回当前文件的偏移量
    /// @param      error_code   保存出错信息
    /// @return     >=0 成功(返回当前文件的偏移量);-1 出错
    virtual int64_t Tell(uint32_t* error_code = NULL) = 0;

    /// @beirf      设置文件有效大小（截短）
    /// @param      length      文件有效大小
    /// @param      error_code  保存出错信息
    /// @return     >=0 成功
    ///              -1 出错
    virtual int32_t Truncate(uint64_t length, uint32_t* error_code = NULL) = 0;


    /// @beirf      获取数据所在的NodeServe位置
    /// @param      start       数据块的开始位置
    /// @param      end         数据块的结束位置
    /// @param      buffer      获取到的位置信息
    /// @param      error_code  保存出错代码
    /// @return     0 成功
    ///              -1 失败
    virtual int32_t LocateData(uint64_t      start,
                               uint64_t      end,
                               std::vector<DataLocation>* buffer,
                               uint32_t*     error_code = NULL) = 0;

    /// @beirf      获得文件大小
    /// @param      file_name   目录全路径名
    /// @param      error_code  保存出错信息
    /// @return     >=0 成功
    ///              -1 失败
    static int64_t  GetSize(const std::string& file_name,
                            uint32_t*   error_code = NULL);

    /// @beirf      获取指定目录下的文件序列
    /// @param      pattern      要查看的目录或文件，目录以/结尾
    /// @param      files        放返回的所有文件序列（不分页）.
    /// @return     0 成功; -1 失败
    static int32_t GetMatchingFiles(const std::string& pattern,
                                    std::vector<std::string>* files,
                                    uint32_t* error_code = NULL,
                                    const ListOptions* list_option = NULL);

    /// @beirf      查看指定文件或者目录是否存在
    /// @param      path         要查看的文件名全路径名，目录以/结尾
    /// @param      error_code   保存出错信息
    /// @return     true 成功(存在)
    ///              false 失败
    static bool IsExist(const std::string&, uint32_t* error_code = NULL);

    /// @beirf      查看指定文件或者目录是否存在
    /// @param      path         要查看的文件名全路径名
    /// @param      error_code   保存出错信息
    /// @return     true 成功(存在)
    ///              false 失败
    /// DEPRECATED_BY(IsExist)
    static bool CheckExist(const std::string& path_name, uint32_t* error_code = NULL);

    /// @beirf      获得文件或目录权限
    /// @param      path_name    要获得的文件名全路径名
    /// @param      permission   目标权限
    /// @param      error_code   保存出错信息
    /// @return     true 成功(存在)
    ///              false 失败
    static bool GetMode(const std::string& path_name,
                        uint32_t* permission,
                        uint32_t* error_code = NULL);

    /// @beirf      修改文件或目录权限
    /// @param      path_name    要修改的文件名全路径名
    /// @param      permission   目标权限
    /// @param      error_code   保存出错信息
    /// @return     true 成功(存在)
    ///              false 失败
    static bool Chmod(const std::string& path_name,
                      const uint32_t permission,
                      uint32_t* error_code = NULL);

    /// @beirf      修改文件或目录的role,注意只有xfs_admin才能修改xfs上文件的Role
    /// @param      path_name     要修改的文件名全路径名
    /// @param      role_name     xfs文件,此参数权限赋予的role;
    ///                            local文件, 此参数为user_name:group_name
    /// @param      error_code    保存出错信息
    /// @return     true 成功(存在)
    ///              false 失败
    static bool ChangeRole(const std::string& path_name,
                           const std::string& role_name,
                           uint32_t* error_code = NULL);

    /// @beirf      修改文件或目录的第二身份(second role), 仅提供xfs的实现
    ///             注意只有该文件或目录的拥有者(owner role)才能修改xfs上文件的second role
    /// @param      path_name        要修改的文件名全路径名
    /// @param      second_role_name 对xfs文件,此参数为一个role名称;
    /// @param      is_recursive     是否递归更改其子文件和子目录的second role
    /// @param      error_code       保存出错信息
    /// @return     true 成功;false 失败
    static bool ChangeSecondRole(const std::string& path_name,
                                 const std::string& second_role_name,
                                 bool is_recursive = false,
                                 uint32_t* error_code = NULL);

    /// @berif      计算指定文件内容的摘要(和XFS Server算法一致)
    /// @param      path_name   需要计算摘要的文件路径
    /// @param      digest      若成功,此参数保存返回的摘要值
    /// @param      error_code  保存出错信息
    /// @return     true 成功;false 失败
    /// @berif       调用过程中用户应保证文件内容不会被修改
    static bool GetDigest(const std::string& path_name,
                          uint32_t* digest,
                          uint32_t* error_code = NULL);

    /// @brief      获取错误码的描述
    /// @param      error_code     错误码
    /// @return     错误描述
    static const char* GetErrorCodeString(uint32_t error_code);

    /// @beirf      清理SDK模块
    /// @return     void
    static void CleanUp();


    virtual std::string GetFileImplName() = 0;

    /// @beirf      Get the prefix of a file name. The prefix must start with
    ///              a segement char '/'.
    ///              The prefix is the registered file system name.
    /// @param      file_path   the test file path
    /// @return     the prefix of a file name, or an empty string if fails.
    static std::string GetFilePrefix(const std::string& file_path);

    /// @brief       对指定的文件进行snapshot
    /// @param       source_path    源文件或目录全路径
    /// @param       target_path    目标全路径
    /// @param       error_code  保存出错信息
    /// @return      0 成功
    ///              -1 失败
    /// 导致失败的原因：
    ///               target_path存在或不可写
    ///               source_path不存在或不可读
    ///               quota不够
    static int32_t Snapshot(const std::string& source_path,
                            const std::string& target_path,
                            uint32_t* error_code = NULL);

    /// @brief       对指定的文件进行Freeze操作，成功之后，该文件不能被追加写
    /// @param       file_name  进行freeze操作的文件名，不支持通配符，并且不能是目录
    /// @param       error_code 保存出错信息
    /// @return      true 成功
    ///              false失败
    static bool Freeze(const std::string& file_name, uint32_t* error_code = NULL);

    /// @brief       修改指定的文件的备份因子，现在只支持增加备份因子数
    /// @param       file_name  进行操作的文件名，不支持通配符，并且不能是目录
    /// @param       error_code 保存出错信息
    /// @return      true 成功
    ///              false失败
    static bool SetReplication(const std::string& file_name,
                               int32_t new_backup_factor,
                               uint32_t* error_code = NULL);

    virtual ~File() {}

protected:
    /// @brief：     Create a File object for the file implementation.
    /// @param      prefix      the file prefix,
    ///                          which is also the file implementation name.
    /// @return     the pointer to the newly created subclass File object.
    ///              For unknown prefix, return default file implementation.
    ///              Return NULL for failures.
    static File* CreateFileImpl(const std::string& prefix);

    /// @beirf      Get the singleton File object for the file implementation.
    /// @param      prefix      the file prefix,
    ///                          which is also the file implementation name.
    /// @return     the pointer to the singleton subclass File object.
    ///              For unknown prefix, return the default file implementation.
    ///              Return NULL for failures.
    static File* GetFileImplSingleton(const std::string& prefix);

    /// @beirf      Get the singleton File object for the file implementation.
    /// @param      path      the file path,
    /// @return     the pointer to the singleton subclass File object.
    ///             For unknown prefix, return the default file implementation.
    ///             Return NULL for failures.
    static File* GetFileImplByPath(const std::string& path);

protected:
    /// File system level operation implementation.
    /// Each operation is the virtual one for the corresponding static method.
    /// Name conversion: has the posfix impl for implementation.

    /// @beirf      初始化SDK模块,在进程中初始化调用一次,
    ///              不需要每个File对象都调用
    ///
    /// @return     true, 成功
    ///              false,失败
    /// @see:        Init
    virtual bool InitImpl() = 0;

    /// @beirf      清理SDK模块
    /// @return     void
    /// @see:        Cleanup
    virtual bool CleanupImpl() = 0;

    ///
    /// @beirf      打开一个文件
    /// @param      file_path   要打开的文件全路径,可以是本地文件或/xfs/网络文件
    ///                          可以下述形式附加选项.
    ///                          file_path:k1=v1:k2=v2
    ///                          选项具体由文件实现解释.
    ///                          当附加的选项与传入的options重复时，以此处为准.
    /// @param      flags       要打开文件的模式,包括打开普通文件或者记录型文件,
    ///                          读写权限，异步或同步读写
    /// @param      options     选项，包括是否使用客户端cache，副本数量等
    /// @param      error_code  用于返回出错代码
    /// @return     表示分配的文件指针; NULL 表示出错
    ///
    /// @note       flags各个模式的意义：
    /// W:           如果不存在会建立文件;如果文件存在，都会把之前文件删除，
    ///              建立一个新的文件;文件指针在文件头;
    ///
    /// A:           如果文件存在,在文件尾追加;若不存在,则建立一个文件,
    ///              文件读写指针在最后;
    ///
    /// R:           只能打开已存在的文件,文件指针在文件头;
    /// R|W:         文件不存在时,会新建文件,文件指针在头部;文件存在时，
    ///              若写文件，则会自动移动到文件尾;
    ///
    /// W|A:         不允许;
    /// R|A:         与RW相同;
    /// R|W|A:       与RW相同;
    /// @see:        Open
    virtual bool OpenImpl(const char *file_path, uint32_t flags,
                        const OpenFileOptions& options,
                        uint32_t *error_code) = 0;

    /// @beirf      移动文件或子目录
    /// @param      src_name    源文件或目录全路径，目录名以/结尾
    /// @param      dest_name   目标目录全路径，目录名以/结尾
    /// @param      error_code  保存出错信息
    /// @return     0 成功
    ///              -1 失败
    /// @see:        Move
    virtual int32_t MoveImpl(const char* src_name,
                        const char* dst_name, uint32_t* error_code) = 0;


    /// @beirf      修改文件或子目录名字
    /// @param      old_path_name   旧的文件或目录全路径名,目录名以/结尾
    /// @param      new_path_name   新的文件或目录名，目录名以/结尾
    /// @param      error_code      保存出错信息
    /// @return     0 成功
    ///              -1 失败
    /// @see:        Rename
    virtual int32_t  RenameImpl(const char* old_path_name,
                                const char* new_path_name,
                                uint32_t*   error_code) = 0;

    /// @beirf      修改文件或子目录名字
    /// @param      old_path_name   旧的文件或目录全路径名,目录名以/结尾
    /// @param      new_path_name   新的文件或目录名，目录名以/结尾
    /// @param      rename_option   rename操作的选项
    /// @param      error_code      保存出错信息
    /// @return     0 成功
    ///              -1 失败
    /// @see:        Rename
    virtual int32_t  RenameImplWithOptions(const std::string& old_path_name,
                                           const std::string& new_path_name,
                                           const RenameOptions& rename_option,
                                           uint32_t*   error_code);

    /// @beirf      删除文件或子目录
    /// @param      path_name       待删除的文件或目录全路径名,目录名以/结尾
    /// @param      is_recursive    是否递归删除其子文件和子目录.
    ///                              部分文件实现不支持.
    /// @param      error_code      保存出错信息
    /// @return     0 成功
    ///              -1 失败
    /// @see:        Remove
    virtual int32_t  RemoveImpl(const char* path_name,
                                bool        is_recursive,
                                uint32_t*   error_code) = 0;

    /// @beirf      添加子目录
    /// @param      path_name   目录全路径名,目录名以/结尾
    /// @param      error_code  保存出错信息
    /// @return     0 成功
    ///              -1 失败
    /// @see:        AddDir
    virtual int32_t  AddDirImpl(const char* path_name,
                                uint32_t* error_code) = 0;


    /// @beirf      查看指定目录或文件的属性信息
    /// @param      pattern     要查看的目录或文件，目录以/结尾，支持* ? []
    /// @param      list_option List操作的选项,包含AttrMask，last_file,count三个参数，
    ///                         具体的含义参看ListOptions定义
    /// @param      attrs      放返回的属性信息,应用层分配的缓冲区
    /// @param      error_code  保存出错信息
    /// @return     0 成功; -1 失败
    /// @see:        List
    virtual int32_t ListImpl(const char* pattern,
                             const ListOptions& list_option,
                             std::vector<AttrsInfo>* attrs,
                             uint32_t* error_code) = 0;

    /// @beirf      查看指定目录或文件的属性信息
    /// @param      name        要查看的目录或文件，不支持* ? [].
    /// @param      mask        需要获取的属性信息掩码
    /// @param      attrs_info  返回的属性信息
    /// @param      error_code  保存出错信息
    /// @return     true 成功; false 失败
    virtual bool  GetAttrsImpl(const std::string& name,
                               const AttrsMask& mask,
                               AttrsInfo* attrs_info,
                               uint32_t* error_code = NULL);

    /// @beirf      查看文件或子目录占用的空间
    /// @param      path_name   查看的文件或目录全路径名，目录须以/结尾
    /// @param      error_code  保存出错信息
    /// @return     >=0 成功(返回文件或子目录大小)
    ///              -1 失败
    /// @see:        Du
    virtual int64_t  DuImpl(const char* path_name,
                        uint32_t* error_code) = 0;

    /// @beirf      查看文件或子目录的汇总后的统计信息
    /// @param      path_name   查看的文件或目录全路径名
    /// @param      mask        in/out参数，传入想要获取的信息的掩码.
    ///                         运行后被设置成文件系统实际支持的信息的掩码.
    /// @param      summary     保存汇总的统计信息
    /// @param      error_code  保存出错信息
    /// @return     true 成功, false 失败
    virtual bool GetContentSummaryImpl(
        const char* path_name,
        FileContentSummaryMask* mask,
        FileContentSummary* summary,
        uint32_t* error_code) = 0;

    /// @beirf      获得文件大小
    /// @param      file_name   目录全路径名
    /// @param      error_code  保存出错信息
    /// @return     >=0 成功
    ///              -1 失败
    /// @see:        GetSize
    virtual int64_t  GetSizeImpl(const char* file_name,
                        uint32_t*   error_code) = 0;

    /// @beirf      查看指定文件或者目录是否存在
    /// @param      path_name    要查看的文件名全路径名，目录以/结尾
    /// @param      error_code   保存出错信息
    /// @return     true 成功(存在)
    ///              false 失败
    /// @see:        CheckExist
    virtual bool CheckExistImpl(const char* path_name,
                        uint32_t* error_code) = 0;

    /// @beirf      获得文件或目录权限
    /// @param      path_name    要获得的文件名全路径名
    /// @param      permission   目标权限
    /// @param      error_code   保存出错信息
    /// @return     true 成功(存在)
    ///             false 失败
    virtual bool GetModeImpl(const std::string& path_name,
                             uint32_t* permission,
                             uint32_t* error_code);

    /// @beirf      修改文件或目录权限
    /// @param      path_name    要修改的文件名全路径名
    /// @param      permission   目标权限
    /// @param      error_code   保存出错信息
    /// @return     true 成功(存在)
    ///             false 失败
    virtual bool ChmodImpl(const char* path_name,
                           const uint32_t permission,
                           uint32_t* error_code) = 0;

    /// @beirf      修改文件或目录的role,注意只有xfs_admin才能修改xfs上文件的Role
    /// @param      path_name     要修改的文件名全路径名
    /// @param      role_name     对xfs文件,此参数为一个role名称;
    ///                            对local文件, 此参数为user_name:group_name
    /// @param      error_code    保存出错信息
    /// @return     true 成功;false 失败
    virtual bool ChangeRoleImpl(const char* path_name,
                                const char* role_name,
                                uint32_t* error_code) = 0;

    /// @beirf      修改文件或目录的第二身份(second role), 仅提供xfs的实现
    ///             注意只有该文件或目录的拥有者(owner role)才能修改xfs上文件的second role
    /// @param      path_name        要修改的文件名全路径名
    /// @param      second_role_name 对xfs文件,此参数为一个role名称;
    /// @param      is_recursive     是否递归更改其子文件和子目录的second role
    /// @param      error_code       保存出错信息
    /// @return     true 成功;false 失败
    virtual bool ChangeSecondRoleImpl(const char* path_name,
                                      const char* second_role_name,
                                      bool is_recursive,
                                      uint32_t* error_code);

    /// @berif      计算指定文件内容的摘要(和XFS Server算法一致)
    /// @param      file_obj    文件句柄
    /// @param      file_size   文件的长度
    /// @param      digest      若成功,此参数保存返回的摘要值
    /// @param      error_code  保存出错信息
    /// @return     true 成功;false 失败
    virtual bool GetDigestImpl(File* file_obj,
                               int64_t   file_size,
                               uint32_t* digest,
                               uint32_t* error_code) = 0;

    /// @brief       对指定的文件进行snapshot
    /// @param       source_path    源文件或目录全路径
    /// @param       target_path    目标全路径
    /// @param       error_code     保存出错信息
    /// @return      0 成功
    ///              -1 失败
    /// TODO(bradzhang): 为了兼容现有代码，此处不使用纯虚函数
    virtual int32_t SnapshotImpl(const char* source_path,
                                 const char* target_path,
                                 uint32_t* error_code);

    /// @brief       对指定的文件进行Freeze操作，成功之后，该文件不能被追加写
    /// @param       file_name  进行freeze操作的文件名，不支持通配符，并且不能是目录
    /// @param       error_code 保存出错信息
    /// @return      true 成功
    ///              false失败
    virtual bool FreezeImpl(const std::string& file_name, uint32_t* error_code);

    /// @brief       修改指定的文件的备份因子，现在只支持增加备份因子数
    /// @param       file_name  进行操作的文件名，不支持通配符，并且不能是目录
    /// @param       error_code 保存出错信息
    /// @return      true 成功
    ///              false失败
    virtual bool SetReplicationImpl(const std::string& file_name,
                                    int32_t new_backup_factor,
                                    uint32_t* error_code);


    /// A helper functions to set error code when necessary.
    static void SetErrorCode(uint32_t* error_code, FILE_ERROR_CODE code) {
        if (error_code) {
            *error_code = code;
        }
    }

    // 构造函数
    File() {}

    static bool IsSameFilePath(const std::string& path_name, const std::string& ref_path_name);

    static bool IsRenameParamValid(const std::string& old_path_name,
                                   const std::string& new_path_name,
                                   uint32_t*   error_code);
};

// Following Macros are for registering and creating file implementations.

CLASS_REGISTER_DEFINE_REGISTRY(file_impl_register, File);

#define REGISTER_FILE_IMPL(path_prefix_as_string, file_impl_name) \
    CLASS_REGISTER_OBJECT_CREATOR_WITH_SINGLETON( \
    file_impl_register, File, path_prefix_as_string, file_impl_name)

#define CREATE_FILE_IMPL(path_prefix_as_string) \
    CLASS_REGISTER_CREATE_OBJECT(file_impl_register, path_prefix_as_string)

#define GET_FILE_IMPL_SINGLETON(path_prefix_as_string) \
    CLASS_REGISTER_GET_SINGLETON(file_impl_register, path_prefix_as_string)

#define FILE_IMPL_COUNT() \
    CLASS_REGISTER_CREATOR_COUNT(file_impl_register)

#define FILE_IMPL_NAME(i) \
    CLASS_REGISTER_CREATOR_NAME(file_impl_register, i)

} // namespace common

// TODO(aaronzou): this using is a quick solution to fix link problems with other projects.
//      Next version must delete this one and all projects use qualified names.
//      Including this header is dangerous and may introduce conflict names.

using common::File;

#endif // COMMON_FILE_FILE_H
