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
