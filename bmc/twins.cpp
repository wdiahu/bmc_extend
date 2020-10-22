#include "utils.h"

extern unsigned long g_misscan;
extern unsigned long g_nMatch;

/*
state_t MethodTwins(DFA* dfa, state_t curState, short* stateArray, TokenInfo* tokenList, int dist, int length)
{
	short* offState = stateArray - dist - 1;
	for (int curPos = 0; curPos < length; curPos++, tokenList++, stateArray++, offState++)
	{
		if (curState == *offState)
		{
			if (dist < length)
			{
				for (int k = 0; k < length - curPos; k++)
				{
					stateArray[k] = offState[k + 1];
					//	if(!dfa->accepts(stateArray[k])->empty())	{g_nMatch++;}
				}
			}
			else
			{
				memcpy(stateArray, offState + 1, sizeof(short) * (length - curPos));
			}
			return offState[length - curPos];
		}
		else
		{
			//	// g_misscan++;
			*stateArray = curState = ScanByte(dfa, curState, tokenList->token);
		}
	}
	return curState;
}

state_t MethodTwinsAlt(DFA* dfa, state_t curState, short* stateArray, TokenInfo* tokenList, int dist, int length)
{
	short* offState = stateArray - dist;
	*stateArray = curState = ScanByte(dfa, curState, tokenList->token);
	stateArray++; tokenList++;

	for (int curPos = 1; curPos < length; curPos++, tokenList++, stateArray++, offState++)
	{
		if (curState == *offState)
		{
			if (0 < length)
			{
				for (int k = 0; k < length - curPos; k++)
				{
					stateArray[k] = offState[k + 1];
					if (!dfa->accepts(stateArray[k])->empty()) { g_nMatch++; }
				}
			}
			else
			{
				memcpy(stateArray, offState + 1, sizeof(short) * (length - curPos));
			}
			return offState[length - curPos];
		}
		else
		{
			//	g_misscan++;
			*stateArray = curState = ScanByte(dfa, curState, tokenList->token);
		}
	}
	return curState;
}

state_t MethodNaive(DFA* dfa, state_t curState, short* stateArray, TokenInfo* tokenList, int dist, int length)
{
	TokenInfo* offsetList = tokenList - dist;
	for (int k = 0; k < length; k++, offsetList++, tokenList++)
	{
		*stateArray = curState = ScanByte(dfa, curState, offsetList->token);
	}

	return curState;
}
*/