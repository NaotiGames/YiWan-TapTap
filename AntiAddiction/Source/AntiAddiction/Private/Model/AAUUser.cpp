#include "AAUUser.h"
#include "AAUType.h"
void FAAUUser::ResetAgeLimit(const int ageLimit, bool isAdult)
{
	if(ageLimit < 0)
	{
		if(isAdult)
		{
			AgeLimit = EAAUAgeLimit::UnknownAgeRangeAdult;
		}else
		{
			AgeLimit = EAAUAgeLimit::UnknownAgeRange;
		}
	}else if(ageLimit == 0)
	{
		AgeLimit = EAAUAgeLimit::Child;
	}else if(ageLimit == 8)
	{
		AgeLimit = EAAUAgeLimit::Teen;
	}else if(ageLimit == 16)
	{
		AgeLimit = EAAUAgeLimit::Young;
	}else 
	{
		AgeLimit = EAAUAgeLimit::Adult;
	}
}

