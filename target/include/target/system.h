//! \note Bra jobbat!
//!
//! \remark Ni hade fått dubletter - jag hittade en till system.h inuti source-katalogen. Jag tog
//!         dock bort denna.
/**
 * @brief Generic system implementation for an MCU with configurable hardware devices.
 */
#pragma once

namespace driver
{
/** ADC (A/D converter) interface. */
class AdcInterface;

/** EEPROM (Electrically Erasable Programmable ROM) stream interface. */
class EepromInterface;

/** GPIO interface. */
class GpioInterface;

/** Serial transmission interface. */
class SerialInterface;

/** Timer interface. */
class TimerInterface;

/** Watchdog timer interface. */
class WatchdogInterface;
} // namespace driver

//! \note Utmärkt att ni fördeklarerar ert linReg-interface i stället för att inkludera filen direkt.
//!       När ni gör detta minskar dependencies (alla andra som inkluderar denna header behöver då
//!       inte ha "ml/lin_reg/interface.h" tillgänlig) samt att kompileringstiden minskar, då det
//!       blir mindre kod inklistrat här. Även om ert interface är litet så gäller många bäckar små;
//!       Gör ni detta konsekvent för alla headerfiler minskar kraftigt den totala koden i 
//!       headerfilerna och det går fortare att kompilera.
//!
//!       Att fördeklarera fungerar bra, då enbart linReg-referenser används samt att ni inte
//!       använder denna typ här (ni kallar inte på funktioner såsom train med mera => kompilatorn
//!       behöver ej information om hur ml::lin_reg::Interface ser ut i detta skede). Eftersom
//!       pekare och referenser alltid är lika stora går det utmärkt att bara fördeklarera typer,
//!       så länge man bara refererar till dem via pekare eller referenser, samtidigt som man inte
//!       interagerar med typen per se. Om ni hade försökt använda en pass-by-value hade ni dock
//!       fått ett kompileringsfel, då kompilatorn i så fall måste känna till storleken på en typ
//!       (kompilatorn måste känna till hur många byte den måste "lägga" på stacken).
//!
//!       Som exempel, ett linReg-objekt lär uppta 14 byte. Motsvarande pekare eller referens
//!       upptar 2 byte i detta system. Om ni har fördeklarerat linReg och sedan referearar till
//!       denna, så vet kompilatorn att den måste "lägga" 2 byte på stacken, även om den inte har
//!       en aning om vad denna typ är för något => super.
namespace ml
{
namespace lin_reg
{
/** Linear regression inteface. */
class Interface;
}
}

namespace target
{
//! \note Denna kommentar är lite out of date, men OK.
/**
 * @brief Generic system for an MCU with configurable hardware devices.
 * 
 *        The following devices are used:
 * 
 *            - A button connected toggles a timer.
 * 
 *            - The aforementioned timer toggles an LED every 100 ms when enabled.
 * 
 *            - Another timer reduces the effect of contact bounces after pushing the button.
 * 
 *            - A watchdog timer is used to restart the program if it gets stuck somewhere.
 *
 *            - The LED state is written to EEPROM upon every change. This value is evaluated upon startup.
 * 
 *        This class is non-copyable and non-movable.
 */
class System final
{
public:
    //! \note Denna kommentar är uppdaterad för att matcha det nya systemet => super!
    /**
     * @brief Create a new system.
     *     
     * @param[in] led The LED to toggle.
     * @param[in] button Button used to toggle the toggle timer.
     * @param[in] debounceTimer Timer used to mitigate effects of contact bounces.
     * @param[in] predictTimer Timer used to toggle the LED.
     * @param[in] serial Serial device used to print status messages.
     * @param[in] watchdog Watchdog timer that resets the program if it becomes unresponsive.
     * @param[in] eeprom EEPROM stream to write the status of the LED to EEPROM.
     * @param[in] adc ADC .
     * @param[in] model Linear regression model for temperature prediction.
     * @param[in] tempSensorPin Temperature sensor pin.
     */
    explicit System(driver::GpioInterface& led, driver::GpioInterface& button, 
                    driver::TimerInterface& debounceTimer, driver::TimerInterface& predictTimer,
                    driver::SerialInterface& serial, driver::WatchdogInterface& watchdog, 
                    driver::EepromInterface& eeprom, driver::AdcInterface& adc,
                    ml::lin_reg::Interface& model, const uint8_t tempSensorPin) noexcept;

    /**
     * @brief Delete system.
     */
    ~System() noexcept;

    /**
     * @brief Enable serial transmission.
     * 
     * @param[in] enable Indicate whether to enable serial transmission.
     */
    void enableSerialTransmission(const bool enable) noexcept;

    /**
     * @brief Button interrupt handler.
     * 
     *        Toggle the timer whenever the button is pressed. 
     * 
     *        Pin change interrupts are disabled for 300 ms after a press to mitigate the effects 
     *        of contact bounce.
     */
    void handleButtonInterrupt() noexcept;

    /**
     * @brief Debounce timer interrupt handler.
     * 
     *        Enable pin change interrupts 300 ms after a press to mitigate the effects of contact bounce.
     */
    void handleDebounceTimerInterrupt() noexcept;

    /**
     * @brief Toggle timer interrupt handler.
     * 
     *        Toggle the LED every 100 ms when the associated timer is enabled.
     */
    void handlePredictTimerInterrupt() noexcept;

    /**
     * @brief Run the system as long as voltage is supplied.                                                               
     */
    void run() noexcept;

    System()                         = delete; // No default constructor.
    System(const System&)            = delete; // No copy constructor.
    System(System&&)                 = delete; // No move constructor.
    System& operator=(const System&) = delete; // No copy assignment.
    System& operator=(System&&)      = delete; // No move assignment.

private:
    void handleButtonPressed() noexcept;
    void predictTemperature() const noexcept;

    /** Reference to the LED to toggle. */
    driver::GpioInterface& myLed;

    /** Button used to toggle the toggle timer. */
    driver::GpioInterface& myButton;

    /** Debounce timer used to mitigate effects of contact bounces. */
    driver::TimerInterface& myDebounceTimer;

    /** Timer used to toggle the LED. */
    driver::TimerInterface& myPredictTimer;

    /** Serial device used to print status messages. */
    driver::SerialInterface& mySerial;

    /** Watchdog timer that resets the program if it becomes unresponsive. */
    driver::WatchdogInterface& myWatchdog;

    /** EEPROM stream to write the status of the LED to EEPROM. */
    driver::EepromInterface& myEeprom;

    /** A/D converter . */
    driver::AdcInterface& myAdc;

    //! \note Grymt att ni hör följer befintlig stil. Det är ett bra tips att göra i allmänhet
    //!       nu när ni är på väg in i industrin.
    /** Linear regression model for temperature prediction. */
    ml::lin_reg::Interface& myModel;

    /** Temperature sensor pin. */
    const uint8_t myTempSensorPin;
};
} // namespace target