#ifndef ENVIRONMENT_MANAGER_H
#define ENVIRONMENT_MANAGER_H

#include "interface.h"


// TODO : add some ambient music!
namespace engine
{
const float ENVMAN_MAX_DAY_RATIO			= 1.0f;
const float ENVMAN_MIN_DAY_RATIO			= 0.0f;
const float DEFAULT_ENVMAN_DAY_RATIO_STEP	= 0.01f;
const float DEFAULT_ENVMAN_DAY_RATIO		= 0.5f;

class EnvironmentManager
{
public:
	float dayRatio;
	float dayRatioStep;

	EnvironmentManager()
		: dayRatio(DEFAULT_ENVMAN_DAY_RATIO),
		dayRatioStep(DEFAULT_ENVMAN_DAY_RATIO_STEP) {}

	void processCommands(const Commands *cmd)
	{
		if (cmd->moreDay == 1 || cmd->moreDay == -1)
		{
			float dayRatio = this->dayRatio + cmd->moreDay * this->dayRatioStep;
			if (dayRatio > ENVMAN_MAX_DAY_RATIO)
			{
				dayRatio = ENVMAN_MAX_DAY_RATIO;
			}
			else if (dayRatio < ENVMAN_MIN_DAY_RATIO)
			{
				dayRatio = ENVMAN_MIN_DAY_RATIO;
			}
			this->dayRatio = dayRatio;
		}
	}
};
}

#endif