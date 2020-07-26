#pragma once

#include <iostream>

#if defined(SBB_M_DEBUG)
    #define SBB_ERROR(x) std::cerr << "Error: "<< __FILE__ << ":" << __LINE__ << " " << __func__ << " [" << x << "]" << std::endl;
    #define SBB_WARNING(x) std::cerr << "Warning: "<< __FILE__ << ":" << __LINE__ << " " << __func__ << " [" << x << "]" << std::endl;
    #define SBB_DEBUG(x) std::cout << "Debug: " << __FILE__ << ":" << __LINE__ << " " << __func__ << " [" << x << "]" << std::endl;
#else
    #define SBB_ERROR(x)
    #define SBB_WARNING(x)
    #define SBB_DEBUG(x)
#endif
