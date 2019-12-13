//
// Created by enijenhuis on 11-12-2019.
//

#ifndef SIMPLELUA_VALUE_HPP
#define SIMPLELUA_VALUE_HPP

#define val_eq(val, type) (val.getType() == typeid(type*))

class AbstractValue {
protected:
    const std::type_info &type;
    void *data;
    AbstractValue(const std::type_info &type, void *data) : type(type), data(data) {}
public:
    template<typename T>
    const T& to() const {
        return *static_cast<T*>(data);
    }
    
    template<typename T>
    bool is() const {
        return type == typeid(T*);
    }

    const std::type_info &getType() {
        return type;
    }
};

template<typename T>
class Value : public AbstractValue {
private:
    bool referenced;
public:
    explicit Value(T *val) : AbstractValue(typeid(T *), val), referenced(true) {}

    explicit Value(const T &val) : AbstractValue(typeid(T *), new T(val)), referenced(false) {}

    explicit Value(T *val, bool referenced) : AbstractValue(typeid(T*), val), referenced(referenced) {}

    ~Value() {
        if (!referenced)
            delete (T *) data;
    }

    T& val() {
        return *static_cast<T*>(data);
    }
};

#endif //SIMPLELUA_VALUE_HPP
