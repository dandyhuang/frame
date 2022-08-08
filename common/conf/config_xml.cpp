#include "common/conf/config_xml.h"

namespace common {

void ConfigXml::init(const std::string& path, int id_default) {
  tinyxml2::XMLError err;
  if (id_default) {
    err = xml.Parse(path.c_str());
  } else {
    err = xml.LoadFile(path.c_str());
  }

  CHECK(err == tinyxml2::XML_SUCCESS);
  _node = xml.RootElement();
}

const tinyxml2::XMLElement* ConfigXml::Node() const { return _node; }

bool ConfigXml::Child(const std::string& name, ConfigXml& out) const {
  tinyxml2::XMLElement* ret = _node->FirstChildElement(name.c_str());
  if (ret) {
    out._node = ret;
    return true;
  }
  return false;
}

bool ConfigXml::Next(ConfigXml& out) const {
  tinyxml2::XMLElement* ret = _node->NextSiblingElement();
  if (ret) {
    out._node = ret;
    return true;
  }
  return false;
}

bool ConfigXml::Next(const std::string& name, ConfigXml& out) const {
  tinyxml2::XMLElement* ret = _node->NextSiblingElement(name.c_str());
  if (ret) {
    out._node = ret;
    return true;
  }
  return false;
}

bool ConfigXml::Find(const std::string& name, ConfigXml& conf) const {
  if (_node->Name() == name) {
    conf._node = _node;
    return true;
  }
  return Next(name, conf);
}

bool ConfigXml::Name(std::string& out) const {
  out = _node->Name();
  return true;
}

}  // namespace vvframe
