//! \note Snyggt interface!
//!
//! \note Tips när ni använder doxygen: Ni måste börja med två asterisker, alltså /**, inte /*.
//!       Därmed blev inte era kommentarer highlighted. Jag löser det åt er här.
#pragma once

namespace ml
{
namespace lin_reg
{
class Interface
{
public:
    /** @brief Delete the linear regression algorithm. */
    virtual ~Interface() noexcept = default;

    /** 
     * @brief Predict based on the given input.
     *
     * @param[in] input The input for which to predict.
     *
     * @return The predicted value.
     */
    virtual double predict(double input) const = 0;
};
} // namespace lin_reg
} //namespace ml