#pragma once

#include "ResultTypes.hpp"
#include <tuple>
#include <string>
#include <iostream>

namespace sbb 
{
    struct Status
    {
        const ResultType type;
        const std::string message;
        bool operator!() const {
            return (type != SBB_RESULT_OK);
        }
    };

    template <typename T>
    using Result = std::tuple<T, const Status>;
}