#include "CriticalSection.h"
#include "InputState.h"

void*** const APPLICATION = (void***)0x1E79048;

InputState* getInputState()
{
	CriticalSection criticalSection;
	criticalSection.enter(*APPLICATION);

	InputState* result = (InputState*)(84 * *(size_t*)((size_t)criticalSection.value + 0xD74) + (size_t)criticalSection.value + 4);

	criticalSection.leave();
	return result;
}
