		tm_sec	33	int
		tm_min	50	int
		tm_hour	17	int
		tm_mday	15	int
		tm_mon	9	int
		tm_year	115	int
		tm_wday	4	int
		tm_yday	287	int
		tm_isdst	0	int


		
		

	int isLeapYear(int year)
	{
		int isLeap = 0;

		if (year % 4 == 0 && year % 100 != 0 || year % 400 == 0)
		{
			isLeap = 1;
		}

		return isLeap;
	}

	time_t SubtractTime(time_t baseTime, int seconds)
	{
		time_t newTime = INDEFINITE_TIME;

		if (seconds > 0)
		{
			tm* t = gmtime(&baseTime);

			t->tm_sec = t->tm_sec - seconds;

			if (t->tm_sec < 0)
			{
				t->tm_min = t->tm_min + t->tm_sec / 60 - 1;
				t->tm_sec = 60 + t->tm_sec % 60;

				if (t->tm_min < 0)
				{
					t->tm_hour = t->tm_hour + t->tm_min / 60 - 1;
					t->tm_min = 60 + t->tm_min % 60;

					if (t->tm_hour < 0)
					{
						t->tm_mday = t->tm_mday + t->tm_hour / 24;
						t->tm_hour = 24 + t->tm_hour % 24;

						while (t->tm_mday < 1)
						{
							int daysInMonth;

							if (t->tm_mon == 0)
							{
								t->tm_mon = 11;
								t->tm_year = t->tm_year - 1;
							}
							else
							{
								t->tm_mon = t->tm_mon - 1;
							}

							if (t->tm_mon % 2 == 0)
							{
								daysInMonth = 30;
							}
							else
							{
								if (t->tm_mon != 1)
								{
									daysInMonth = 31;
								}
								else
								{
									if (isLeapYear(t->tm_year))
									{
										daysInMonth = 29;
									}
									else
									{
										daysInMonth = 28;
									}
								}
							}

							t->tm_mday = t->tm_mday + daysInMonth + 1;
						}
					}
				}
			}

			newTime = mktime(t);
		}

		return newTime;
	}

	
	
	
		char *nowstr, *t0str, *t1str, *t2str, *t3str, *t4str, *t5str;
		nowstr = (char*)malloc(sizeof(char) * 40);
		t0str = (char*)malloc(sizeof(char) * 40);
		t1str = (char*)malloc(sizeof(char) * 40);
		t2str = (char*)malloc(sizeof(char) * 40);
		t3str = (char*)malloc(sizeof(char) * 40);
		t4str = (char*)malloc(sizeof(char) * 40);
		t5str = (char*)malloc(sizeof(char) * 40);

		time_t now = time(NULL);
		strcpy(nowstr, ctime(&now));

		time_t t0 = SubtractTime(now, 5);
		strcpy(t0str, ctime(&t0));

		time_t t1 = SubtractTime(now, 60);
		strcpy(t1str, ctime(&t1));

		time_t t2 = SubtractTime(now, 500);
		strcpy(t2str, ctime(&t2));

		time_t t3 = SubtractTime(now, 24*60*60);
		strcpy(t3str, ctime(&t3));

		time_t t4 = SubtractTime(now, 365 * 24 * 60 * 60);
		strcpy(t4str, ctime(&t4));

		time_t t5 = SubtractTime(now, 40 * 365 * 24 * 60 * 60);
		strcpy(t5str, ctime(&t5));

		free(nowstr);
		free(t0str);
		free(t1str);
		free(t2str);
		free(t3str);
		free(t4str);
		free(t5str);
	