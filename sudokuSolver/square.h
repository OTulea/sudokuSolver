#pragma once
/* a square contains its value finalVal, its list of possible values,
and a bool for whether the other instance variables? have been updated. */
class square {
	// make private for security?
public:
	unsigned int finalVal = 0;
	bool updated = false;
	unsigned int possibleVals[9]{ 1,2,3,4,5,6,7,8,9 };
};