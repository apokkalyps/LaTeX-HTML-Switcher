//---------- Implementation of module <Toolbox> (file Toolbox.cpp) 

//--------------------------------------------------------------- Includes

#define __STDC_WANT_LIB_EXT1__ 1

#include <chrono>
#include <cstdlib>
#include <cstring>
#include "Toolbox.hpp"

namespace Toolbox
{
	/////////////////////////////////////////////////////////////////  PRIVATE

	//-------------------------------------------------------------- Constants

	//------------------------------------------------------------------ Types

	//------------------------------------------------------- Static variables

	//------------------------------------------------------ Private functions

	//////////////////////////////////////////////////////////////////  PUBLIC
	//------------------------------------------------------- Public functions

	
	double Timethis(size_t iter, const std::function<void(void)>& func)
	{
		using namespace std::chrono;
		high_resolution_clock::time_point beginning = high_resolution_clock::now();
		for (size_t i = 0; i < iter; i++)
		{
			func();
		}
		return (duration<double>(high_resolution_clock::now() - beginning).count()) / double(iter);
	}

	wchar_t* ToWchar_t(const char* source)
	{
		size_t length = strlen(source);
		auto* destination = new wchar_t[length + 1];
		size_t retValue = 0;

#if defined _MSC_VER || (defined __STDC_LIB_EXT1__ && defined __STDC_WANT_LIB_EXT1__ && __STDC_WANT_LIB_EXT1__ == 1)
		mbstowcs_s(&retValue, destination, length + 1, source, length);
#else
		retValue = mbstowcs(destination, source, length) - length;
#endif
		
		if (!retValue) {
			delete[] destination;
			destination = nullptr;
		}
		return destination;
	}

	InCharArrayStream::InCharArrayBuffer::InCharArrayBuffer(const char* content, size_t size) :
		std::stringbuf(std::ios_base::in)
	{
		char* p = const_cast<char*>(content);
		setg(p, p, p + size);
	}

	InCharArrayStream::InCharArrayStream(const char* content, size_t size) :
		icab(content, size)
	{
		set_rdbuf(&icab);
	}
}