

#ifndef LIBCORE_H_
#define LIBCORE_H_
#pragma once

#ifdef LIB_CORE
#error "libCore.h already included"
#endif

#define LIB_CORE

#define OPEN_NAMESPACE(ns) namespace ns { extern int ns_##ns
#define CLOSE_NAMESPACE(ns) } // namespace ns

#include <vector>
#include <memory>
#include <string>
#include <list>
#include <unordered_map>
#include <unordered_set>
#include <iostream>
#include <assert.h>
#include <functional>

OPEN_NAMESPACE(Elf);

using std::cout;
using std::endl;

using std::function;
using std::pair;

using std::vector;
using std::string;
using std::list;
using std::unordered_map;
using std::unordered_set;

using std::shared_ptr;
using std::weak_ptr;

CLOSE_NAMESPACE(Elf);

#endif
