//! \note Snyggt jobbat! Er linReg-modell är korrekt implenterad och tränas med statiska parametrar
//!       (learning rate samt antalet epoker). Ett förbättringsförslag som ni inte behöver göra,
//!       men som jag hade föreslagit om denna kod skulle användas under en längre tid i industrin:
//!       Modellen hade kunnat utvecklas för att kunna tränas adaptivt, dvs. modellen justerar
//!       lärhastigheten utefter hur träningen går och träning sker tills modellen predikterar med
//!       väldigt hög precision.

#include "ml/lin_reg/lin_reg.h"

namespace ml
{
namespace lin_reg
{   
namespace 
{
constexpr size_t min(const size_t x, const size_t y) noexcept
{
    return x <= y ? x : y;
}
} // namespace

LinReg::LinReg(const Vector<double>& trainInput, const Vector<double>& trainOutput) noexcept
    : myTrainInput{trainInput}
    , myTrainOutput{trainOutput}
    , myTrainSetCount{min(trainInput.size(), trainOutput.size())}
    , myWeight{0.5}
    , myBias{0.5}
{}

//! \note Denna metod kan markeras noexcept, den kommer garanterat inte generera några undantag.
//!       Särskilt inte i denna miljö, där untantag är inaktiverat som default, då det är
//!       tämligen resurskrävande.
double LinReg::predict(double input) const
{
    /*y = kx + m*/
    return myWeight * input + myBias;
}

bool LinReg::train(unsigned int epochCount, double learningRate)
{
    //! \note Det blev väldigt tydligt när ni lade respektive check i var sin if-sats. 
    //!       Här rekommenderar jag dock starkt Yoda notation, dvs. if (0U == epochCount),
    //!       särskilt då epochCount och learningRate är variabler; hade ni i stället råkat skriva
    //!       if (0U = epochCount) hade koden rullat på, samtidigt som epochCount hade satts till 0.
    //!       Hade ni gjort detta misstag med Yoda notation hade ni fått ett kompileringsfel i stället
    //!       => vi fångar då misstaget tidigt.
    if (myTrainSetCount == 0)    return false;
    if (epochCount == 0)         return false;
    if (learningRate <= 0.0)     return false;
    
    //! \note Initiera helst alla typer med den universiella initieraren {} i stället för att blanda.
    //!       Det går givetvis hur bra som helst med = för numeriska typer, men det är lättare att
    //!       att bara köra på ett sätt. Jag kommer inte nämnda detta igen, men det gäller resten
    //!       av koden också.
    for (unsigned int epoch = 0; epoch < epochCount; epoch++)
    {
        double gradW = 0.0;
        double gradB = 0.0;
        
        for(size_t i{0U}; i < myTrainSetCount; i++)
        {
            const double x   = myTrainInput[i];
            const double y   = myTrainOutput[i];
            const double yref= myWeight * x + myBias;
            const double e   = yref - y;
            
            gradW += e * x;
            gradB += e;
        }
        //! \note Snyggt att ni beräknar gradienterna (eller "felen" som jag när jag undervisar 
        //!       brukar kalla det för att hålla språket mer basic) för hela epoken innan
        //!       optimeringen genomförs.
        const double scale = 2.0 / static_cast<double>(myTrainSetCount);
        gradW *= scale;
        gradB *= scale;
        
        myWeight -= learningRate * gradW;
        myBias   -= learningRate * gradB;
    }
    return true;
}
} //end lin_reg 
}//end ml