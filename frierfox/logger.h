#pragma once

#include <iostream>

#define __FILE_NAME__ (strrchr(__FILE__,'\\') ? strrchr(__FILE__,'\\') : __FILE__)

#define TRACE std::cout << __TIMESTAMP__  << " " << __FUNCTION__ << " (" << __FILE_NAME__ << ", " << __LINE__ << ") : "

#define ENDL std::endl
