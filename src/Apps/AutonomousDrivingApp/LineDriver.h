#ifndef WHEELSON_FIRMWARE_LINEDRIVER_H
#define WHEELSON_FIRMWARE_LINEDRIVER_H


#include "../../Components/Driver.h"
#include <opencv2/core/types.hpp>
#include <opencv2/core/mat.hpp>

class LineDriver : public Driver {
public:
	void toggleDisplayMode() override;
	void draw() override;

protected:
	void process() override;

private:
	std::vector<cv::Point2i> brokenLineResult;
	int midIndex = 0;

	enum DisplayMode {
		RAW,
		GRAY,
		THRESH_SIMPLE,
		//THRESH_ADAPTIVE,
		COUNT
	} displayMode = RAW;

	int lastx = -1;

	void rotL();
	void rotR();

	cv::Mat drawMat;

	void drawLine(int x1, int y1, int x2, int y2, Color* buffer,uint32_t color);
};


#endif //WHEELSON_FIRMWARE_LINEDRIVER_H
