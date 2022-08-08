#ifndef BAIDU_FCR_MODEL_ABACUS_DNN_COMPONENT_H
#define BAIDU_FCR_MODEL_ABACUS_DNN_COMPONENT_H

#include <memory>
#include "vlps/common/all_third.h"
#include "vlps/common/virtual_object.h"
#include "vlps/common/factory.h"
#include "vlps/common/config.h"

namespace abacus_dnn {

class Component : public abacus::VirtualObject, public std::enable_shared_from_this<Component> {
public:
    ~Component() {
    }
    template<class T = Component>
    std::shared_ptr<T> shared_from_this() {
        std::shared_ptr<T> res = std::dynamic_pointer_cast<T>
                            (std::enable_shared_from_this<Component>::shared_from_this());
        CHECK(res);
        return std::move(res);
    }
    void set_name(const std::string& name) {
        _name = name;
    }
    const std::string& get_name() {
        return _name;
    }
private:
    std::string _name = "";
};

inline abacus::Factory<Component>& global_component_factory() {
    static abacus::Factory<Component> f;
    return f;
}

}
#endif //BAIDU_FCR_MODEL_ABACUS_DNN_COMPONENT_H
