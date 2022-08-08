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

// �����첽��д���߳�
// extern LocalFileThread*         g_local_thread;

class LocalFile : public File {
public:
    // Let public methods be virtual, so that the subclass could extends these when necessary.

    LocalFile();

    virtual ~LocalFile();

    virtual std::string GetFileImplName() {
        return LOCAL_FILE_PREFIX;
    }

    /// @beirf      �ر�һ���ļ�
    /// @param      error_code  ��ų�����Ϣ
    /// @return     0 �ɹ�; -1 ʧ��
    ///
    /// @note:       �ر�֮ǰ����������Flushһ��
    virtual int32_t Close(uint32_t* error_code = NULL);

    /// @beirf      ����������ˢ������������������
    /// @param      error_code  ��ų�����Ϣ
    /// @return     0  �ɹ�
    ///              <0 ʧ��
    virtual int32_t Flush(uint32_t* error_code = NULL);

    /// @beirf      ͬ����ʽ��򿪵��ļ�д�����ݣ��ļ�ͨ�������ڱ���,
    ///              ����Flush��Closeʱˢ�µ������ļ�. ����Ĭ����3�����ԣ�������Դ�����FLAG������
    ///
    /// @param      buffer      ָ���д�������
    /// @param      size        ��д����ļ�����
    /// @param      error_code  ��ų�����Ϣ, ���سɹ�ʱ��ʵ��д����ֽ������ڴ���buf_size��
    ///
    /// @return     ʵ�ʳɹ�д��ĳ���;
    ///              -1��ʾʧ�ܡ�
    virtual int64_t Write(const void* buf, int64_t buf_size,
                          uint32_t* error_code = NULL);

    /// @beirf      ͬ�����ļ���ȡ����
    /// @param      buffer      ��ʾ������ݵı����ڴ�ռ䣻
    /// @param      size        ��ʾ��Ҫ��ȡ��������ݳ��ȣ�
    /// @param      error_code  ��Ŵ�����Ϣ��
    /// @return     -1 ����
    ///              0  �������ļ���β
    ///              >0 ʵ�ʶ�ȡ�����ļ�����
    virtual int64_t Read(void* buf, int64_t buf_size,
                uint32_t* error_code = NULL);

    /// @beirf      ͬ�����ļ���ȡһ������, ���max_size - 1���ֽ�
    ///              ��'\0'����
    /// @param      buffer      ��ʾ������ݵı����ڴ�ռ䣻
    /// @param      max_size    ��ʾ��Ҫ��ȡ��������ݳ��ȣ�
    /// @return     -1 ����
    ///              0  �������ļ���β
    ///              >0 ʵ�ʶ�ȡ�����ļ�����
    virtual int32_t ReadLine(void* buffer, int32_t max_size);

    /// @beirf      �첽��ʽ��򿪵��ļ�д�����ݣ��ļ�ͨ�������ڱ���,
    ///              ����Flush��Closeʱˢ�µ������ļ���ֻ֧��׷�ӷ�ʽд���ݡ�
    ///
    /// @param      buffer      ָ���д�������
    /// @param      size        ��д����ļ�����
    /// @param      callback    ����д��ɹ���ûص�������������
    ///                          (void:��ʾ�ص������ķ���ֵ,�������ǲ���:
    ///                          ��һ����ʾ�ɹ�����ĳ���;
    ///                          �ڶ���Ϊerrorcode)
    ///
    /// @param      timeout     ��ʱʱ��,seconds
    /// @param      error_code  ��ų�����Ϣ,
    ///
    /// @return     =0 �ɹ�(д��ĳ���)
    ///              -1 ʧ��
    virtual int32_t AsyncWrite(const void* buf, int64_t buf_size,
                               Closure<void, int64_t, uint32_t>* callback,
                               uint32_t time_out = kDefaultAsyncTimeout,
                               uint32_t* error_code = NULL);


    /// @beirf       ���ļ���ָ��λ���첽��ȡ����
    /// @param       buffer         ��ʾ������ݵı����ڴ�ռ䣻
    /// @param       size           ��ʾ��Ҫ��ȡ��������ݳ��ȣ�
    /// @param       start_position ��ʾ�Ӹ�λ�ÿ�ʼ��ȡ���ݣ�
    /// @param       callback       �뵱ǰ�������Ӧ�Ļص�����
    ///                              (void:��ʾ�ص������ķ���ֵ,�������ǲ���:
    ///                              ��һ����ʾ�ɹ�����ĳ���;
    ///                              �ڶ���Ϊerrorcode)
    ///
    /// @param       timeout        ��ʱʱ��,seconds
    /// @param       error_code     ���������Ϣ
    /// @return      =0 �ɹ�
    ///               -1 ʧ��
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


    /// @beirf      �ı��ļ��ĵ�ǰƫ����
    /// @param      offset      �����origin��ƫ����
    /// @param      whence      ƫ���������λ��
    ///              ��ѡ����:
    ///                  SEEK_SET    �ļ���ʼ
    ///                  SEEK_CUR    �ļ��ĵ�ǰλ��
    ///                  SEEK_END    �ļ���β
    /// @param      error_code  ���س�����Ϣ
    /// @return     >=0 �ɹ�(���ص�ǰ�ļ���ƫ����)
    ///              1 ����
    virtual int64_t Seek(int64_t offset, int32_t origin, uint32_t* error_code = NULL);



    /// @beirf      ���ص�ǰ�ļ���ƫ����
    /// @param      error_code   ���������Ϣ
    /// @return     >=0 �ɹ�(���ص�ǰ�ļ���ƫ����);-1 ����
    virtual int64_t Tell(uint32_t* error_code = NULL);


    /// @beirf      �����ļ���Ч��С���ض̣�
    /// @param      length      �ļ���Ч��С
    /// @param      error_code  ���������Ϣ
    /// @return     >=0 �ɹ�
    ///              -1 ����
    virtual int32_t Truncate(uint64_t length, uint32_t* error_code = NULL);

    /// @beirf      ��ȡ�������ڵ�NodeServeλ��
    /// @param      start       ���ݿ�Ŀ�ʼλ��
    /// @param      end         ���ݿ�Ľ���λ��
    /// @param      buffer      ��ȡ����λ����Ϣ
    /// @param      error_code  ����������
    /// @return     0 �ɹ�
    ///              -1 ʧ��
    virtual int32_t LocateData(uint64_t start_pos, uint64_t end_pos,
                               std::vector<DataLocation>* buf,
                               uint32_t* error_code = NULL);

protected:
    // Cipher file or not
    inline bool IsCipherFile() {return (m_open_flag & ENUM_FILE_TYPE_CIPHER) ? true : false;}

    /// @beirf      ����������ˢ������������������
    /// @param      error_code  ��ų�����Ϣ
    /// @return     0  �ɹ�
    ///              <0 ʧ��
    virtual int32_t RealFlush(uint32_t* error_code = NULL);

    /// @beirf      ͬ����ʽ��򿪵��ļ�д�����ݣ��ļ�ͨ�������ڱ���,
    ///              ����Flush��Closeʱˢ�µ������ļ�. ����Ĭ����3�����ԣ�������Դ�����FLAG������
    ///
    /// @param      buffer      ָ���д�������
    /// @param      size        ��д����ļ�����
    /// @param      error_code  ��ų�����Ϣ, ���سɹ�ʱ��ʵ��д����ֽ������ڴ���buf_size��
    ///
    /// @return     ʵ�ʳɹ�д��ĳ���;
    ///              -1��ʾʧ�ܡ�
    virtual int64_t RealWrite(const void* buf, int64_t buf_size,
                uint32_t* error_code = NULL);

    /// @beirf      ͬ�����ļ���ȡ����
    /// @param      buffer      ��ʾ������ݵı����ڴ�ռ䣻
    /// @param      size        ��ʾ��Ҫ��ȡ��������ݳ��ȣ�
    /// @param      error_code  ��Ŵ�����Ϣ��
    /// @return     -1 ����
    ///              0  �������ļ���β
    ///              >0 ʵ�ʶ�ȡ�����ļ�����
    virtual int64_t RealRead(void* buf, int64_t buf_size,
                uint32_t* error_code = NULL);

    /// @beirf      ��ʼ��SDKģ��,�ڽ����г�ʼ������һ��,
    ///              ����Ҫÿ��File���󶼵���
    ///
    ///
    /// @return     true, �ɹ�
    ///              false,ʧ��
    /// @see:        Init
    virtual bool InitImpl();

    /// @beirf      ����SDKģ��
    /// @return     void
    /// @see:        Cleanup
    virtual bool CleanupImpl();

    ///
    /// @beirf      ��һ���ļ�
    /// @param      file_path   Ҫ�򿪵��ļ�ȫ·��,�����Ǳ����ļ���/xfs/�����ļ�
    ///                          ����������ʽ����ѡ��.
    ///                          file_path:k1=v1:k2=v2.
    ///                          Ŀǰ��LocalFileʵ�ֲ�֧�ֶ����ѡ��.
    /// @param      flags       Ҫ���ļ���ģʽ,��������ͨ�ļ����߼�¼���ļ�,
    ///                          ��дȨ�ޣ��첽��ͬ����д
    /// @param      options     ѡ������Ƿ�ʹ�ÿͻ���cache������������
    ///                          Ŀǰ��LocalFileʵ�ֺ���ѡ��.
    /// @param      error_code  ���ڷ��س������
    /// @return     ��ʾ������ļ�ָ��; NULL ��ʾ����
    ///
    /// @note:       flags����ģʽ�����壺
    /// W:           ��������ڻὨ���ļ�;����ļ����ڣ������֮ǰ�ļ�ɾ����
    ///              ����һ���µ��ļ�;�ļ�ָ�����ļ�ͷ;
    ///
    /// A:           ����ļ�����,���ļ�β׷��;��������,����һ���ļ�,
    ///              �ļ���дָ�������;
    ///
    /// R:           ֻ�ܴ��Ѵ��ڵ��ļ�,�ļ�ָ�����ļ�ͷ;
    /// R|W:         ������;
    ///
    /// W|A:         ������;
    /// R|A:         ������;
    /// R|W|A:       ������;
    /// @see:        Open
    virtual bool OpenImpl(const char* file_path, uint32_t flags,
                          const OpenFileOptions& options,
                          uint32_t *error_code);


    /// @beirf      �鿴ָ��Ŀ¼���ļ���������Ϣ
    /// @param      pattern     Ҫ�鿴��Ŀ¼���ļ���Ŀ¼��/��β��֧��* ? []
    /// @param      list_option List������ѡ��,����AttrMask��last_file,count��������
    ///                          ����ĺ���ο�ListOptions����.
    /// @param      buffer      �ŷ��ص�������Ϣ,Ӧ�ò����Ļ�����
    ///                          ĳЩ�ֶο������ڻ�ȡ��Ϣʧ�ܶ�����Ϊ��Ĭ��ֵ.
    /// @param      error_code  ���������Ϣ
    /// @return     0 �ɹ�; -1 ʧ��
    /// @see:        List
    virtual int32_t ListImpl(const char* pattern,
                             const ListOptions& list_option, std::vector<AttrsInfo>* attrs_info,
                             uint32_t* error_code);

    /// @beirf      �鿴ָ���ļ�����Ŀ¼�Ƿ����
    /// @param      file_name   Ҫ�鿴���ļ���ȫ·������Ŀ¼��/��β
    /// @param      error_code  ���������Ϣ
    /// @return     true �ɹ�(����)
    ///              false ʧ��
    /// @see:        CheckExist
    virtual bool CheckExistImpl(const char* file_name,
                                uint32_t* error_code);

    /// @beirf      �鿴�ļ�����Ŀ¼ռ�õĿռ�
    /// @param      path        �鿴���ļ���Ŀ¼ȫ·������Ŀ¼����/��β
    /// @param      error_code  ���������Ϣ
    /// @return     >=0 �ɹ�(�����ļ�����Ŀ¼��С)
    ///              -1 ʧ��
    /// @see:        Du
    virtual int64_t  DuImpl(const char* path_name, uint32_t* error_code);

    /// @beirf      �鿴�ļ�����Ŀ¼�Ļ��ܺ��ͳ����Ϣ
    /// @param      path_name   �鿴���ļ���Ŀ¼ȫ·����
    /// @param      mask        in/out������������Ҫ��ȡ����Ϣ������.
    ///                         ���к����ó��ļ�ϵͳʵ��֧�ֵ���Ϣ������.
    /// @param      summary     ������ܵ�ͳ����Ϣ
    /// @param      error_code  ���������Ϣ
    /// @return     true �ɹ�, false ʧ��
    virtual bool GetContentSummaryImpl(
        const char* path_name,
        FileContentSummaryMask* mask,
        FileContentSummary* summary,
        uint32_t* error_code);

    /// @beirf      �ƶ��ļ�����Ŀ¼������ݹ��ƶ������Կ����.
    ///              ���src_name��dest_name�����ļ����൱�ڸ���
    ///              ���src_name���ļ���dest_name��Ŀ¼������ļ��ƶ�����ӦĿ¼
    ///              ���src_name��dest_name����Ŀ¼�����src_name��ȫ�����ݵݹ��ƶ���dest_name
    /// @param      src_name    Դ�ļ���Ŀ¼ȫ·����Ŀ¼����/��β
    /// @param      dest_name   Ŀ��Ŀ¼ȫ·����Ŀ¼����/��β
    /// @param      error_code  ���������Ϣ
    /// @return     0 �ɹ�
    ///              -1 ʧ��
    /// @see:        Move
    virtual int32_t MoveImpl(const char* src_name, const char* dst_name, uint32_t* error_code);


    /// @beirf      �޸��ļ�����Ŀ¼���֣����ܿ���̣�����ͬC���rename.
    /// @param      old_path_name   �ɵ��ļ���Ŀ¼ȫ·����,Ŀ¼����/��β
    /// @param      new_path_name   �µ��ļ���Ŀ¼����Ŀ¼����/��β
    /// @param      error_code      ���������Ϣ
    /// @return     0 �ɹ�
    ///              -1 ʧ��
    /// @see:        Rename
    virtual int32_t  RenameImpl(const char* old_path_name,
                                const char* new_path_name,
                                uint32_t*   error_code);

    /// @beirf      �޸��ļ�����Ŀ¼���֣����ܿ���̣�����ͬC���rename.
    /// @param      old_path_name   �ɵ��ļ���Ŀ¼ȫ·����,Ŀ¼����/��β
    /// @param      new_path_name   �µ��ļ���Ŀ¼����Ŀ¼����/��β
    /// @param      rename_option   rename������ѡ��
    /// @param      error_code      ���������Ϣ
    /// @return     0 �ɹ�
    ///              -1 ʧ��
    /// @see:        Rename
    virtual int32_t  RenameImplWithOptions(const std::string& old_path_name,
                                           const std::string& new_path_name,
                                           const RenameOptions& rename_option,
                                           uint32_t*   error_code);

    /// @beirf      ɾ���ļ�����Ŀ¼
    /// @param      path_name       ��ɾ�����ļ���Ŀ¼ȫ·����,Ŀ¼����/��β
    /// @param      is_recursive    �Ƿ�ݹ�ɾ�������ļ�����Ŀ¼, �����ļ�ʵ�ֲ�֧��.
    /// @param      error_code      ���������Ϣ
    /// @return     0 �ɹ�
    ///              -1 ʧ��
    /// @see:        Remove
    virtual int32_t RemoveImpl(const char* file_name, bool is_recursive,
                               uint32_t* error_code);


    /// @beirf      �����Ŀ¼
    /// @param      path_name   Ŀ¼ȫ·����,Ŀ¼����/��β
    /// @param      error_code  ���������Ϣ
    /// @return     0 �ɹ�
    ///              -1 ʧ��
    /// @see:        AddDir
    virtual int32_t AddDirImpl(const char* file_name, uint32_t* error_code);

    /// @beirf      ����ļ���С
    /// @param      file_name   Ŀ¼ȫ·����
    /// @param      error_code  ���������Ϣ
    /// @return     >=0 �ɹ�
    ///             -1 ʧ��
    virtual int64_t GetSizeImpl(const char* file_name, uint32_t* error_code);

    /// @beirf      ����ļ���Ŀ¼Ȩ��
    /// @param      path_name    Ҫ��õ��ļ���ȫ·����
    /// @param      permission   Ŀ��Ȩ��
    /// @param      error_code   ���������Ϣ
    /// @return     true �ɹ�(����)
    ///             false ʧ��
    virtual bool GetModeImpl(const std::string& path_name,
                             uint32_t* permission,
                             uint32_t* error_code);

    /// @beirf      �޸��ļ���Ŀ¼Ȩ��
    /// @param      path_name    Ҫ�鿴���ļ���ȫ·����
    /// @param      permission   Ŀ��Ȩ��
    /// @param      error_code   ���������Ϣ
    /// @return     true �ɹ�(����)
    ///             false ʧ��
    virtual bool ChmodImpl(const char* path_name,
                           const uint32_t permission,
                           uint32_t* error_code);

    /// @beirf      �޸��ļ���Ŀ¼��owner
    /// @param      path_name     Ҫ�޸ĵ��ļ�ȫ·����
    /// @param      role_name     user_name:group_name�����
    /// @param      error_code    ���������Ϣ
    /// @return     true �ɹ�(����)
    ///             false ʧ��
    virtual bool ChangeRoleImpl(const char* path_name,
                                const char* role_name,
                                uint32_t* error_code);

    /// @beirf      �޸��ļ���Ŀ¼�ĵڶ����(second role), ���ṩxfs��ʵ��
    ///             ע��ֻ�и��ļ���Ŀ¼��ӵ����(owner role)�����޸�xfs���ļ���second role
    /// @param      path_name        Ҫ�޸ĵ��ļ���ȫ·����
    /// @param      second_role_name ��xfs�ļ�,�˲���Ϊһ��role����;
    /// @param      is_recursive     �Ƿ�ݹ���������ļ�����Ŀ¼��second role
    /// @param      error_code       ���������Ϣ
    /// @return     true �ɹ�;false ʧ��
    virtual bool ChangeSecondRoleImpl(const char* path_name,
                                      const char* second_role_name,
                                      bool is_recursive,
                                      uint32_t* error_code);

    /// @berif      ����ָ���ļ����ݵ�ժҪ(��XFS Server�㷨һ��)
    /// @param      path_name   ��Ҫ����ժҪ���ļ�·��
    /// @param      file_size   �ļ��ĳ���
    /// @param      digest      ���ɹ�,�˲������淵�ص�ժҪֵ
    /// @param      error_code  ���������Ϣ
    /// @return     true �ɹ�;false ʧ��
    /// @berif      ���ù������û�Ӧ��֤�ļ����ݲ��ᱻ�޸�
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
