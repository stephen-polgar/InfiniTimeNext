# [InfiniTimeNext](https://github.com/stephen-polgar/InfiniTimeNext)

This repository is a fork of the [InfiniTime firmware](https://github.com/InfiniTimeOrg/InfiniTime) for the PineTime smartwatch.

Code optimized to access all the capabilities of the watch.

## Main features
- The stopwatch can run in the background
- Multiple saved alarm times with advanced settings
- Several saved timers can run at the same time
- Steps are shown in distance also
* [Calculator](https://github.com/InfiniTimeOrg/InfiniTime/pull/1483)
* [Calendar](https://github.com/InfiniTimeOrg/InfiniTime/pull/923)
* [File Manager](doc/apps/FileManager.md)

### Changes [here](doc/code/Apps.md)
Most of system and user application constructors are empty, and are used to create new `Screen` based objects with different arguments.
Constructors do not load the screen. The watch screen are filled and cleaned by the `Screen::Load()` and `Screen::UnLoad()` methods.
The `utility/StaticStack.h` has been replaced with `utility/ScreenStack.h` which can store `Screen objects` with different arguments, so it can continue interrupted apps (for example, game results are not lost due to new notifications and timers). Watch faces are handled by `WatchFaceScreen class` instead of `DisplayApp`. `ScreenList class` has been replaced with `utility/ArrayTouchHandler` which allows scrolling large arrays without creating and deleting objects (the items are only renamed), so some class and constant size `std::array` has been removed (`Label`, `Tile`, `List`, `CheckboxList`). I have no debug device and bug reports are welcome.

### Installations
Use [InfiniSimNext](https://github.com/stephen-polgar/InfiniSimNext) to run in the simulator or install [firmware](https://github.com/stephen-polgar/InfiniTimeNext/releases) on [PineTime smartwatch](https://pine64.org/devices/pinetime/).

## New to InfiniTime?
- [Getting started with InfiniTime](doc/gettingStarted/gettingStarted-1.0.md)
- [Updating the software](doc/gettingStarted/updating-software.md)
- [About the firmware and bootloader](doc/gettingStarted/about-software.md)

### Companion apps
- [Gadgetbridge](https://gadgetbridge.org/) (Android)
- [AmazFish](https://openrepos.net/content/piggz/amazfish/) (SailfishOS)
- [Siglo](https://github.com/alexr4535/siglo) (Linux)
- [InfiniLink](https://github.com/InfiniTimeOrg/InfiniLink) (iOS)
- [ITD](https://gitea.elara.ws/Elara6331/itd) (Linux)
- [WatchMate](https://github.com/azymohliad/watchmate) (Linux)

***Note**: We removed mentions to NRFConnect as this app is closed source and recent versions do not work anymore with InfiniTime (the last version known to work is 4.24.3). If you used NRFConnect in the past, we recommend you switch to [Gadgetbridge](https://gadgetbridge.org/).*

## Development
- [InfiniTime Vision](doc/InfiniTimeVision.md)
- [Rough structure of the code](doc/code/Intro.md)
- [How to implement an application](doc/code/Apps.md)
- [Generate the fonts and symbols](src/displayapp/fonts/README.md)
- [Tips on designing an app UI](doc/ui_guidelines.md)
- [Bootloader, OTA and DFU](bootloader/README.md)
- [External resources](doc/ExternalResources.md)

### Contributing
- [How to contribute?](CONTRIBUTING.md)
- [Coding conventions](doc/coding-convention.md)

### Build, flash and debug
- [InfiniTimeNext simulator](https://github.com/stephen-polgar/InfiniSimNext)
- [Build the project](doc/buildAndProgram.md)
- [Build the project with Docker](doc/buildWithDocker.md)
- [Build the project with VSCode](doc/buildWithVScode.md)
- [Flash the firmware using OpenOCD and STLinkV2](doc/openOCD.md)
- [Flash the firmware using SWD interface](doc/SWD.md)
- [Flash the firmware using JLink](doc/jlink.md)
- [Flash the firmware using GDB](doc/gdb.md)
- [Stub using NRF52-DK](doc/PinetimeStubWithNrf52DK.md)

### API
- [BLE implementation and API](doc/ble.md)

### Architecture and technical topics
- [Memory analysis](doc/MemoryAnalysis.md)

### Project management
- [Maintainer's guide](doc/maintainer-guide.md)
- [Versioning](doc/versioning.md)
- [Files included in the release notes](doc/filesInReleaseNotes.md)
- [Files needed by the factory](doc/files-needed-by-factory.md)

## Licenses
This project is released under the GNU General Public License version 3 or, at your option, any later version.

It integrates the following projects:

- RTOS: **[FreeRTOS](https://freertos.org)** under the MIT license
- UI: **[LittleVGL/LVGL](https://lvgl.io/)** under the MIT license
- BLE stack: **[NimBLE](https://github.com/apache/mynewt-nimble)** under the Apache 2.0 license
- Font: **[Jetbrains Mono](https://www.jetbrains.com/fr-fr/lp/mono/)** under the Apache 2.0 license

## Credits
I’m not working alone on this project. First, many people create pull requests for this project. Then, there is the whole #pinetime community: a lot of people all around the world who are hacking, searching, experimenting and programming the Pinetime. We exchange our ideas, experiments and code in the chat rooms and forums.

Here are some people I would like to highlight:

- [Atc1441](https://github.com/atc1441/): He works on an Arduino based firmware for the Pinetime and many other smartwatches based on similar hardware. He was of great help when I was implementing support for the BMA421 motion sensor and I²C driver.
- [Koen](https://github.com/bosmoment): He’s working on a firmware based on RiotOS. He integrated similar libs as me: NimBLE, LittleVGL,… His help was invaluable too!
- [Lup Yuen Lee](https://github.com/lupyuen): He is everywhere: he works on a Rust firmware, builds a MCUBoot based bootloader for the Pinetime, designs a Flutter based companion app for smartphones and writes a lot of articles about the Pinetime!
