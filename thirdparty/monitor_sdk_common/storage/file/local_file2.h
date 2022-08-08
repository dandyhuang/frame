// Copyright (c) 2015, Vivo Inc. All rights reserved.
/**
 * @file local_file.h
 * @brief
 * @author chenzheng05@baidu.com
 * @date 2015-01-20
 */
//            Refactor to use polymorphism to forward to the right subclass.

#ifndef COMMON_FILE_LOCAL_FILE_H
#define COMMON_FILE_LOCAL_FILE_H

#include <stdio.h>
#include <string>
#include <vector>

#include "thirdparty/monitor_sdk_common/storage/file/cipher_base.h"
#include "thirdparty/monitor_sdk_common/storage/file/file2.h"

#include "thirdparty/monitor_sdk_common/system/concurrency/mutex.h"

namespace common {

const char LOCAL_FILE_PREFIX[] = "/local/";

// 本地异步读写的线程
// extern LocalFileThread*         g_local_thread;

class LocalFile : public File {
public:
    // Let public methods be virtual, so that the subclass could extends these when necessary.

    LocalFile();

    virtual ~LocalFile();

    virtual std::string GetFileImplName() {
        return LOCAL_FILE_PREFIX;
    }

    /// @beirf      关闭一个文件
    /// @param      error_code  存放出错信息
    /// @return     0 成功; -1 失败
    ///
    /// @note:       关闭之前会主动调用Flush一次
    virtual int32_t Close(uint32_t* error_code = NULL);

    /// @beirf      将本地数据刷新至网络服务器或磁盘
    /// @param      error_code  存放出错信息
    /// @return     0  成功
    ///              <0 失败
    virtual int32_t Flush(uint32_t* error_code = NULL);

    /// @beirf      同步方式向打开的文件写入数据，文件通常缓存在本地,
    ///              调用Flush或Close时刷新到网络文件. 函数默认有3次重试，最大重试次数由FLAG决定。
    ///
    /// @param      buffer      指向待写入的数据
    /// @param      size        待写入的文件长度
    /// @param      error_code  存放出错信息, 返回成功时，实际写入的字节数等于传入buf_size。
    ///
    /// @return     实际成功写入的长度;
    ///              -1表示失败。
    virtual int64_t Write(const void* buf, int64_t buf_size,
                          uint32_t* error_code = NULL);

    /// @beirf      同步从文件读取数据
    /// @param      buffer      表示存放数据的本地内存空间；
    /// @param      size        表示需要读取的最大数据长度；
    /// @param      error_code  存放错误信息；
    /// @return     -1 出错
    ///              0  读到了文件结尾
    ///              >0 实际读取到的文件长度
    virtual int64_t Read(void* buf, int64_t buf_size,
                uint32_t* error_code = NULL);

    /// @beirf      同步从文件读取一行数据, 最多max_size - 1个字节
    ///              以'\0'结束
    /// @param      buffer      表示存放数据的本地内存空间；
    /// @param      max_size    表示需要读取的最大数据长度；
    /// @return     -1 出错
    ///              0  读到了文件结尾
    ///              >0 实际读取到的文件长度
    virtual int32_t ReadLine(void* buffer, int32_t max_size);

    /// @beirf      异步方式向打开的文件写入数据，文件通常缓存在本地,
    ///              调用Flush或Close时刷新到网络文件。只支持追加方式写数据。
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
    virtual int32_t AsyncWrite(const void* buf, int64_t buf_size,
                               Closure<void, int64_t, uint32_t>* callback,
                               uint32_t time_out = kDefaultAsyncTimeout,
                               uint32_t* error_code = NULL);


    /// @beirf       从文件的指定位置异步读取数据
    /// @param       buffer         表示存放数据的本地内存空间；
    /// @param       size           表示需要读取的最大数据长度；
    /// @param       start_position 表示从该位置开始读取数据；
    /// @param       callback       与当前数据相对应的回调函数
    ///                              (void:表示回调函数的返回值,后两个是参数:
    ///                              第一个表示成功传输的长度;
    ///                              第二个为errorcode)
    ///
    /// @param       timeout        超时时间,seconds
    /// @param       error_code     保存错误信息
    /// @return      =0 成功
    ///               -1 失败
    virtual int32_t AsyncReadFrom(void* buffer,
                                  int64_t size,
                                  int64_t start_position,
                                  Closure<void, int64_t, uint32_t>* callback,
                                  uint32_t  timeout = kDefaultAsyncTimeout,
                                  uint32_t* error_code = NULL);

    /// @beirf      The file implementation support asynchronous operations?
    ///              For file implementation doesn't support aync operaton,
    ///                  the behavior of calling async operation is undefined.
    /// @return     return true if support async operations.
    virtual bool SupportAsync() {
        return true;
    }


    /// @beirf      改变文件的当前偏移量
    /// @param      offset      相对于origin的偏移量
    /// @param      whence      偏移量的相对位置
    ///              可选参数:
    ///                  SEEK_SET    文件开始
    ///                  SEEK_CUR    文件的当前位置
    ///                  SEEK_END    文件结尾
    /// @param      error_code  返回出错信息
    /// @return     >=0 成功(返回当前文件的偏移量)
    ///              1 出错
    virtual int64_t Seek(int64_t offset, int32_t origin, uint32_t* error_code = NULL);



    /// @beirf      返回当前文件的偏移量
    /// @param      error_code   保存出错信息
    /// @return     >=0 成功(返回当前文件的偏移量);-1 出错
    virtual int64_t Tell(uint32_t* error_code = NULL);


    /// @beirf      设置文件有效大小（截短）
    /// @param      length      文件有效大小
    /// @param      error_code  保存出错信息
    /// @return     >=0 成功
    ///              -1 出错
    virtual int32_t Truncate(uint64_t length, uint32_t* error_code = NULL);

    /// @beirf      获取数据所在的NodeServe位置
    /// @param      start       数据块的开始位置
    /// @param      end         数据块的结束位置
    /// @param      buffer      获取到的位置信息
    /// @param      error_code  保存出错代码
    /// @return     0 成功
    ///              -1 失败
    virtual int32_t LocateData(uint64_t start_pos, uint64_t end_pos,
                               std::vector<DataLocation>* buf,
                               uint32_t* error_code = NULL);

protected:
    // Cipher file or not
    inline bool IsCipherFile() {return (m_open_flag & ENUM_FILE_TYPE_CIPHER) ? true : false;}

    /// @beirf      将本地数据刷新至网络服务器或磁盘
    /// @param      error_code  存放出错信息
    /// @return     0  成功
    ///              <0 失败
    virtual int32_t RealFlush(uint32_t* error_code = NULL);

    /// @beirf      同步方式向打开的文件写入数据，文件通常缓存在本地,
    ///              调用Flush或Close时刷新到网络文件. 函数默认有3次重试，最大重试次数由FLAG决定。
    ///
    /// @param      buffer      指向待写入的数据
    /// @param      size        待写入的文件长度
    /// @param      error_code  存放出错信息, 返回成功时，实际写入的字节数等于传入buf_size。
    ///
    /// @return     实际成功写入的长度;
    ///              -1表示失败。
    virtual int64_t RealWrite(const void* buf, int64_t buf_size,
                uint32_t* error_code = NULL);

    /// @beirf      同步从文件读取数据
    /// @param      buffer      表示存放数据的本地内存空间；
    /// @param      size        表示需要读取的最大数据长度；
    /// @param      error_code  存放错误信息；
    /// @return     -1 出错
    ///              0  读到了文件结尾
    ///              >0 实际读取到的文件长度
    virtual int64_t RealRead(void* buf, int64_t buf_size,
                uint32_t* error_code = NULL);

    /// @beirf      初始化SDK模块,在进程中初始化调用一次,
    ///              不需要每个File对象都调用
    ///
    ///
    /// @return     true, 成功
    ///              false,失败
    /// @see:        Init
    virtual bool InitImpl();

    /// @beirf      清理SDK模块
    /// @return     void
    /// @see:        Cleanup
    virtual bool CleanupImpl();

    ///
    /// @beirf      打开一个文件
    /// @param      file_path   要打开的文件全路径,可以是本地文件或/xfs/网络文件
    ///                          可以下述形式附加选项.
    ///                          file_path:k1=v1:k2=v2.
    ///                          目前的LocalFile实现不支持额外的选项.
    /// @param      flags       要打开文件的模式,包括打开普通文件或者记录型文件,
    ///                          读写权限，异步或同步读写
    /// @param      options     选项，包括是否使用客户端cache，副本数量等
    ///                          目前的LocalFile实现忽略选项.
    /// @param      error_code  用于返回出错代码
    /// @return     表示分配的文件指针; NULL 表示出错
    ///
    /// @note:       flags各个模式的意义：
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
    /// @see:        Open
    virtual bool OpenImpl(const char* file_path, uint32_t flags,
                          const OpenFileOptions& options,
                          uint32_t *error_code);


    /// @beirf      查看指定目录或文件的属性信息
    /// @param      pattern     要查看的目录或文件，目录以/结尾，支持* ? []
    /// @param      list_option List操作的选项,包含AttrMask，last_file,count三个参数
    ///                          具体的含义参看ListOptions定义.
    /// @param      buffer      放返回的属性信息,应用层分配的缓冲区
    ///                          某些字段可能由于获取信息失败而保留为其默认值.
    /// @param      error_code  保存出错信息
    /// @return     0 成功; -1 失败
    /// @see:        List
    virtual int32_t ListImpl(const char* pattern,
                             const ListOptions& list_option, std::vector<AttrsInfo>* attrs_info,
                             uint32_t* error_code);

    /// @beirf      查看指定文件或者目录是否存在
    /// @param      file_name   要查看的文件名全路径名，目录以/结尾
    /// @param      error_code  保存出错信息
    /// @return     true 成功(存在)
    ///              false 失败
    /// @see:        CheckExist
    virtual bool CheckExistImpl(const char* file_name,
                                uint32_t* error_code);

    /// @beirf      查看文件或子目录占用的空间
    /// @param      path        查看的文件或目录全路径名，目录须以/结尾
    /// @param      error_code  保存出错信息
    /// @return     >=0 成功(返回文件或子目录大小)
    ///              -1 失败
    /// @see:        Du
    virtual int64_t  DuImpl(const char* path_name, uint32_t* error_code);

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
        uint32_t* error_code);

    /// @beirf      移动文件或子目录，允许递归移动，可以跨磁盘.
    ///              如果src_name和dest_name都是文件，相当于改名
    ///              如果src_name是文件，dest_name是目录，则把文件移动到对应目录
    ///              如果src_name和dest_name都是目录，则把src_name的全部内容递归移动到dest_name
    /// @param      src_name    源文件或目录全路径，目录名以/结尾
    /// @param      dest_name   目标目录全路径，目录名以/结尾
    /// @param      error_code  保存出错信息
    /// @return     0 成功
    ///              -1 失败
    /// @see:        Move
    virtual int32_t MoveImpl(const char* src_name, const char* dst_name, uint32_t* error_code);


    /// @beirf      修改文件或子目录名字，不能跨磁盘，语义同C库的rename.
    /// @param      old_path_name   旧的文件或目录全路径名,目录名以/结尾
    /// @param      new_path_name   新的文件或目录名，目录名以/结尾
    /// @param      error_code      保存出错信息
    /// @return     0 成功
    ///              -1 失败
    /// @see:        Rename
    virtual int32_t  RenameImpl(const char* old_path_name,
                                const char* new_path_name,
                                uint32_t*   error_code);

    /// @beirf      修改文件或子目录名字，不能跨磁盘，语义同C库的rename.
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
    /// @param      is_recursive    是否递归删除其子文件和子目录, 部分文件实现不支持.
    /// @param      error_code      保存出错信息
    /// @return     0 成功
    ///              -1 失败
    /// @see:        Remove
    virtual int32_t RemoveImpl(const char* file_name, bool is_recursive,
                               uint32_t* error_code);


    /// @beirf      添加子目录
    /// @param      path_name   目录全路径名,目录名以/结尾
    /// @param      error_code  保存出错信息
    /// @return     0 成功
    ///              -1 失败
    /// @see:        AddDir
    virtual int32_t AddDirImpl(const char* file_name, uint32_t* error_code);

    /// @beirf      获得文件大小
    /// @param      file_name   目录全路径名
    /// @param      error_code  保存出错信息
    /// @return     >=0 成功
    ///             -1 失败
    virtual int64_t GetSizeImpl(const char* file_name, uint32_t* error_code);

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
    /// @param      path_name    要查看的文件名全路径名
    /// @param      permission   目标权限
    /// @param      error_code   保存出错信息
    /// @return     true 成功(存在)
    ///             false 失败
    virtual bool ChmodImpl(const char* path_name,
                           const uint32_t permission,
                           uint32_t* error_code);

    /// @beirf      修改文件或目录的owner
    /// @param      path_name     要修改的文件全路径名
    /// @param      role_name     user_name:group_name的组合
    /// @param      error_code    保存出错信息
    /// @return     true 成功(存在)
    ///             false 失败
    virtual bool ChangeRoleImpl(const char* path_name,
                                const char* role_name,
                                uint32_t* error_code);

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
    /// @param      path_name   需要计算摘要的文件路径
    /// @param      file_size   文件的长度
    /// @param      digest      若成功,此参数保存返回的摘要值
    /// @param      error_code  保存出错信息
    /// @return     true 成功;false 失败
    /// @berif      调用过程中用户应保证文件内容不会被修改
    virtual bool GetDigestImpl(File* file_obj,
                               int64_t   file_size,
                               uint32_t* digest,
                               uint32_t* error_code);

private:
    FILE*                   m_fp;
    int32_t                 m_open_flag;
    SimpleMutex             m_mutex;
    CipherBase*             m_cipher;

    bool CheckOpenModeValid(const uint32_t flags);

    // Build the full path for file_name in file_path.
    static std::string ConnectPathComponent(const char* file_path, const char* file_name);

    // Normalize a given file path and append seperator when necessary.
    static void NormalizePath(std::string* file_path);

    // Like basename command in shell script, but only support file name.
    // For a file path, get the last segment after the last seperator, means the file name.
    static std::string GetBaseName(const std::string& file_path);

    // Use errno of local file to set common::File error_code.
    static void SetErrorCodeByLocal(uint32_t* error_code, uint32_t local_errno);

    // actually write file in sync way to help AsyncWrite.
    void DoAsyncWrite(
        const void* buf,
        int64_t buf_size,
        Closure<void, int64_t, uint32_t>* callback,
        uint32_t time_out = kDefaultAsyncTimeout);

    // actually read file in sync way to help AsyncReadFrom.
    void DoAsyncReadFrom(
        void* buffer,
        int64_t size,
        int64_t start_position,
        Closure<void, int64_t, uint32_t>* callback,
        uint32_t  timeout = kDefaultAsyncTimeout);

    void AsyncWriteAIOCallback(
        Closure<void, int64_t, uint32_t>* out_callback,
        int64_t start_position,
        int64_t size,
        uint32_t status_code);

    void AsyncReadFromAIOCallback(
        Closure<void, int64_t, uint32_t>* out_callback,
        int64_t size,
        uint32_t status_code);

    // return 0 for valid path without wildcard
    // return 1 for valid path with wildcard in last path component
    // return -1 for invalide file path has wildcard in non-last path component
    int32_t ValidatePathWildcard(const char* file_path);
};

} // namespace common

#endif // COMMON_FILE_LOCAL_FILE_H
