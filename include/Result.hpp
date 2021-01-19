#pragma once

#include "ResultTypes.hpp"
#include <tuple>
#include <string>
#include <iostream>

namespace sbb
{

    struct Status
    {

        ResultType type;
        std::string message;

        bool operator!() const
        {
            return (type != RESULT_OK);
        }
        operator bool() const
        {
            return (type == RESULT_OK);
        }
    };

    template <typename T>
    using Result = std::tuple<T, const Status>;
} // namespace sbb
