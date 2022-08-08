//
// Created by 黄伟锋 on 2019-04-22.
//

#include <algorithm>
#include <chrono>
#include <iostream>
#include <openssl/hmac.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <utility>

#include "thirdparty/monitor_sdk_common/base/scoped_ptr.h"
#include "thirdparty/monitor_sdk_common/base/string/format/print.h"
#include "thirdparty/monitor_sdk_common/librmq/rmq_constants.h"
#include "thirdparty/monitor_sdk_common/librmq/utils.h"
#include "thirdparty/monitor_sdk_common/storage/file/file.h"
#include "thirdparty/monitor_sdk_common/storage/path/path.h"

#include "thirdparty/gflags/gflags.h"

using namespace common;


DEFINE_string(rmq_tmp_dir, "/tmp", "dir for the temp file that is used for saving local config");


bool RmqConnectionConfigUtils::SaveLocalCache(const std::string &topic_key, const std::string &connection_config_str) {
  std::string filename = RmqConnectionConfigUtils::get_cache_filename(topic_key);
  std::string file_path = Path::Join(FLAGS_rmq_tmp_dir, filename);
  scoped_ptr<File> fp(File::Open(file_path, "w"));
  if (!fp->Write(connection_config_str.c_str(), connection_config_str.size())) return false;
  return fp->Flush();
}


bool RmqConnectionConfigUtils::LoadLocalCache(const std::string &topic_key, std::string *ret) {
  std::string filename = RmqConnectionConfigUtils::get_cache_filename(topic_key);
  std::string file_path = Path::Join(FLAGS_rmq_tmp_dir, filename);
  return File::ReadAll(file_path, ret);
}


std::string RmqConnectionConfigUtils::get_cache_filename(const std::string &topic_key) {
  return StringPrint("vivo-rmq-%s.cache", topic_key);
}


bool RmqSignatureUtils::GenerateSignature(
    const std::string &app_name,
    const std::string &topic_key,
    const std::string &idc,
    const std::string &timestamp,
    const std::string &nonce,
    const std::string &secret_key,
    std::string *ret) {

  std::string original_signature = StringPrint("%s_%s_%s_%s_%s_%s", app_name, topic_key, idc, timestamp, timestamp, nonce);
  Hamcsha1(original_signature, secret_key, ret);

  return true;
}


bool RmqSignatureUtils::Hamcsha1(const std::string &data_str, const std::string &secret_key, std::string *ret) {

  // The key to hash
  char *key = new char [secret_key.length()+1];
  strcpy(key, secret_key.c_str());

  // The data that we're going to hash using HMAC
  char *data = new char [data_str.length()+1];
  strcpy(data, data_str.c_str());

  unsigned char digest[EVP_MAX_MD_SIZE] = {'\0'};
  unsigned int digest_len = 0;

  // Using sha1 hash engine here.
  // You may use other hash engines. e.g EVP_md5(), EVP_sha224, EVP_sha512, etc
  HMAC(EVP_sha1(), key, strlen(key), (unsigned char*)data, strlen(data), digest, &digest_len);

  // Be careful of the length of string with the choosen hash engine. SHA1 produces a 20-byte hash value which rendered as 40 characters.
  // Change the length accordingly with your choosen hash engine
  char mdString[41] = {'\0'};
  for(int i = 0; i < 20; i++)
    sprintf(&mdString[i*2], "%02x", (unsigned int)digest[i]);

  std::string hash_str(mdString);

  delete key;
  delete data;

  *ret = hash_str;

  return true;
}


void RmqLogUtils::LogInfo(const std::string &msg) {
//  std::cout << msg << std::endl;
}


long RmqTimeUtils::GetCurrentTimeMillis() {
  using namespace std::chrono;
  milliseconds ms = duration_cast<milliseconds>(
      system_clock::now().time_since_epoch()
  );
  return long(ms.count());
}


std::string RmqHttpUtils::user_agent() {
  return StringPrint("%s/%s", RmqConstants::MODULE_NAME, RmqConstants::VERSION);
}

std::string RmqIpUtils::get_host_name() {
    char hostname[2048] = { 0 };
    if (gethostname(hostname, sizeof(hostname))) {
        printf("gethostname fail. %s\r\n", strerror(errno));
        return "";
    }
    return hostname;
}
