#ifndef SMARTPOINTER_H
#define SMARTPOINTER_H

#include <memory>

template<typename Type>
using sp = std::shared_ptr<Type>;

template<typename _Tp, typename... _Args>
sp<_Tp> make_sp(_Args... __args) {
  return std::make_shared<_Tp>(std::forward<_Args>(__args)...);
}

#endif

