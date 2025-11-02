//! \note Bra jobbat här!

//! \note Denna kommentar är out of date, detta är inte längre ett device driver demo. :)
/**
 * @brief Demonstration of GPIO device drivers in C++:
 * 
 *        The following devices are used:
 *            - A button connected to pin 13 on the device toggles a timer.
 *            - The aforementioned timer toggles an LED every 100 ms when enabled.
 *            - Another timer reduces the effect of contact bounces after pushing the button.
 *            - A watchdog timer is used to restart the program if it gets stuck somewhere.
 *            - An EEPROM stream is used to store the LED state. On startup, this value is read;
 *              if the last stored state before power down was "on," the LED will automatically blink.
 */
//! \note Bra sorterat av headerfilerna!
#include "container/vector.h"
#include "driver/atmega328p/adc.h"
#include "driver/atmega328p/eeprom.h"
#include "driver/atmega328p/gpio.h"
#include "driver/atmega328p/serial.h"
#include "driver/atmega328p/timer.h"
#include "driver/atmega328p/watchdog.h"
#include "ml/lin_reg/lin_reg.h"
#include "target/system.h"

//! \note Snyggt att ni lägger using-direktiven på fil-nivå i .cpp-filerna. Då kan ni förutom
//!       att slippa skriva driver::atmega328p::Led osv. kontrollera att ni inte får namnkrockar.
//!       Detta hade exempelvis kunnat ske om ni använder ett using-direktiv för ett annat namespace
//!       X som också innehåller en klass döpt Led och så vidare. Ligger ett using-direktiv i en
//!       headerfil ökar chansen för detta kraftigt, då using-direktiven då inte är "kontrollerade"
//!       och dessa oftast inkluderas på många ställen.
using namespace container;
using namespace driver::atmega328p;

namespace
{
/** Pointer to the system implementation. */
target::System* mySys{nullptr};

/**
 * @brief Callback for the button.
 */
void buttonCallback() noexcept { mySys->handleButtonInterrupt(); }

/**
 * @brief Callback for the debounce timer.
 * 
 *        This callback is invoked whenever the debounce timer elapses.
 */
void debounceTimerCallback() noexcept { mySys->handleDebounceTimerInterrupt(); }

/**
 * @brief Callback for the toggle timer.
 * 
 *        This callback is invoked whenever the toggle timer elapses.
 */
void predictTimerCallback() noexcept { mySys->handlePredictTimerInterrupt(); }

constexpr int round(const double number)
{
    // Round to the nearest integer - 2.7 is rounded to 3, 2.2 is rounded to 2, 
    // and -2.4 is rounded to 2.

    // number = 2.7 => we add 0.5 => 3.2. Then 3.2 is converted to int, i.e. the decimal part
    // truncated => the result is 3.
    return 0.0 <= number ? static_cast<int>(number + 0.5) : static_cast<int>(number - 0.5);
}

} // namespace

/**
 * @brief Initialize and run the system on the target MCU.
 * 
 * @return 0 on termination of the program (should never occur).
 */
int main()
{
    // Obtain a reference to the singleton serial device instance.
    auto& serial{Serial::getInstance()};
    serial.setEnabled(true);

    serial.printf("Machine learning project!\n");

    // Input voltage 0 - 5 V.
    const Vector<double> trainInput{0.0, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0};

    // Expected temperature in Celsius; T = 100 * Vin - 50.
    const Vector<double> trainOutput{-50.0, -40.0, -30.0, -20.0, -10.0, 0.0, 10.0, 20.0, 30.0, 40.0, 50.0};

    //! \note Jag lade till en kommentar för er modell här.
    // Create a machine learning model for temperature prediction.
    ml::lin_reg::LinReg model{trainInput, trainOutput};

    // Train the linear regression model during 2000 epochs with a 10 % learning rate.
    const bool trained = model.train(2000, 0.1);
    
    //! \note Variabeln "trained" är en placeholder, men eftersom detta är "release-mjukvaran" hade
    //!       den kunnat tas bort.
    // This constant might be used later, so save it for now.
    (void) (trained);
    
    //! \note Lade till en kommentar här med.
    //! Test the linear regression model post training, print the result in the terminal.
    for (const auto& input : trainInput)
    {
        const double prediction{model.predict(input)};
        const auto mV{input * 1000.0};

        serial.printf("Input: %d mV, predicted output: %d!\n", round(mV), round(prediction));
    }

    constexpr uint8_t tempSensorPin{2U};

    // Initialize the GPIO devices.
    Gpio led{8U, Gpio::Direction::Output};
    Gpio button{13U, Gpio::Direction::InputPullup, buttonCallback};

    // Initialize the timers.
    Timer debounceTimer{300U, debounceTimerCallback};
    Timer predictTimer{6000UL, predictTimerCallback};

    // Obtain a reference to the singleton watchdog timer instance.
    auto& watchdog{Watchdog::getInstance()};

    // Obtain a reference to the singleton EEPROM instance.
    auto& eeprom{Eeprom::getInstance()};

    // Obtain a reference to the singleton ADC instance.
    auto& adc{Adc::getInstance()};

    //! \note Snyggt att ni passerar modellen samt temperatursensors pin-nummer till 
    //!       system-implementationen. Ni som användare kan då välja vad ni vill använda,
    //!       samtidigt som system-implementationen löser logiken.
    // Initialize the system with the given hardware.
    target::System system{led, button, debounceTimer, predictTimer, 
        serial, watchdog, eeprom, adc, model, tempSensorPin};
    mySys = &system;

    // Run the system perpetually on the target MCU.
    mySys->run();

    // This point should never be reached; the system is intended to run indefinitely on the target MCU.
    return 0;
}
