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
    // �첽������Ĭ�ϳ�ʱʱ�䣨��λ:s��
    static const uint32_t kDefaultAsyncTimeout = 10 * 60 * 60;

    /// @beirf       ����Open�е�flag����
    enum FILE_FLAG {
        kOpenModeRead = 0x01,
        kOpenModeWrite = 0x02,
        kOpenModeAppend = 0x04,
        kIoAsync = 0X1000,   ///< ����������ͬ��,���������첽

        // compatible old names
        ENUM_FILE_OPEN_MODE_R     = kOpenModeRead,
        ENUM_FILE_OPEN_MODE_W     = kOpenModeWrite,
        ENUM_FILE_OPEN_MODE_A     = kOpenModeAppend,
        ENUM_FILE_TYPE_RA_FILE    = 0x200,
        ENUM_FILE_IO_NON_BLOCKING = kIoAsync,
        ENUM_FILE_TYPE_CIPHER     = 0x400,
    };

    /// @beirf      ��ʼ��SDKģ��,�ڽ����г�ʼ������һ��,
    ///              ����Ҫÿ��File���󶼵���
    ///
    ///              ���ٴ���identity,�����ļ����ʵ�ʱ���ʹ���û���ĳһ��role,
    ///              Ĭ��Ϊ���û���ͬ����һ��role.
    ///              ʹ��--identity=xxx���ߵ�ǰ�Ļ����������identity
    ///
    /// @return     true, �ɹ�
    ///              false,ʧ��
    static bool     Init();

    /// @beirf      ��һ���ļ�
    /// @param      file_path   Ҫ�򿪵��ļ�ȫ·��,�����Ǳ����ļ���/xfs/�����ļ�
    ///                          ����������ʽ����ѡ��.
    ///                          file_path:k1=v1:k2=v2
    ///                          ѡ��������ļ�ʵ�ֽ���.
    ///                          �����ӵ�ѡ���봫���options�ظ�ʱ���Դ˴�Ϊ׼.
    /// @param      flags       Ҫ���ļ���ģʽ,��������ͨ�ļ����߼�¼���ļ�,
    ///                          ��дȨ�ޣ��첽��ͬ����д
    /// @param      options     ѡ������Ƿ�ʹ�ÿͻ���cache������������
    /// @param      error_code  ���ڷ��س������
    /// @return     ��ʾ������ļ�ָ��; NULL ��ʾ����; �û�����delete���ص�ָ���Ա����ڴ�й©.
    ///
    /// @note       flags����ģʽ�����壺
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
    static File*    Open(const std::string& file_path,
                         uint32_t flags,
                         const OpenFileOptions& options = OpenFileOptions(),
                         uint32_t* error_code = NULL);

    /// @beirf      ��һ���ļ���һ�ӿ�, ���������Ҫ���ɹ�
    ///              Ҫ�������������ɸ�����Ҫѡ�ã�
    /// @param      file_path   Ҫ�򿪵��ļ�ȫ·��,�����Ǳ����ļ���/xfs/�����ļ�
    /// @param      flags       Ҫ���ļ���ģʽ,��������ͨ�ļ����߼�¼���ļ�,
    ///                          ��дȨ�ޣ��첽��ͬ����д
    /// @param      options     ��д�ļ���ѡ��
    /// @return     ��ʾ������ļ�ָ��; NULL ��ʾ����
    ///
    /// @note       flags����ģʽ�����壺
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
    static File*    OpenOrDie(const std::string& file_path,
                              uint32_t flags,
                              const OpenFileOptions& options = OpenFileOptions());

    /// @beirf      ��һ���ļ���ֻ��ͬ����
    /// @param      file_path   Ҫ�򿪵��ļ�ȫ·��,�����Ǳ����ļ���/xfs/�����ļ�
    /// @param      mode        r,w,a��ʽ��
    /// @return     ��ʾ������ļ�ָ��; NULL ��ʾ����
    static File* Open(const std::string& file_path, const char* mode,
                      uint32_t* error_code = NULL);

    /// @beirf      ͬ�����ļ���ȡ����
    /// @param      buffer      ��ʾ������ݵı����ڴ�ռ䣻
    /// @param      size        ��ʾ��Ҫ��ȡ��������ݳ��ȣ�
    /// @param      error_code  ��Ŵ�����Ϣ��
    /// @return     -1 ����
    ///              0  �������ļ���β
    ///              >0 ʵ�ʶ�ȡ�����ļ�����
    virtual int64_t Read(void*     buffer,
                         int64_t   size,
                         uint32_t* error_code = NULL) = 0;

    /// @beirf      ͬ�����ļ���ȡ����
    /// @param      result      ��ʾ������ݵ�string��
    /// @return     false ����
    ///             true �ɹ�
    static bool LoadToString(const std::string& file_path, std::string* result);

    /// @brief:      ͬ�����ļ���ȡһ������, ���max_size - 1���ֽ�
    ///              ��'\0'����
    ///              '\n'��Ϊ�н�����־������'\n'���ᱻ�������ᱻ��ȡ��buffer��
    ///              �����������ֵret > 0, ��ôbuffer[ret] = '\0'��
    ///              ���0 < ret < max_size - 1�� ��ôbuffer[ret - 1] = '\n'��
    ///              ���ret = max_size - 1�� ��ôbuffer[ret - 1] = '\n'������ȡ��һ�еĽ�β��
    ///              �������buffer̫Сû�ж�ȡ��һ�еĽ�β
    /// @param      buffer      ��ʾ������ݵı����ڴ�ռ䣻
    /// @param      max_size    ��ʾ��Ҫ��ȡ��������ݳ��ȣ�
    /// @return     -1 ����
    ///              0  �������ļ���β
    ///              >0 ʵ�ʶ�ȡ�����ļ�����
    virtual int32_t ReadLine(void* buffer, int32_t max_size) = 0;

    int32_t ReadLine(std::string* result);

    /// @beirf      ͬ����ʽ��򿪵��ļ�д�����ݣ��ļ�ͨ�������ڱ���,
    ///              ����Flush��Closeʱˢ�µ������ļ�. ����ʵ�ֿ��������ԣ����������ο���ʵ�֡�
    ///
    /// @param      buffer      ָ���д�������
    /// @param      size        ��д����ļ�����
    /// @param      error_code  ��ų�����Ϣ�����سɹ�ʱ��ʵ��д����ֽ�������buf_size��
    ///
    /// @return     >=0 ʵ�ʳɹ�д��ĳ���;
    ///              -1  ʧ��
    virtual int64_t Write(const void* buffer,
                          int64_t     size,
                          uint32_t*   error_code = NULL) = 0;

    /// @beirf      ���ļ���ָ��λ���첽��ȡ����
    /// @param      buffer          ��ʾ������ݵı����ڴ�ռ䣻
    /// @param      size            ��ʾ��Ҫ��ȡ��������ݳ��ȣ�
    /// @param      start_position  ��ʾ�Ӹ�λ�ÿ�ʼ��ȡ���ݣ�
    /// @param      callback        �뵱ǰ�������Ӧ�Ļص�����
    ///                              (void:��ʾ�ص������ķ���ֵ,�������ǲ���:
    ///                              ��һ����ʾ�ɹ�����ĳ���;
    ///                              �ڶ���Ϊerrorcode)
    ///
    /// @param      timeout         ��ʱʱ��,seconds
    /// @param      error_code      ���������Ϣ
    /// @return     =0 �ɹ�
    ///              -1 ʧ��
    virtual int32_t AsyncReadFrom(void* buffer,
                                  int64_t size,
                                  int64_t start_position,
                                  Closure<void, int64_t, uint32_t>* callback,
                                  uint32_t  timeout = kDefaultAsyncTimeout,
                                  uint32_t* error_code = NULL)=0;

    /// @beirf      �첽��ʽ��򿪵��ļ�д�����ݣ��ļ�ͨ�������ڱ���,
    ///              ����Flush��Closeʱˢ�µ������ļ�
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

    /// @beirf      ����������ˢ������������������
    /// @param      error_code  ��ų�����Ϣ
    /// @return     0  �ɹ�
    ///              <0 ʧ��
    virtual int32_t Flush(uint32_t* error_code = NULL) = 0;

    /// @beirf      �ر�һ���ļ�
    /// @param      error_code  ��ų�����Ϣ
    /// @return     0 �ɹ�; -1 ʧ��
    ///
    /// @note       �ر�֮ǰ����������Flushһ��
    virtual int32_t Close(uint32_t* error_code = NULL) = 0;

    /// @beirf      �����ļ�����,�ݲ�֧�ֺ�ͨ����Ĵ�����������
    /// @param      src_file_path   Ŀ��Դ�ļ�ȫ·��
    /// @param      dest_file_path  Ŀ���ļ�ȫ·��
    /// @param      error_code      ���������Ϣ
    /// @return     0 �ɹ�
    ///              -1 ʧ��
    static int32_t  Copy(const std::string& src_file_path,
                         const std::string& dest_file_path,
                         uint32_t*      error_code = NULL);

    /// @beirf      �ƶ��ļ�����Ŀ¼
    /// @param      src_name    Դ�ļ���Ŀ¼ȫ·��
    /// @param      dest_name   Ŀ��Ŀ¼ȫ·��
    /// @param      error_code  ���������Ϣ
    /// @return     0 �ɹ�
    ///              -1 ʧ��
    static int32_t  Move(const std::string& src_name,
                         const std::string& dest_name,
                         uint32_t*   error_code = NULL);

    /// @beirf      �޸��ļ�����Ŀ¼����
    /// @param      old_path_name   �ɵ��ļ���Ŀ¼ȫ·����
    /// @param      new_path_name   �µ��ļ���Ŀ¼��
    /// @param      error_code      ���������Ϣ
    /// @return     0 �ɹ�
    ///              -1 ʧ��
    static int32_t  Rename(const std::string& old_path_name,
                           const std::string& new_path_name,
                           uint32_t*   error_code = NULL);

    /// @beirf      �޸��ļ�����Ŀ¼����
    /// @param      old_path_name   �ɵ��ļ���Ŀ¼ȫ·����
    /// @param      new_path_name   �µ��ļ���Ŀ¼��
    /// @param      rename_option   rename������ѡ��
    /// @param      error_code      ���������Ϣ
    /// @return     0 �ɹ�
    ///              -1 ʧ��
    static int32_t  Rename(const std::string& old_path_name,
                           const std::string& new_path_name,
                           const RenameOptions& rename_option,
                           uint32_t*   error_code = NULL);

    /// @beirf      ɾ���ļ�����Ŀ¼
    /// @param      path_name       ��ɾ�����ļ���Ŀ¼ȫ·����,Ŀ¼����/��β
    /// @param      is_recursive    �Ƿ�ݹ�ɾ�������ļ�����Ŀ¼,
    ///                              �����ļ�ʵ�ֲ�֧��.
    /// @param      error_code      ���������Ϣ
    /// @return     0 �ɹ�
    ///              -1 ʧ��
    ///
    /// ��ע������վ���ܣ�
    /// ���ļ�·���������":recycle=true"�����Խ��ļ��������վ������������ɾ����
    /// ����վ�ڵ��ļ���ŵ�ԭ�ļ���ͬĿ¼�£�����'~'��ʱ�����ʾ����
    /// xfs����7��֮��ɾ������վ�ڵ��ļ���
    /// 7��֮�ڣ��û�������fu ls -y����������վ�е��ļ������ҿ�����rename�ķ�ʽ�ָ��ļ���
    /// ע�⣺����վ�е��ļ���Ȼ��������û���quota.
    /// �����quota���㣬��ֱ��ɾ������վ�ڵ��ļ����ͷ�quota��
    ///
    static int32_t  Remove(const std::string& path_name,
                           bool        is_recursive = false,
                           uint32_t*   error_code = NULL);

    /// @beirf      �����Ŀ¼
    /// @param      path_name   Ŀ¼ȫ·����,Ŀ¼����/��β
    /// @param      error_code  ���������Ϣ
    /// @return     0 �ɹ�
    ///              -1 ʧ��
    static int32_t  AddDir(const std::string& file_path, uint32_t* error_code = NULL);

    /// @beirf      �ݹ������Ŀ¼
    /// @param      path_name   Ŀ¼ȫ·����,
    /// @param      error_code  ���������Ϣ
    /// @return     true �ɹ�
    ///             false ʧ��
    /// Modify from xfs/tools/file_utility/make_dir.h
    static bool AddDirRecursively(const std::string& path, uint32_t* error_code = NULL);


    /// @beirf      �鿴ָ��Ŀ¼���ļ���������Ϣ
    /// @param      pattern     Ҫ�鿴��Ŀ¼���ļ���֧��* ? [].
    /// @param      list_option ����ĺ���ο�ListOptions����.
    /// @param      buffer      �ŷ��ص�������Ϣ,Ӧ�ò����Ļ�����.
    ///                          ĳЩ�ֶο������ڻ�ȡ��Ϣʧ�ܶ�����Ϊ��Ĭ��ֵ.
    /// @param      error_code  ���������Ϣ
    /// @return     0 �ɹ�; -1 ʧ��
    static int32_t List(const std::string& pattern,
                        const ListOptions& list_option,
                        std::vector<AttrsInfo>* attrs_info,
                        uint32_t* error_code = NULL);

    /// @beirf      �鿴ָ��Ŀ¼���ļ���������Ϣ
    /// @param      name        Ҫ�鿴��Ŀ¼���ļ�����֧��* ? [].
    /// @param      mask        ��Ҫ��ȡ��������Ϣ����
    /// @param      attrs_info  ���ص�������Ϣ
    /// @param      error_code  ���������Ϣ
    /// @return     true �ɹ�; false ʧ��
    static bool  GetAttrs(const std::string& name,
                          const AttrsMask& mask,
                          AttrsInfo* attrs_info,
                          uint32_t* error_code = NULL);

    /// @beirf      �ı��ļ��ĵ�ǰƫ����
    /// @param      offset      �����origin��ƫ����
    /// @param      whence      ƫ���������λ��
    ///              ��ѡ����:
    ///                  SEEK_SET    �ļ���ʼ
    ///                  SEEK_CUR    �ļ��ĵ�ǰλ��
    ///                  SEEK_END    �ļ���β
    /// @param      error_code  ���س�����Ϣ
    /// @return     >=0 �ɹ�(���ص�ǰ�ļ���ƫ����)
    ///              -1 ����
    virtual int64_t Seek(int64_t   offset,
                         int32_t   whence,
                         uint32_t* error_code = NULL) = 0;

    /// @beirf      �鿴�ļ�����Ŀ¼ռ�õĿռ�
    /// @param      path_name   �鿴���ļ���Ŀ¼ȫ·����
    /// @param      error_code  ���������Ϣ
    /// @return     >=0 �ɹ�(�����ļ�����Ŀ¼��С)
    ///              -1 ʧ��
    static int64_t  Du(const std::string& path_name, uint32_t* error_code = NULL);

    /// @beirf      �鿴�ļ�����Ŀ¼�Ļ��ܺ��ͳ����Ϣ
    /// @param      path_name   �鿴���ļ���Ŀ¼ȫ·����
    /// @param      mask        in/out������������Ҫ��ȡ����Ϣ������.
    ///                         ���к����ó��ļ�ϵͳʵ��֧�ֵ���Ϣ������.
    /// @param      summary     ������ܵ�ͳ����Ϣ
    /// @param      error_code  ���������Ϣ
    /// @return     true �ɹ�, false ʧ��
    static bool GetContentSummary(const std::string& path_name,
                                  FileContentSummaryMask* mask,
                                  FileContentSummary* summary,
                                  uint32_t* error_code = NULL);

    /// @beirf      ���ص�ǰ�ļ���ƫ����
    /// @param      error_code   ���������Ϣ
    /// @return     >=0 �ɹ�(���ص�ǰ�ļ���ƫ����);-1 ����
    virtual int64_t Tell(uint32_t* error_code = NULL) = 0;

    /// @beirf      �����ļ���Ч��С���ض̣�
    /// @param      length      �ļ���Ч��С
    /// @param      error_code  ���������Ϣ
    /// @return     >=0 �ɹ�
    ///              -1 ����
    virtual int32_t Truncate(uint64_t length, uint32_t* error_code = NULL) = 0;


    /// @beirf      ��ȡ�������ڵ�NodeServeλ��
    /// @param      start       ���ݿ�Ŀ�ʼλ��
    /// @param      end         ���ݿ�Ľ���λ��
    /// @param      buffer      ��ȡ����λ����Ϣ
    /// @param      error_code  ����������
    /// @return     0 �ɹ�
    ///              -1 ʧ��
    virtual int32_t LocateData(uint64_t      start,
                               uint64_t      end,
                               std::vector<DataLocation>* buffer,
                               uint32_t*     error_code = NULL) = 0;

    /// @beirf      ����ļ���С
    /// @param      file_name   Ŀ¼ȫ·����
    /// @param      error_code  ���������Ϣ
    /// @return     >=0 �ɹ�
    ///              -1 ʧ��
    static int64_t  GetSize(const std::string& file_name,
                            uint32_t*   error_code = NULL);

    /// @beirf      ��ȡָ��Ŀ¼�µ��ļ�����
    /// @param      pattern      Ҫ�鿴��Ŀ¼���ļ���Ŀ¼��/��β
    /// @param      files        �ŷ��ص������ļ����У�����ҳ��.
    /// @return     0 �ɹ�; -1 ʧ��
    static int32_t GetMatchingFiles(const std::string& pattern,
                                    std::vector<std::string>* files,
                                    uint32_t* error_code = NULL,
                                    const ListOptions* list_option = NULL);

    /// @beirf      �鿴ָ���ļ�����Ŀ¼�Ƿ����
    /// @param      path         Ҫ�鿴���ļ���ȫ·������Ŀ¼��/��β
    /// @param      error_code   ���������Ϣ
    /// @return     true �ɹ�(����)
    ///              false ʧ��
    static bool IsExist(const std::string&, uint32_t* error_code = NULL);

    /// @beirf      �鿴ָ���ļ�����Ŀ¼�Ƿ����
    /// @param      path         Ҫ�鿴���ļ���ȫ·����
    /// @param      error_code   ���������Ϣ
    /// @return     true �ɹ�(����)
    ///              false ʧ��
    /// DEPRECATED_BY(IsExist)
    static bool CheckExist(const std::string& path_name, uint32_t* error_code = NULL);

    /// @beirf      ����ļ���Ŀ¼Ȩ��
    /// @param      path_name    Ҫ��õ��ļ���ȫ·����
    /// @param      permission   Ŀ��Ȩ��
    /// @param      error_code   ���������Ϣ
    /// @return     true �ɹ�(����)
    ///              false ʧ��
    static bool GetMode(const std::string& path_name,
                        uint32_t* permission,
                        uint32_t* error_code = NULL);

    /// @beirf      �޸��ļ���Ŀ¼Ȩ��
    /// @param      path_name    Ҫ�޸ĵ��ļ���ȫ·����
    /// @param      permission   Ŀ��Ȩ��
    /// @param      error_code   ���������Ϣ
    /// @return     true �ɹ�(����)
    ///              false ʧ��
    static bool Chmod(const std::string& path_name,
                      const uint32_t permission,
                      uint32_t* error_code = NULL);

    /// @beirf      �޸��ļ���Ŀ¼��role,ע��ֻ��xfs_admin�����޸�xfs���ļ���Role
    /// @param      path_name     Ҫ�޸ĵ��ļ���ȫ·����
    /// @param      role_name     xfs�ļ�,�˲���Ȩ�޸����role;
    ///                            local�ļ�, �˲���Ϊuser_name:group_name
    /// @param      error_code    ���������Ϣ
    /// @return     true �ɹ�(����)
    ///              false ʧ��
    static bool ChangeRole(const std::string& path_name,
                           const std::string& role_name,
                           uint32_t* error_code = NULL);

    /// @beirf      �޸��ļ���Ŀ¼�ĵڶ����(second role), ���ṩxfs��ʵ��
    ///             ע��ֻ�и��ļ���Ŀ¼��ӵ����(owner role)�����޸�xfs���ļ���second role
    /// @param      path_name        Ҫ�޸ĵ��ļ���ȫ·����
    /// @param      second_role_name ��xfs�ļ�,�˲���Ϊһ��role����;
    /// @param      is_recursive     �Ƿ�ݹ���������ļ�����Ŀ¼��second role
    /// @param      error_code       ���������Ϣ
    /// @return     true �ɹ�;false ʧ��
    static bool ChangeSecondRole(const std::string& path_name,
                                 const std::string& second_role_name,
                                 bool is_recursive = false,
                                 uint32_t* error_code = NULL);

    /// @berif      ����ָ���ļ����ݵ�ժҪ(��XFS Server�㷨һ��)
    /// @param      path_name   ��Ҫ����ժҪ���ļ�·��
    /// @param      digest      ���ɹ�,�˲������淵�ص�ժҪֵ
    /// @param      error_code  ���������Ϣ
    /// @return     true �ɹ�;false ʧ��
    /// @berif       ���ù������û�Ӧ��֤�ļ����ݲ��ᱻ�޸�
    static bool GetDigest(const std::string& path_name,
                          uint32_t* digest,
                          uint32_t* error_code = NULL);

    /// @brief      ��ȡ�����������
    /// @param      error_code     ������
    /// @return     ��������
    static const char* GetErrorCodeString(uint32_t error_code);

    /// @beirf      ����SDKģ��
    /// @return     void
    static void CleanUp();


    virtual std::string GetFileImplName() = 0;

    /// @beirf      Get the prefix of a file name. The prefix must start with
    ///              a segement char '/'.
    ///              The prefix is the registered file system name.
    /// @param      file_path   the test file path
    /// @return     the prefix of a file name, or an empty string if fails.
    static std::string GetFilePrefix(const std::string& file_path);

    /// @brief       ��ָ�����ļ�����snapshot
    /// @param       source_path    Դ�ļ���Ŀ¼ȫ·��
    /// @param       target_path    Ŀ��ȫ·��
    /// @param       error_code  ���������Ϣ
    /// @return      0 �ɹ�
    ///              -1 ʧ��
    /// ����ʧ�ܵ�ԭ��
    ///               target_path���ڻ򲻿�д
    ///               source_path�����ڻ򲻿ɶ�
    ///               quota����
    static int32_t Snapshot(const std::string& source_path,
                            const std::string& target_path,
                            uint32_t* error_code = NULL);

    /// @brief       ��ָ�����ļ�����Freeze�������ɹ�֮�󣬸��ļ����ܱ�׷��д
    /// @param       file_name  ����freeze�������ļ�������֧��ͨ��������Ҳ�����Ŀ¼
    /// @param       error_code ���������Ϣ
    /// @return      true �ɹ�
    ///              falseʧ��
    static bool Freeze(const std::string& file_name, uint32_t* error_code = NULL);

    /// @brief       �޸�ָ�����ļ��ı������ӣ�����ֻ֧�����ӱ���������
    /// @param       file_name  ���в������ļ�������֧��ͨ��������Ҳ�����Ŀ¼
    /// @param       error_code ���������Ϣ
    /// @return      true �ɹ�
    ///              falseʧ��
    static bool SetReplication(const std::string& file_name,
                               int32_t new_backup_factor,
                               uint32_t* error_code = NULL);

    virtual ~File() {}

protected:
    /// @brief��     Create a File object for the file implementation.
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

    /// @beirf      ��ʼ��SDKģ��,�ڽ����г�ʼ������һ��,
    ///              ����Ҫÿ��File���󶼵���
    ///
    /// @return     true, �ɹ�
    ///              false,ʧ��
    /// @see:        Init
    virtual bool InitImpl() = 0;

    /// @beirf      ����SDKģ��
    /// @return     void
    /// @see:        Cleanup
    virtual bool CleanupImpl() = 0;

    ///
    /// @beirf      ��һ���ļ�
    /// @param      file_path   Ҫ�򿪵��ļ�ȫ·��,�����Ǳ����ļ���/xfs/�����ļ�
    ///                          ����������ʽ����ѡ��.
    ///                          file_path:k1=v1:k2=v2
    ///                          ѡ��������ļ�ʵ�ֽ���.
    ///                          �����ӵ�ѡ���봫���options�ظ�ʱ���Դ˴�Ϊ׼.
    /// @param      flags       Ҫ���ļ���ģʽ,��������ͨ�ļ����߼�¼���ļ�,
    ///                          ��дȨ�ޣ��첽��ͬ����д
    /// @param      options     ѡ������Ƿ�ʹ�ÿͻ���cache������������
    /// @param      error_code  ���ڷ��س������
    /// @return     ��ʾ������ļ�ָ��; NULL ��ʾ����
    ///
    /// @note       flags����ģʽ�����壺
    /// W:           ��������ڻὨ���ļ�;����ļ����ڣ������֮ǰ�ļ�ɾ����
    ///              ����һ���µ��ļ�;�ļ�ָ�����ļ�ͷ;
    ///
    /// A:           ����ļ�����,���ļ�β׷��;��������,����һ���ļ�,
    ///              �ļ���дָ�������;
    ///
    /// R:           ֻ�ܴ��Ѵ��ڵ��ļ�,�ļ�ָ�����ļ�ͷ;
    /// R|W:         �ļ�������ʱ,���½��ļ�,�ļ�ָ����ͷ��;�ļ�����ʱ��
    ///              ��д�ļ�������Զ��ƶ����ļ�β;
    ///
    /// W|A:         ������;
    /// R|A:         ��RW��ͬ;
    /// R|W|A:       ��RW��ͬ;
    /// @see:        Open
    virtual bool OpenImpl(const char *file_path, uint32_t flags,
                        const OpenFileOptions& options,
                        uint32_t *error_code) = 0;

    /// @beirf      �ƶ��ļ�����Ŀ¼
    /// @param      src_name    Դ�ļ���Ŀ¼ȫ·����Ŀ¼����/��β
    /// @param      dest_name   Ŀ��Ŀ¼ȫ·����Ŀ¼����/��β
    /// @param      error_code  ���������Ϣ
    /// @return     0 �ɹ�
    ///              -1 ʧ��
    /// @see:        Move
    virtual int32_t MoveImpl(const char* src_name,
                        const char* dst_name, uint32_t* error_code) = 0;


    /// @beirf      �޸��ļ�����Ŀ¼����
    /// @param      old_path_name   �ɵ��ļ���Ŀ¼ȫ·����,Ŀ¼����/��β
    /// @param      new_path_name   �µ��ļ���Ŀ¼����Ŀ¼����/��β
    /// @param      error_code      ���������Ϣ
    /// @return     0 �ɹ�
    ///              -1 ʧ��
    /// @see:        Rename
    virtual int32_t  RenameImpl(const char* old_path_name,
                                const char* new_path_name,
                                uint32_t*   error_code) = 0;

    /// @beirf      �޸��ļ�����Ŀ¼����
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
    /// @param      is_recursive    �Ƿ�ݹ�ɾ�������ļ�����Ŀ¼.
    ///                              �����ļ�ʵ�ֲ�֧��.
    /// @param      error_code      ���������Ϣ
    /// @return     0 �ɹ�
    ///              -1 ʧ��
    /// @see:        Remove
    virtual int32_t  RemoveImpl(const char* path_name,
                                bool        is_recursive,
                                uint32_t*   error_code) = 0;

    /// @beirf      �����Ŀ¼
    /// @param      path_name   Ŀ¼ȫ·����,Ŀ¼����/��β
    /// @param      error_code  ���������Ϣ
    /// @return     0 �ɹ�
    ///              -1 ʧ��
    /// @see:        AddDir
    virtual int32_t  AddDirImpl(const char* path_name,
                                uint32_t* error_code) = 0;


    /// @beirf      �鿴ָ��Ŀ¼���ļ���������Ϣ
    /// @param      pattern     Ҫ�鿴��Ŀ¼���ļ���Ŀ¼��/��β��֧��* ? []
    /// @param      list_option List������ѡ��,����AttrMask��last_file,count����������
    ///                         ����ĺ���ο�ListOptions����
    /// @param      attrs      �ŷ��ص�������Ϣ,Ӧ�ò����Ļ�����
    /// @param      error_code  ���������Ϣ
    /// @return     0 �ɹ�; -1 ʧ��
    /// @see:        List
    virtual int32_t ListImpl(const char* pattern,
                             const ListOptions& list_option,
                             std::vector<AttrsInfo>* attrs,
                             uint32_t* error_code) = 0;

    /// @beirf      �鿴ָ��Ŀ¼���ļ���������Ϣ
    /// @param      name        Ҫ�鿴��Ŀ¼���ļ�����֧��* ? [].
    /// @param      mask        ��Ҫ��ȡ��������Ϣ����
    /// @param      attrs_info  ���ص�������Ϣ
    /// @param      error_code  ���������Ϣ
    /// @return     true �ɹ�; false ʧ��
    virtual bool  GetAttrsImpl(const std::string& name,
                               const AttrsMask& mask,
                               AttrsInfo* attrs_info,
                               uint32_t* error_code = NULL);

    /// @beirf      �鿴�ļ�����Ŀ¼ռ�õĿռ�
    /// @param      path_name   �鿴���ļ���Ŀ¼ȫ·������Ŀ¼����/��β
    /// @param      error_code  ���������Ϣ
    /// @return     >=0 �ɹ�(�����ļ�����Ŀ¼��С)
    ///              -1 ʧ��
    /// @see:        Du
    virtual int64_t  DuImpl(const char* path_name,
                        uint32_t* error_code) = 0;

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
        uint32_t* error_code) = 0;

    /// @beirf      ����ļ���С
    /// @param      file_name   Ŀ¼ȫ·����
    /// @param      error_code  ���������Ϣ
    /// @return     >=0 �ɹ�
    ///              -1 ʧ��
    /// @see:        GetSize
    virtual int64_t  GetSizeImpl(const char* file_name,
                        uint32_t*   error_code) = 0;

    /// @beirf      �鿴ָ���ļ�����Ŀ¼�Ƿ����
    /// @param      path_name    Ҫ�鿴���ļ���ȫ·������Ŀ¼��/��β
    /// @param      error_code   ���������Ϣ
    /// @return     true �ɹ�(����)
    ///              false ʧ��
    /// @see:        CheckExist
    virtual bool CheckExistImpl(const char* path_name,
                        uint32_t* error_code) = 0;

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
    /// @param      path_name    Ҫ�޸ĵ��ļ���ȫ·����
    /// @param      permission   Ŀ��Ȩ��
    /// @param      error_code   ���������Ϣ
    /// @return     true �ɹ�(����)
    ///             false ʧ��
    virtual bool ChmodImpl(const char* path_name,
                           const uint32_t permission,
                           uint32_t* error_code) = 0;

    /// @beirf      �޸��ļ���Ŀ¼��role,ע��ֻ��xfs_admin�����޸�xfs���ļ���Role
    /// @param      path_name     Ҫ�޸ĵ��ļ���ȫ·����
    /// @param      role_name     ��xfs�ļ�,�˲���Ϊһ��role����;
    ///                            ��local�ļ�, �˲���Ϊuser_name:group_name
    /// @param      error_code    ���������Ϣ
    /// @return     true �ɹ�;false ʧ��
    virtual bool ChangeRoleImpl(const char* path_name,
                                const char* role_name,
                                uint32_t* error_code) = 0;

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
    /// @param      file_obj    �ļ����
    /// @param      file_size   �ļ��ĳ���
    /// @param      digest      ���ɹ�,�˲������淵�ص�ժҪֵ
    /// @param      error_code  ���������Ϣ
    /// @return     true �ɹ�;false ʧ��
    virtual bool GetDigestImpl(File* file_obj,
                               int64_t   file_size,
                               uint32_t* digest,
                               uint32_t* error_code) = 0;

    /// @brief       ��ָ�����ļ�����snapshot
    /// @param       source_path    Դ�ļ���Ŀ¼ȫ·��
    /// @param       target_path    Ŀ��ȫ·��
    /// @param       error_code     ���������Ϣ
    /// @return      0 �ɹ�
    ///              -1 ʧ��
    /// TODO(bradzhang): Ϊ�˼������д��룬�˴���ʹ�ô��麯��
    virtual int32_t SnapshotImpl(const char* source_path,
                                 const char* target_path,
                                 uint32_t* error_code);

    /// @brief       ��ָ�����ļ�����Freeze�������ɹ�֮�󣬸��ļ����ܱ�׷��д
    /// @param       file_name  ����freeze�������ļ�������֧��ͨ��������Ҳ�����Ŀ¼
    /// @param       error_code ���������Ϣ
    /// @return      true �ɹ�
    ///              falseʧ��
    virtual bool FreezeImpl(const std::string& file_name, uint32_t* error_code);

    /// @brief       �޸�ָ�����ļ��ı������ӣ�����ֻ֧�����ӱ���������
    /// @param       file_name  ���в������ļ�������֧��ͨ��������Ҳ�����Ŀ¼
    /// @param       error_code ���������Ϣ
    /// @return      true �ɹ�
    ///              falseʧ��
    virtual bool SetReplicationImpl(const std::string& file_name,
                                    int32_t new_backup_factor,
                                    uint32_t* error_code);


    /// A helper functions to set error code when necessary.
    static void SetErrorCode(uint32_t* error_code, FILE_ERROR_CODE code) {
        if (error_code) {
            *error_code = code;
        }
    }

    // ���캯��
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
