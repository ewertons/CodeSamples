
#include "agenttime.h";

typedef struct {
	int Days;
	int Hours;
	int Minutes;
	int Seconds;
	int Milliseconds;
} TimeSpan;

TimeSpan CreateTimeSpan(int hours, int minutes, int seconds)
{
	TimeSpan t;
	t.Hours = hours;
	t.Minutes = minutes;
	t.Seconds = seconds;

	NormalizeTimeSpan(&t);

	return t;
}

long GetTimespanTotalSeconds(TimeSpan* t)
{
	long totalSec = 0;

	if (t != NULL)
	{
		totalSec = t->Days * 24;
		totalSec = (totalSec + t->Hours) * 60;
		totalSec = (totalSec + t->Minutes) * 60;
		totalSec += t->Seconds;
	}

	return totalSec;
}

long GetTimespanTotalMilliseconds(TimeSpan* t)
{
	long totalMillisecs = 0;

	if (t != NULL)
	{
		totalMillisecs = GetTimespanTotalSeconds(t) + t->Milliseconds;
	}

	return totalMillisecs;
}

void NormalizeTimeSpan(TimeSpan *t)
{
	if (t != NULL)
	{
		long totalMillisecs = GetTimespanTotalMilliseconds(t);
		t->Days = (totalMillisecs / 86400000); // 1000ms * 60sec * 60mins * 24hours
		totalMillisecs = (totalMillisecs % 86400000);
		t->Hours = (totalMillisecs / 3600000); // 1000ms * 60sec * 60mins
		totalMillisecs = (totalMillisecs % 3600000);
		t->Minutes = (totalMillisecs / 60000); // 1000ms * 60sec
		totalMillisecs = (totalMillisecs % 60000);
		t->Seconds = (totalMillisecs / 1000); // 1000ms
		totalMillisecs = (totalMillisecs % 1000);
		t->Milliseconds = totalMillisecs;
	}
}

int DiffTimeSpan(TimeSpan* t0, TimeSpan* t1)
{
	
}






// Returns values:
//     Invalid arguments:    -1
//     Timeout was reached:   0
//     Function succeded:   > 1
int RunUntilSuccessOrTimeout(double timeoutInSecs, TimeoutFunction timeoutFunction)
{
	int result = -1;

	if (timeoutInSecs > 0 && timeoutFunction != NULL)
	{
		double timeoutInClocks = timeoutInSecs / CLOCKS_PER_SEC;

		result = 0;
		clock_t t0 = clock();

		while ((clock() - t0) <= timeoutInClocks && (result = timeoutFunction()) <= 0) 
		{
			ThreadAPI_Sleep(1);
		}

		// Normalizing the result...
		if (result < 0) result = 0;
	}

	return result;
}
