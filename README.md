# os_project [SENSOR LOGGER IN ARDUINO]

Authors:
	Paolo Rollo, Matricola 1713885
	Ludovico Lowe, Matricola 1707235
Students @Sapienza, University of Rome.

Using bare metal programming in AVR C (so without intermediating with the Arduino IDE/Libraries), we are going to implement a system to read one or more sensors (like temperature..), and to deliver, when requested, the registrated data to the PC using a binary packet protocol.
On the PC, we're going to implement a program to interact with the Arduino to:
-set the parameters of the sensor(s).
-send requests of getting the data registrated till the moment.
-display the incoming messages.


## Specifiche del progetto
#### Arduino Sensor Logger (Termometro)
- On Arduino
	- Periodicamente (~ 1 ora) prendo la temperatura dal sensore e la metto in un ring buffer sulla eeprom (ring buffer senno' mi si riempie tutto).
	- Quando un client si connette (un PC), se il client lo richiede invio tutto le registrazioni contenute nel ring buffer;
	+ La comunicazione deve avvenire tramite la UART (vedere ```orazio_firmware_port/src/arch/include``` e ```orazio_firmware_port/src/arch/atmega328```);
	+ La comunicazione tra PC e Microcontrollore **deve essere a pacchetti** (vedere ```so_game/so_game_protocol.h```)
	- I **parametri** del sensore devono essere poter configurati e salvati nella eeprom. Appena acceso il microcontrollore carica la configurazione salvata sulla eeprom.

- On PC
	- Aprire la seriale senza librerie strane (usare ```orazio_firmware_port/src/client/serial_linux.h``` e vedere ```orazio_firmware_port/src/client/orazio_client.c```);
	- Richiedere i log di temperatura tramite un pacchetto request
	- Leggere i log e stamparli da qualche parte
	- Mandare e leggere il pacchetto di configurazione dei parametri del sensore.
