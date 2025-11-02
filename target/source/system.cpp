/**
 * @brief Generic system implementation details for an MCU with configurable hardware devices.
 */
#include <stdint.h>

#include "driver/adc/interface.h"
#include "driver/eeprom/interface.h"
#include "driver/gpio/interface.h"
#include "driver/serial/interface.h"
#include "driver/timer/interface.h"
#include "driver/watchdog/interface.h"
#include "ml/lin_reg/interface.h"
#include "target/system.h"

namespace target
{
namespace
{
// -----------------------------------------------------------------------------
constexpr int round(const double number) noexcept
{
    return 0.0 <= number ? static_cast<int>(number + 0.5) : static_cast<int>(number - 0.5);
}
} // namespace

// -----------------------------------------------------------------------------
System::System(driver::GpioInterface& led, driver::GpioInterface& button,
               driver::TimerInterface& debounceTimer, driver::TimerInterface& toggleTimer,
               driver::SerialInterface& serial, driver::WatchdogInterface& watchdog,
               driver::EepromInterface& eeprom, driver::AdcInterface& adc,
               ml::lin_reg::Interface& model, const uint8_t tempSensorPin) noexcept
    : myLed{led}
    , myButton{button}
    , myDebounceTimer{debounceTimer}
    , myPredictTimer{toggleTimer}
    , mySerial{serial}
    , myWatchdog{watchdog}
    , myEeprom{eeprom}
    , myAdc{adc}
    , myModel{model}
    , myTempSensorPin{tempSensorPin}
{
    myButton.enableInterrupt(true);
    mySerial.setEnabled(true);
    myWatchdog.setEnabled(true);
    myAdc.setEnabled(true);
    myPredictTimer.start();
}

// -----------------------------------------------------------------------------
System::~System() noexcept
{
    myLed.write(false);
    myButton.enableInterrupt(false);
    myDebounceTimer.stop();
    myPredictTimer.stop();
    myWatchdog.setEnabled(false);
}

// -----------------------------------------------------------------------------
void System::enableSerialTransmission(const bool enable) noexcept
{
    mySerial.setEnabled(enable);
}

// -----------------------------------------------------------------------------
void System::handleButtonInterrupt() noexcept
{
    myButton.enableInterruptOnPort(false);
    myDebounceTimer.start();
    if (myButton.read()) { handleButtonPressed(); }
}

// -----------------------------------------------------------------------------
void System::handleDebounceTimerInterrupt() noexcept
{
    myDebounceTimer.stop();
    myButton.enableInterruptOnPort(true);
}

// -----------------------------------------------------------------------------
void System::handlePredictTimerInterrupt() noexcept 
{ 
    predictTemperature(); 
}

// -----------------------------------------------------------------------------
void System::run() noexcept
{
    mySerial.printf("Running the system!\n");
    
    while (1)
    {
        myWatchdog.reset();
    }
}

// -----------------------------------------------------------------------------
void System::handleButtonPressed() noexcept
{
    mySerial.printf("Button pressed!\n");
    predictTemperature();

    // Restart the timer after button press.
    myPredictTimer.restart();
}

//! \note Jag gillar denna metod. Jättesnyggt att ni lade till denna variant i stället för att
//!       skriva samma kodstycke två gånger (i handleButtonPressed samt handlePredictTimerInterrupt).
// -----------------------------------------------------------------------------
void System::predictTemperature() const noexcept
{
    const auto inputVoltage{myAdc.inputVoltage(myTempSensorPin)};
    const auto mV{inputVoltage * 1000.0};
    const double predictedTemp{myModel.predict(inputVoltage)};
    mySerial.printf("Input: %d mV, predicted output: %d!\n", round(mV), round(predictedTemp));
}
} // namespace target