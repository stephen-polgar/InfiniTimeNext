#pragma once

namespace Pinetime {
  namespace Utility {
    template <class T>
    class DirtyValue {
    public:
      DirtyValue() = default; // Use NSDMI

      explicit DirtyValue(T const& v) : value {v} {
      } // Use MIL and const-lvalue-ref

      bool IsUpdated() {
        if (isUpdated) {
          isUpdated = false;
          return true;
        }
        return false;
      }
    
      T const& Get() {
        isUpdated = false;
        return value;
      } // never expose a non-const lvalue-ref

      DirtyValue& operator=(const T& other) {
        if (value != other) {
          value = other;
          isUpdated = true;
        }
        return *this;
      }

    private:
      T value {};            // NSDMI - default initialise type
      bool isUpdated {true}; // NSDMI - use brace initialisation
    };
  }
}
