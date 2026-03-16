#pragma once

#include "detail.h"
#include "util/none.h"

#include <memory>

template <typename T, typename F = None>
class Shared : public std::enable_shared_from_this<T> {
  protected:
    class Only {
        explicit Only() = default;

        friend class Shared<T, F>;
    };

  public:
    using Ptr = std::shared_ptr<T>;

    Ptr get_ptr() {
        return std::enable_shared_from_this<T>::shared_from_this();
    }

  protected:
    // Create a new shared ptr
    template <typename... Args>
    static Ptr make(Args&&... args)
        requires(detail::FirstCtorArg<T, Only, Args...>)
    {
        return std::make_shared<T>(Only{}, std::forward<Args>(args)...);
    }

    // Create a new shared ptr
    template <typename... Args>
    static Ptr make(Args&&... args)
        requires(!detail::FirstCtorArg<T, Only, Args...>)
    {
        return std::make_shared<T>(std::forward<Args>(args)...);
    }

  private:
    friend F;
};