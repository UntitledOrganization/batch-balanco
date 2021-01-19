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

        bool operator!() const
        {
            return (type != RESULT_OK);
        }

        operator bool() const
        {
            return (type == RESULT_OK);
        }

        Status &operator=(const Status &rhs)
        {
            return {rhs.type, rhs.message};
        }
    };

    template <typename T>
    using Result = std::tuple<T, const Status>;
} // namespace sbb
