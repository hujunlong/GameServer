#pragma once

#include <cstdint>

struct MqHead
{
	int32_t aid; //Associate id
	int16_t type;
	int16_t flag;
};
typedef struct MqHead MqHead;
