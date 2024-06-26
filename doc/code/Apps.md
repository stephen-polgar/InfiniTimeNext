# Apps

This page will teach you:

- what screens and apps are in InfiniTimeNext
- how to implement your own app

## Theory

The user interface of InfiniTimeNext is made up of **screens**.
Screens that are opened from the app launcher are considered **apps**.
Every app in InfiniTimeNext is its own class.
Apps run inside the `DisplayApp` task (briefly discussed [here](./Intro.md)).
When the user launch an app new instance of the class is created when it is not in the `ScreenStack` and the `DisplayApp` calls the `void Load()` method.
Apps `void Load()` method are responsible for everything drawn on the screen.
On leaving the app `DisplayApp` calls the `bool UnLoad()` method.  
If the `bool UnLoad()` method returns true, `DisplayApp` stores the app in `ScreenStack`, otherwise deletes it.
The `bool UnLoad()` method normally calls the ,lv_obj_clean(lv_scr_act())` method, which also deletes any lvgl objects created by other objects. 
Apps can be refreshed periodically and reacts to external events (touch or button).

## Interface

Every app class is declared inside the namespace `Pinetime::Applications::Screens` 
and inherits
from [`Pinetime::Applications::Screens::Screen`](/src/displayapp/screens/Screen.h).

Each app defines its own constructor.
The constructors mostly used to create new object with different parameters (ex: [AlarmSet](/src/displayapp/screens/AlarmSet.h),...). The Screen::Load() method is responsible for initializing the UI of the app.

The **destructor** normally calls the `bool UnLoad()` method for cleans up LVGL and restores any changes (for example re-enable sleeping).

App classes can override `bool OnButtonPushed()`, `bool OnTouchEvent(TouchEvents event)`
and `bool OnTouchEvent(uint16_t x, uint16_t y)` to implement their own functionality for those events.

Apps that need to be refreshed periodically create an `lv_task` (using `lv_task_create()`)
that will call the method `Refresh()` periodically.

## App types

There are basically 3 types of applications : **system** apps and **user** apps and **watch faces**.

**System** applications are always built into InfiniTimeNext, and InfiniTimeNext cannot work properly without those apps.

**User** applications are optionally built into the firmware. 

**Watch faces** are very similar to the **user** apps, they are optional, but at least one must be built into the firmware.

The distinction between **system** apps,  **user** apps and watch faces allows for more flexibility and customization.
This allows to easily select which user applications and watch faces must be built into the firmware
without overflowing the system memory.

## Apps and watch faces initialization

The UI of the current app are loaded by `DisplayApp` in `DisplayApp::loadScreen.
This method calls the Screen::Load() method.

The constructor of **system** apps is called directly. If the application is a **user** app,
the corresponding `AppDescription` is first retrieved from `userApps`
and then the function `create` is called to create an instance of the app.

Watch faces are handled in a similar way as the **user** apps : they are created by `DisplayApp` in the method `DisplayApp::loadScreen` when the application type is `Apps::Clock`.

## User application selection at build time

The list of user applications is generated at build time by the `consteval` function `CreateAppDescriptions()`
in `UserApps.h`. This method takes the list of applications that must be built into the firmware image.
This list of applications is defined as a list `Apps` enum values named `UserAppTypes` in `[Apps.h](/src/displayapp/apps/Apps.h)`.
For each application listed in `UserAppTypes`, an entry of type `AppDescription` is added to the array `userApps`.
This entry is created by using the information provided by a template `AppTraits`
that is customized for every user application.

Here is an example of an AppTraits customized for the Alarm application. 
It defines the type of application, its icon and a function that returns an instance of the application.

```c++
template <>
struct AppTraits<Apps::Alarm> {
  static constexpr Apps app = Apps::Alarm;
  static constexpr const char* icon = Screens::Symbols::clock;

  static Screens::Screen* Create() {
    return new Screens::Alarm();
  };
};
```

This array `userApps` is used by `DisplayApp` to create the applications and the `AppLauncher`
to list all available applications.

## Watch face selection at build time

The list of available watch faces is also generated at build time by the `consteval`
function `CreateWatchFaceDescriptions()` in [`UserApps.h`](/src/displayapp/UserApps.h) in the same way as the **user** apps.
Watch faces must declare a `WatchFaceTraits` so that the corresponding `WatchFaceDescription` can be generated.
Here is an example of `WatchFaceTraits`:
```c++
    template <>
    struct WatchFaceTraits<WatchFace::Analog> {
      static constexpr WatchFace watchFace = WatchFace::Analog;
      static constexpr const char* name = "Analog face";

      static Screens::Screen* Create() {
        return new Screens::WatchFaceAnalog();
      };

      static bool IsAvailable(Pinetime::Controllers::FS& /*filesystem*/) {
        return true;
      }
    };
```

## Creating your own app

A minimal user app could look like this:

MyApp.h:

```cpp
#pragma once

#include "displayapp/screens/Screen.h"
#include "Symbols.h"

namespace Pinetime {
  namespace Applications {
    namespace Screens {
      class MyApp : public Screen {
      public:
        MyApp();
        MyApp::MyApp(/* optional arguments */);
        void Load() override;
        bool UnLoad() override;
        ~MyApp() override;
       private:
        // optional arguments
      };
    }    
    template <>
    struct AppTraits<Apps:MyApp> {
      static constexpr Apps app = Apps::MyApp;
      static constexpr const char* icon = Screens::Symbol::myApp;
      static Screens::Screens* Create() {
        return new Screens::MyApp();
      }
    };
  }
}
```

MyApp.cpp:

```cpp
#include "displayapp/screens/MyApp.h"
#include "systemtask/SystemTask.h"  // for Controllers

using namespace Pinetime::Applications::Screens;

MyApp::MyApp(/* optional arguments */) : Screen(Apps::MyApp)  {
// optional arguments
}

MyApp::MyApp() : Screen(Apps::MyApp) {
}

void MyApp::Load() {
  running = true;
 // using optional arguments for load if any
  lv_obj_t* title = lv_label_create(lv_scr_act(), NULL);  
  lv_label_set_align(title, LV_LABEL_ALIGN_CENTER);
  lv_label_set_text_static(title, "My test application");
  lv_obj_align(title, NULL, LV_ALIGN_CENTER, 0, 0);
  /**
  * Controllers:
  * System::SystemTask::displayApp->settingsController , ...
  * System::SystemTask::displayApp->systemTask->nimbleController.weatherService , ...
  */
}

/**
* Override `Screen::~Screen()` with `MyApp::~MyApp() { UnLoad(); }`
* and `bool Screen::UnLoad()` with `bool MyApp::UnLoad() { ... }`
* if necessary.
*/ 

```

Both of these files should be in [displayapp/screens/](/src/displayapp/screens/).

Now we have our very own app, but InfiniTime does not know about it yet.
The first step is to include your `MyApp.cpp` (or any new cpp files for that matter)
in the compilation by adding it to [CMakeLists.txt](/CMakeLists.txt).
The next step to making it launch-able is to give your app an id.
To do this, add an entry in the enum class `Pinetime::Applications::Apps` [displayapp/apps/Apps.h](/src/displayapp/apps/Apps.h).
Name this entry after your app. Add `#include "displayapp/screens/MyApp.h"`
to the file [`displayapp/UserApps.h`](/src/displayapp/UserApps.h).

If your application is a **system** application, go to the function `DisplayApp::loadNewScreen`
and add another case to the switch statement.
The case will be the id you gave your app earlier.
If your app needs any additional arguments, this is the place to pass them.

If your application is a **user** application, you don't need to add anything in DisplayApp, everything will be automatically generated for you.
The user application will also be automatically be added to the app launcher menu.

Starting applications:

```cpp
#include "systemtask/SystemTask.h"
System::SystemTask::displayApp->StartApp(Apps::MyApp);  // old but working method for use permanent arguments
System::SystemTask::displayApp->StartApp(new MyApp(/* arguments */);  // new method for use different arguments
```

For build only selected apps edit the [displayapp/apps/UsedAppsConfig.h](/src/displayapp/apps/UsedAppsConfig.h) file.

You should now be able to [build](../buildAndProgram.md) the firmware
and flash it to your PineTime. Yay!

Please remember to pay attention to the [UI guidelines](../ui_guidelines.md)
when designing an app that you want to be included in InfiniTime.
