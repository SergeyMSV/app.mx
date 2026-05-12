
## SiRF

### 100—SetSerialPort

#### Request

```
PSRF100,1,4800,8,1,0
PSRF100,1,9600,8,1,0
PSRF100,1,19200,8,1,0
PSRF100,1,38400,8,1,0
```

#### Response

It seems to be no response even if the baudrate is the same.

### 101—NavigationInitialization

Request

```
PSRF101,0,0,0,0,0,0,12,1	- Hot Start— All data valid
PSRF101,0,0,0,0,0,0,12,2	- Warm Start—Ephemeris cleared	
PSRF101,0,0,0,0,0,0,12,3	- Warm Start (with Init)—Ephemeris cleared, initialization data loaded
PSRF101,0,0,0,0,0,0,12,4	- Cold Start—Clears all data in memory
PSRF101,0,0,0,0,0,0,12,8	- Clear Memory—Clears all data in memory and resets the receiver back to factory defaults (it resets SerialPort Baudrate)
```

#### Response

```
$PSRFTXT,NMEA: ID 101 Ack Input
$PSRFTXT,GSU-7x : Position Co.,Ltd.2009
$PSRFTXT,Firmware Checksum: ???? 
$PSRFTXT,TOW:  0
$PSRFTXT,WK:   ????
$PSRFTXT,POS:  ??????? 0 0
$PSRFTXT,CLK:  ?????
$PSRFTXT,CHNL: 12
$PSRFTXT,Baud rate: 19200
```

### 103—Query/Rate Control

1. Msg: GGA,GSV...
1. Mode: 0=SetRate, 1=Query
1. Rate: sec Output—off=0, max=255
1. CksumEnable: 0=Disable Checksum, 1=Enable Checksum

#### Request

```
PSRF103,00,00,00,01 - GGA off
PSRF103,00,00,01,01 - GGA once a second
PSRF103,01,00,00,01 - GLL off
PSRF103,01,00,01,01 - GLL once a second
PSRF103,02,00,00,01 - GSA off
PSRF103,02,00,01,01 - GSA once a second
PSRF103,03,00,00,01 - GSV off
PSRF103,03,00,01,01 - GSV once a second
PSRF103,04,00,00,01 - RMC off
PSRF103,04,00,01,01 - RMC once a second
PSRF103,05,00,00,01 - VTG off
PSRF103,05,00,01,01 - VTG once a second
PSRF103,06,00,00,01 - MSS off (If internal beacon is supported)
PSRF103,06,00,01,01 - MSS once a second (If internal beacon is supported)
PSRF103,07,00,00,01 - Not defined
PSRF103,07,00,01,01 - Not defined
PSRF103,08,00,00,01 - ZDA off (if 1PPS output is supported)
PSRF103,08,00,01,01 - ZDA once a second (if 1PPS output is supported)
PSRF103,09,00,00,01 - Not defined
PSRF103,09,00,01,01 - Not defined
```

#### Response

```
$PSRFTXT,NMEA: ID 103 Ack Input
```
