# IMU data processing for compass heading

Das Projekt basiert auf einem ICM20498 Chip, welcher über drei verschiedene Sensoren verfügt (Gyroscope, 
Accelerometer, Magnetometer).

Ziel ist es die Daten des Sensors zu erfassen und auszulesen. Danach werden die Daten über eine
Serielle-Schnittstelle weiter geleitet und mit einem speziellen Tool intepretiert. Die Tools sind 
mittels der Programmiersprache "Processing" realisiert.

In einer früheren Version wurde der MPU9250 anstatt des ICM20948 verwendet. Da dieser aber kaum noch in 
einer vertrauenswürdigen Variante verfügbar ist, wurde deshalb der Sensor auf den ICM20948 umgestellt.

Unter folgendem Link ist eine frühere Version der Firmware als Tag zu finden:
https://github.com/CreLab/imu_processing/tree/v0.1

Zum Ausführen der Firmware und Tools wird folgendes benötigt:

### Hardware
- ESP32 NodeMCU -> https://www.az-delivery.de/products/esp32-developmentboard
- Breakout ICM20948 -> https://shop.pimoroni.com/products/icm20948?variant=27843993960531

### Software
- Visual Studio Code (*Version 1.67.2 oder höher*) -> https://code.visualstudio.com/Download
- PlatformIO (*VS Code AddOn*) (*Core 6.0.1 / Home 3.4.1*) 
- Processing (*Verion 3.5.4 oder höher*) -> https://processing.org/download
- Python (*Dies sollte in der Regel mit VS Code schon installiert sein*) -> https://www.python.org/downloads/

## Anschließen der Hardware

![alt text](https://github.com/CreLab/imu_processing/blob/main/doc/HardwareSetup.png)

## Ordnerbeschreibung

```
├── doc       --> Found documenations about the topic
├── include   --> Header includes
├── lib       --> PlatformIO libs
├── src       --> Source files
├── test      --> PlatformIO test
├── tools     --> Processing Tools
│    ├── AccelerometerTest   --> Viewer for accelerometer data
│    ├── CompassViewer       --> Viewer for compass East/South/West/North
│    ├── MagCalibration      --> Calibration tool for magnitude sensor
│    └── MagViewer           --> Viewer for magnitude data
├── .gitignore       --> Ignore list to exclude files and directories for github checkin
└── platformio.ini   --> Init file for PlatformIO
```

## Compass Kalibrierung

Für die Kalibrierung ist es als erstes erforderlich die korrekte Firmware auf den *ESP32 NodeMCU*
zu bekommen.

Dafür wird mit *Visual Studio Code* das define `#define FREE_IMU_CAL` in der Datei *defines.h* 
mit `#if 1` aktiviert und der Code anschließend auf die Hardware hochgeladen.

Danach ist es möglich das Tool *MagCalibration* mit Processing3 auszführen. Beim Bewegen des Sensors
sollte ein ähnliches Bild, wie das folgende, entstehen:

![Alt-Text](https://github.com/CreLab/imu_processing/blob/main/doc/NotCalibratedMagneto.png)

Während das Tool zur Kalibierung ausgeführt wird, schreibt das es im Hintergrund gleichzeitig alle erfassten Daten
in eine *.csv* Datei. In dieser Datei werden alle Werte des Accelerometers und Magnetometers erfasst.

Wenn genug Werte erfasst worden sind, können wir das Programm schließen und die *.csv* mit *Excel* öffnen.
Falls *Excel* nicht auf Englisch gestellt ist müssen, mit einem Editor nach Wahl, vorher alle Punkte der Zahlen
mit Kommas ersetzt werden. Danach kann man die Datei mit *Excel* erneut öffnen und die Spalten wie folgt kopieren.

Die ertsen drei Spalten sind die erfasten Werte für das Accelerometer. Diese müssen alle markiert und in eine Datei
mit dem Namen *acc.txt* kopiert werden. Danach müssen, für das spätere Python-Skipt, wieder alle Kommas mit Punkten
ersetzt werden.

Das muss nun auch für die Werte des Magnetometers gemacht werden. Dazu alle Werte der letzten drei Spalten aus *Excel* 
markieren und in eine Datei mit dem Namen *magn.txt* kopieren. Auch hier müssen, für das spätere Python-Skipt, wieder
alle Kommas mit Punkten ersetzt werden.

Falls Excel mit der englischen Zahlendarstellung umgehen kann, muss dieser Schritt nicht gemacht werden.

Nun ist alles bereit für die Berechnung der Kalibrierungswerte. Dazu einfach das *freeimu_cal.bat* mit einem doppel-Klick
ausführen und es erscheint folgendes Fenster:

![Alt-Text](https://github.com/CreLab/imu_processing/blob/main/doc/PythonOutput.png)

Die Fehlermeldungen können vorerst ignoriert werden. Diese passieren auf Grund der Python Version, welche nicht mehr alle 
Funktionen für die Berechnung der Accelerometer Kalibrierungswerte bereit stellt.

Nun kopiert man die dort auf geführten Kalibrierungswerte an die entsprechende Stelle in die Datei *sensor_icm20948.cpp*
und tauscht die dort hinterlegten Werte aus.

Die Formel ist immer wie folgt:

`Y = ( X - Offset ) / Scale`

Ist dieser Schritt erledigt, wird in der Datei *defines.h* das Define `#define FREE_IMU_CAL` mit `#if 0` wieder deaktiviert
und damit die Kalibrierungswerte übernommen. Nach einem erneuten Hochladen der Firmware, kann jetzt das Tool *MagViewer* 
ausgeführt werden und ein ähnliches Bild erscheinen.

![Alt-Text](https://github.com/CreLab/imu_processing/blob/main/doc/CalibratedMagneto.png)

Jetzt ist das Magnetometer und das Accelerometer kalibriert und einsatzbereit.
