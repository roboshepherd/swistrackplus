#include "Utility.h"
#define THISCLASS Utility

double THISCLASS::SquareDistance(CvPoint2D32f p1, CvPoint2D32f p2) {
	double dx = p1.x - p2.x;
	double dy = p1.y - p2.y;
	return dx*dx + dy*dy;
}

CvRect THISCLASS::RectByCenter(int centerX, int centerY, int width, int height) {
	int lowerLeftx = centerX - width / 2;
	int lowerLefty = centerY - width / 2;
	return cvRect(lowerLeftx, lowerLefty, width, height);
}



double THISCLASS::Trunc(double value, int decimal_places)
{
    double integer = 0,
           fractional = 0,
           output = 0;

    int j = 0,
        places = 1;

    fractional = modf(value, &output);
    for( int i = 0; i < decimal_places + 1; i++ )
    {
        fractional = modf(fractional, &integer);
        for( j = 0; j < i; j++ )
        {
            places *= 10;
        }
        output += integer / places;
        places = 1;
        fractional *= 10;
    }
    return output;
}
