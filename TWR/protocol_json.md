# JSON-based protocol

## Control

### Version

#### Request

```
{
 "ep": "control",
 "cmd": "version"
 }
```

#### Response

```
{
 "ep": "control",
 "cmd": "version"
 "version": "TWR.0.3.1"
}
```

## DALLAS

### Version

#### Request

```
{
 "ep": "dallas",
 "cmd": "version"
}
```

### Search

#### Request

```
{
 "ep": "dallas",
 "cmd": "search"
}
```

#### Response

```
{
  "ep": "dallas",
  "cmd": "search",
  "roms": [
   {
    "family_code": "28",
    "id": "123456789AB1"
   }
  ],
  "rsp": "ok"
}
```

### Request measurements from certain thermosensors

#### Request

```
{
 "ep":" dallas",
 "cmd": "thermo",
 "ids": [
  "123456789AB1",
  "123456789AB2"
 ]
}
```

#### Response

```
{
 "ep": "dallas",
 "cmd": "thermo",
 "ids": [
  "123456789AB1"
 ],
 "measurements": [
  {
   "id": "123456789AB1",
   "temperature": "26.75"
  }
 ],
 "rsp": "ok"
}
```

## UART

### Open port

#### Request

```
{
 "ep": "uart_X",
 "cmd": "open"
 }
 ```

#### Response

```
{
 "ep": "uart_X",
 "cmd": "open",
 "rsp": "ok"				- ok, error (port is not available)
}
 ```

### Close port

#### Request

```
{
 "ep": "uart_X",
 "cmd": "close"
}
```

#### Response

```
{
 "ep": "uart_X",
 "cmd": "close",
 "rsp": "ok"				- ok
}
```

### Receive

#### Request

```
{
 "ep": "uart_X",
 "cmd": "receive"
}
```

#### Response

```
{
 "ep": "uart_X",
 "cmd": "receive",
 "data": "RECEIVED STRING",
 "rsp": "ok"
}
```

### Send

#### Request

```
{
 "ep": "uart_X",
 "cmd": "send",
 "data": "SENT STRING"
}
 ```

#### Response

```
{
 "ep": "uart_X",
 "cmd": "send",
 "rsp": "ok"
}
```

### Get settings - it hasn't been implemented yet.

#### Request

```
{
 "ep": "uart_X",
 "cmd": "get_settings",
}
```

#### Response

```
{
 "ep": "uart_X",
 "cmd": "get_settings",
 "id": "/dev/ttyS0",
 "br": 115200
}
```

### Set settings - it hasn't been implemented yet.

#### Request

```
{
 "ep": "uart_X",
 "cmd": "set_settings",
 "id": "/dev/ttyS0",
 "br": 115200
}
```

#### Response

```
{
 "ep": "uart_X",
 "cmd": "set_settings",
 "rsp": "ok"
}
```

### Ping - it hasn't been implemented yet.

#### Request

```
{
 "ep": "uart_X",
 "cmd": "ping",
 "rts": true,
 "dtr": true
}
```

#### Response

```
{
 "ep": "uart_X",
 "cmd": "ping",
 "rts": true,
 "cts": true,
 "dtr": true,
 "dsr": true,
 "data_stored_size": 450023,			- received data size
 "rsp": "ok"					- ok, error (port is not available)
}
```
