# The Split Alarm Clock

The split alarm clock has been invented because I am incredibly bad at waking up and not hitting the snooze button.

My over-engineered solution? Put the alarm clock next to my bed, and put the off button in another room (like my bathroom) so that once I get up to turn it off, I might as well take a shower and wake up anyway.

## Current Base Station Features
Features, roughly grouped by file:
* Alarm Clock
    * Currently supports two alarm clocks, each configurable by day(s), hour, and minute
    * Alarm can only be cleared by pressing the remote button
* RGB notifications
    * Flashing red and blue lights for when the alarm is going off
    * Subtle green light notifying the user when the connection to the remote button has been lost
* Real Time Clock
    * Interrupt driven real-time clock based on on-board 32.xxx kHz osc
* USB-Serial Debugging interface
    * View and Configure settings of the device over USB
    * Simulate conditons for debugging purposes
* TODO: HD44780 Display Support
    * Still todo. Waiting on a part to be delivered, but a placeholder library has been dropped in
* EEPROM backup of time and alarms
    * Non-Volatile backup of time and alarms
    * Currently this is the only backup across power cycles for time and alarms
    * Once I add a battery backed external RTC, this probably wont store time anymore. "Good Enough" for now.
    * Currently loading occurs automatically at power-up, storing has to be done manually over USB debugging interface
    * In the future, alarms will be stored automatically when set.
    * CRC32 checking of stored parameters before loading
* XBee-based Communication with a remote button
    * Expects message from low-power remote button once per minute, monitors link health
    * Could support messages from multiple remote buttons if desired later

## Current Remote Button Features
* Ultra low power usage
    * Only wakes once per minute to poll the button status
    * If the button is pressed, the radio will immediately wake to notify the base station
    * Radio uses <10uA in sleep mode, coupled with LDO regulator.
    * Device is sleeping >99.8% of the time. Estimated battery life is easily more than a year on 3xAA cells
    * Radio only uses 45mA while transmitting
* 100ft (30m) range indoors

## Definite future features
* Full support of a 4x20 HD44780 style display
    * View time
    * View next alarm/time to next alarm
    * View remote button health
* User input using buttons rather than USB connection
* Battery-backed RTC for power cycle persistence
* A loud alarm to actually wake me up, instead of blinking angrily
* Switch from RGB LEDs to 2x Shiftbrites for ultra brite LED alarm lights

## Possible future features
* Adaptive power scaling
    * Monitor signal strength to reduce remote button RX/TX power to improve battery life
* Low battery warning for remote button
    * Would require a voltage divider
* Support for multiple remote buttons
    * One in the kitchen, one in the bathroom
* Wi-Fi support
    * Live time updates using WiFi
    * Remote management
    * Could wake using music from a PC server rather than a Piezo speaker
* Atomic Clock support
    * Radio input from the German Atomic clock broadcast
* Support for a "Slave" Alarm Clock
    * Allows multiple base stations that share the same Remote Button, but have separate alarm times and behaviors
* Control remote devices
    * WiFi enabled lights
    * WiFi enabled coffee maker
* Disable alarms with a local button press if the remote button has lost connection
* Replace analog potentiometer for backlight brightness with a digital pot controlled by I2C to adaptively adjust brightness
* Full-power backup to prevent the device from being unplugged to shut it up
