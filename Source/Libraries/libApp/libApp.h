#ifndef LIBAPP_H_
#define LIBAPP_H_
#pragma once

namespace Elf
{
	struct libApp
	{
		static void Initialize(int ac, char** av);
	};

	extern int FirestormRun();
}

#endif
