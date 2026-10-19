// Bridging header: exposes the parts of libchiaki (chiaki-ng) the Swift engine uses.
// Headers come from build/ios/include: the library's public headers plus the generated config.h, copied
// there by scripts/build_chiaki_ios.sh.
#ifndef PS5_REMOTE_CONTROL_CHIAKI_BRIDGE_H
#define PS5_REMOTE_CONTROL_CHIAKI_BRIDGE_H

#include <chiaki/common.h>
#include <chiaki/log.h>
#include <chiaki/base64.h>
#include <chiaki/discovery.h>
#include <chiaki/discoveryservice.h>
#include <chiaki/regist.h>
#include <chiaki/controller.h>
#include <chiaki/ctrl.h>
#include <chiaki/session.h>

#endif
