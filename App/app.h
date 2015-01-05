#ifndef _APP_H
#define _APP_H

#ifdef __cplusplus
extern "C" {
#endif

#include "../evo.h"
#include "entities.h"
#include "../tv_client.h"
#include "playermotor.h"

#include "../grid.h"
#include "../camera_controller.h"
#include "../cursor.h"
#include "../hud.h"

#include "../ability.h"
#include "../move.h"

#include "../abilities.h"

#include "../projectile.h"
#include "../laser.h"
#include "../gun.h"
#include "../mass.h"
#include "../velocity.h"

#include "../spawner.h"
#include "../sinespawner.h"
#include "../story_point.h"

#include "../app_sine_move.h"

void app_register();

#ifdef __cplusplus
}
#endif
#endif
