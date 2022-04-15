# billard-club-table-device
Code for running an Arduino device for controlling the light by an RFID card system.

## Schematics

![Steckplatine](/schematics/TableDevice_Steckplatine.png)

## Building via Arudino CLI

The code is prepared for builing the code with the Arduino CLI toolchain.

### Install Arduino CLI on MacOS

To run Arduino CLI on MaxOS install it via Homebrew.

```sh
brew update
brew install arduino-cli
````

Detailed information other installation options can be found on the Arduino Website: https://arduino.github.io/arduino-cli/0.21/installation/

### Prepare build

Create a file ```secret_credentials.h``` in the TableDevice folder of this repository. This holds your private credentials that should never be pushed to the repository!!!
Insert the following into the file and fill your secrets:

```cpp
#ifndef SECRET_CREDENTIALS_H
#define SECRET_CREDENTIALS_H

#define SECRET_WIFI_SSID "<YOUR_WIFI_SSID>" 
#define SECRET_WIFI_KEY "<PSK_FOR_THE_WIFI>"
#define SECRET_TABLE_USERNAME "<API_USERNAME_OF_THIS_DEVICE>"
#define SECRET_TABLE_PASSWORD "<API_USERS_PASSWORD_OF_THIS_DEVICE>"
#define SECRET_API_BASEURL "http://192.168.2.103:3000/club-api" // this could differ, depending on you ip an protocol
#define SECRET_API_USERAGENT "DSC_SnookerTable1" // change to the desired userAgent ID

#endif
```

Update the local platform cache:

```sh
arduino-cli core update-index
```

See if your own board is present:

```sh
arduino-cli board list
```

On my machine, it looks like this:

```sh
Port                            Protocol Typ               Board Name FQBN Core
/dev/cu.BLTH                    serial   Serial Port       Unknown
/dev/cu.Bluetooth-Incoming-Port serial   Serial Port       Unknown
/dev/cu.BoseNC700HP             serial   Serial Port       Unknown
/dev/cu.usbserial-142230        serial   Serial Port (USB) Unknown
```

Details about this can be found here: https://arduino.github.io/arduino-cli/0.21/getting-started/#connect-the-board-to-your-pc

### Build with the build.sh script

### Manual build process

To build it manually, you need to enter you WIFI AP credentials in the priva

We use the Wemos D1 Mini board here, which is based on the NodeMCU with ESP8266 on a Mac. So the build command itself looks like this:

```sh
arduino-cli compile --fqbn esp8266:esp8266:d1 TableDevice

```