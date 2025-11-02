//! \note Bra jobbat! Jag lade till några kommentarer om noexcept, annars ser det bra ut. Visst
//!       hade kommenterna kunnat skrivas i enlighet med Doxygen, men de är tydliga och beskrivande
//!       som de redan är! Jag lade också ned måsvingarna på separata rader för att hålla koden
//!       konsekvent med resterande kodbas. Jag kommer inte nämna det igen, men jag har också
//!       gjort detta i resterande filer.
#pragma once 

#include "container/vector.h"
#include "ml/lin_reg/interface.h"

/**Declaration of LinReg, */
namespace ml
{
namespace lin_reg
{
//! \note Som jag kommer ihåg att vi pratade om, så är det normalt inte rekommenderat att använda
//!       using-direktiv i headerfiler. Men, lägger ni den i ett annat namespace, som ni har gjort
//!       här, så är det fine; eran förkortning gäller då bara i ml::lin_reg och i detta nästlade
//!       namespace är det ju bara ni som lägger till linReg-modeller => fine. Hade ni lagt detta
//!       direktiv i det globala namespacet (utanför era namespace) däremot, hade denna förkortning
//!       gäller överallt. 
//!
//!       Jag skulle till och med säga att jag tycker att detta var en bra lösning. Det är ganska 
//!       normalt att göra liknande via alias, vilket implementeras via using-direktiv utan
//!       nyckelordet namespace; jag hade skrivit using Vector == container::Vector för att skapa
//!       aliaset Vector inuti detta namespace. Denna hade då varit lika med container::Vector.
//!       Effekten hade blivit samma som er förkortning dock, men det jag skrev är "vanligare".
using namespace container;

class LinReg final : public Interface
{
public:
    /*
    creat a model for training data.
    trainInput reference to a const vector with input data (x)
    trainOutput reference to a const vector with output data (y)
    */
    explicit LinReg(const Vector<double>& trainInput, const Vector<double>& trainOutput) noexcept;

    /*virtual destructor that saves over the interface destructor*/
    ~LinReg() noexcept override = default;

    /*make a prediction */
    //! \note Denna metod kan/bör markeras noexcept.
    double predict(double input) const override;

    /*trains the model in a set number of epochs with a learning rate of 1%*/
    //! \note Denna metod kan/bör markeras noexcept.
    bool train(unsigned int epochCount, double learningRate = 0.01);

    //! \note Snyggt!
    LinReg() = delete;                          // no default-constructor
    LinReg(const LinReg&) = delete;             // no copying(constructor)
    LinReg& operator=(const LinReg&) = delete;  // no copying(allocation)
    LinReg(LinReg&&) = delete;                  // no moving the (constructor)    
    LinReg& operator=(LinReg&&) = delete;       // no moving the(allocation)

private:
    /** Training data. */
    const Vector<double>& myTrainInput;
    const Vector<double>& myTrainOutput;

    //! \note Lade till en kommentar för denna.
    /** The number of complete training sets. */
    const size_t myTrainSetCount;

    /** Bias = m and weight = k value i y= kx + m. */
    double myWeight;
    double myBias;
};
//! \note Fixade indenteringen här.
}// end lin_reg
} //end ml 