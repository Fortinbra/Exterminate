#pragma once

#ifdef __cplusplus
extern "C" {
#endif

// Forward declaration
struct uni_platform;

// Get the custom platform implementation
struct uni_platform* get_exterminate_platform(void);

#ifdef __cplusplus
}
#endif
