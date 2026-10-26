# Pinned libraries

Exact copies of the public libraries this sketch uses, so a library update can never change the build.
The Arduino IDE compiles everything under `src/` automatically. Include them by path, never with `<...>`:

```cpp
#include "src/PubSubClient/PubSubClient.h"
#include "src/ArduinoJson/ArduinoJson-v7.4.3.h"
```

| Library | Version | Source | License |
|---|---|---|---|
| ArduinoJson | 7.4.3 | single-header release `ArduinoJson-v7.4.3.h` from github.com/bblanchon/ArduinoJson/releases | MIT |
| PubSubClient | 2.8 | `src/` of tag `v2.8`, github.com/knolleary/pubsubclient | MIT |

Files are unmodified. To upgrade, replace the files, update the `#include` lines and this table, then test.
