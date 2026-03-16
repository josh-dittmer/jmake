#pragma once

#include "detail.h"
#include "util/none.h"

#include <memory>

template <typename T, typename F = None> class Unique {
  protected:
    class Only {
        explicit Only() = default;

        friend class Unique<T, F>;
    };

  public:
    using Ptr = std::unique_ptr<T>;

  protected:
    // Create a new unique ptr
    template <typename... Args>
    static Ptr make(Args&&... args)
        requires(detail::FirstCtorArg<T, Only, Args...>)
    {
        return std::make_unique<T>(Only{}, std::forward<Args>(args)...);
    }

    // Create a new unique ptr
    template <typename... Args>
    static Ptr make(Args&&... args)
        requires(!detail::FirstCtorArg<T, Only, Args...>)
    {
        return std::make_unique<T>(std::forward<Args>(args)...);
    }

  private:
    friend F;
};