
#ifndef LIBMIRROR_H_
#define LIBMIRROR_H_
#pragma once

#ifdef LIB_MIRROR
#error "libMirror.h already included"
#endif

#define LIB_MIRROR

#include <libCore.h>

#include <rttr/registration>
#include <rttr/type>

OPEN_NAMESPACE(Elf);
OPEN_NAMESPACE(Mirror);

typedef rttr::type         Type;
typedef rttr::registration Registration;

CLOSE_NAMESPACE(Mirror);
CLOSE_NAMESPACE(Elf);

#endif
