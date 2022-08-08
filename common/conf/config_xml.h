
#pragma once

#include <iostream>
#include <string>

#include "boost/lexical_cast.hpp"
#include "glog/logging.h"
#include "tinyxml2.h"
namespace common {

class ConfigXml {
 public:
  explicit ConfigXml() : _node(NULL) {}
  explicit ConfigXml(const std::string& path) { init(path); }
  explicit ConfigXml(const ConfigXml& para) { _node = para._node; }
  explicit ConfigXml(ConfigXml&& para) {
    _node = para._node;
    para._node = NULL;
  }

  void init(const std::string& path, int id_default = 0);
  const tinyxml2::XMLElement* Node() const;
  bool Child(const std::string& name, ConfigXml& out) const;
  template <class T>
  bool Attr(const std::string& name, T& out) const {
    const tinyxml2::XMLAttribute* a = _node->FindAttribute(name.c_str());
    if (a) {
      out = boost::lexical_cast<T>(a->Value());
      return true;
    }
    return false;
  }
  template <class T>
  bool Attr(const std::string& name, T& out, const T& default_value) const {
    bool ret = Attr(name, out);
    if (!ret) {
      out = default_value;
    }
    return ret;
  }

  bool Next(ConfigXml& out) const;
  bool Next(const std::string& name, ConfigXml& out) const;
  bool Find(const std::string& name, ConfigXml& out) const;
  bool Name(std::string& out) const;

 private:
  tinyxml2::XMLDocument xml;  //所有_node及其子_node归这个管
  tinyxml2::XMLElement* _node;
};
}  // namespace common
