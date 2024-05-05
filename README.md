# Hide Health and Safety App

## Usage
Via the plugin config menu (press L, DPAD Down and Minus on the gamepad) you can configure the plugin. The available options are the following:
- **Settings**: 
  - Enable:
    - With this option you can globally enable/disable this Plugin. It will automatically restart the current application 

## Installation
(`[ENVIRONMENT]` is a placeholder for the actual environment name.)

1. Copy the file `HideHealthandSafetyApp.wps` into `sd:/wiiu/environments/[ENVIRONMENT]/plugins`.  
2. Requires the [WiiUPluginLoaderBackend](https://github.com/wiiu-env/WiiUPluginLoaderBackend) in `sd:/wiiu/environments/[ENVIRONMENT]/modules`.


## Buildflags

### Logging
Building via `make` only logs errors (via OSReport). To enable logging via the [LoggingModule](https://github.com/wiiu-env/LoggingModule) set `DEBUG` to `1` or `VERBOSE`.

`make` Logs errors only (via OSReport).  
`make DEBUG=1` Enables information and error logging via [LoggingModule](https://github.com/wiiu-env/LoggingModule).  
`make DEBUG=VERBOSE` Enables verbose information and error logging via [LoggingModule](https://github.com/wiiu-env/LoggingModule).  

If the [LoggingModule](https://github.com/wiiu-env/LoggingModule) is not present, it'll fall back to UDP (Port 4405) and [CafeOS](https://github.com/wiiu-env/USBSerialLoggingModule) logging.

---