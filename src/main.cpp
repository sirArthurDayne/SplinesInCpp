#define OLC_PGE_APPLICATION
#include"olcPixelGameEngine.h"
#include<iostream>


struct Splines
{
	std::vector<olc::v2d_generic<float>> vecPoints2d;
	std::vector<float> vecLength;
	float totalLength;
	//methods
	olc::v2d_generic<float> getSplinePoint(float t, bool isLoop = false)
	{
		//indices base upon t values
		int p0, p1, p2, p3;
		//normal calculations for spline
		if (!isLoop)
		{
			p1 = (int)t + 1;
			p2 = p1 + 1;
			p3 = p1 + 2;
			p0 = p1 - 1;
		}
		else
		{
			//loop calculations, join first and last points to spline
			p1 = (int)t;
			p2 = (p1 + 1) % vecPoints2d.size();
			p3 = (p2 + 1) % vecPoints2d.size();
			p0 = (p1 >= 1) ? p1 - 1 : vecPoints2d.size() - 1;
		}
		
		//down the value back less than 1
		t = t - (int)t;

		//cuadratic & cube values
		float t_square = t * t;
		float t_cube = t_square * t;

		//Ecuations for interpolation (influential fields values)
		float ec1 = -t_cube + 2.0f * t_square - t;
		float ec2 = 3.0f * t_cube - 5.0f * t_square + 2.0f;
		float ec3 = -3.0f * t_cube + 4.0f * t_square + t;
		float ec4 = t_cube - t_square;

		//x,y axis value(multiply by 0.5 to return a value between 1 and 0)
		float x_interpolation = 0.5f * ( vecPoints2d[p0].x * ec1 + vecPoints2d[p1].x * ec2 + vecPoints2d[p2].x * ec3 + vecPoints2d[p3].x * ec4);
		float y_interpolation = 0.5f * ( vecPoints2d[p0].y * ec1 + vecPoints2d[p1].y * ec2 + vecPoints2d[p2].y * ec3 + vecPoints2d[p3].y * ec4);


		return { x_interpolation, y_interpolation };
	}

	olc::v2d_generic<float> getGradientPoint(float t, bool isLoop = false)
	{
		//indices base upon t values
		int p0, p1, p2, p3;
		//normal calculations for spline
		if (!isLoop)
		{
			p1 = (int)t + 1;
			p2 = p1 + 1;
			p3 = p1 + 2;
			p0 = p1 - 1;
		}
		else
		{
			//loop calculations, join first and last points to spline
			p1 = (int)t;
			p2 = (p1 + 1) % vecPoints2d.size();
			p3 = (p2 + 1) % vecPoints2d.size();
			p0 = (p1 >= 1) ? p1 - 1 : vecPoints2d.size() - 1;
		}
		
		//down the value back less than 1
		t = t - (int)t;

		//cuadratic &  values
		float t_square = t * t;

		//Ecuations for interpolation (influential fields values)
		float ec1 = -3.0f * t_square + 4.0f * t - 1.0f;
		float ec2 = 9.0f * t_square - 10.0f * t;
		float ec3 = -9.0f * t_square + 8.0f * t + 1.0f;
		float ec4 = 3.0f * t_square - 2.0f * t;

		//x,y axis value(multiply by 0.5 to return a value between 1 and 0)
		float x_interpolation = 0.5f * ( vecPoints2d[p0].x * ec1 + vecPoints2d[p1].x * ec2 + vecPoints2d[p2].x * ec3 + vecPoints2d[p3].x * ec4);
		float y_interpolation = 0.5f * ( vecPoints2d[p0].y * ec1 + vecPoints2d[p1].y * ec2 + vecPoints2d[p2].y * ec3 + vecPoints2d[p3].y * ec4);


		return { x_interpolation, y_interpolation };
	}
	
	float CalculateSegmentLength(int node, bool isLoop = false)
	{
		float length = 0.0f;
		float stepSize = 0.01f;

		olc::v2d_generic<float> oldPoint, newPoint;
		oldPoint = getSplinePoint((float)node, isLoop);

		for (float t = 0.0f; t < 1.0f; t+= stepSize)
		{
			newPoint = getSplinePoint((float)node + t, isLoop);

			length += sqrtf((newPoint.x - oldPoint.x) * (newPoint.x - oldPoint.x) + (newPoint.y - oldPoint.y) * (newPoint.y - oldPoint.y));
			oldPoint = newPoint;
		}

		return length;
	}

	float getNormalizedOffset(float pos_length)
	{
		//find  node base
		int i = 0;
		while (pos_length > vecLength[i])
		{
			pos_length -= vecLength[i];
			i++;
		}
		//the remainder is the offset
		return (float)i + (pos_length / vecLength[i]);
	}
};


class SplinesPractice : public olc::PixelGameEngine
{
public:
	SplinesPractice() { sAppName = "Splines practice in Cpp"; }

private:
	bool OnUserCreate() override
	{
		for (int i = 1; i <= 9; i++)
		{

			path.vecPoints2d.push_back({ ScreenWidth() /2 + 130.0f * sinf((float)i / 9.0f * 3.141592f * 2.0f), 
										 ScreenHeight()/2 + 130.0f * cosf((float)i / 9.0f * 3.141592f * 2.0f) });

			path.vecLength.push_back(0.0f);

		}
		return true;
	}
	bool OnUserUpdate(float fElapsedTime) override
	{
		Clear(olc::BLACK);



		//input handler
		MouseInput();
		KeyboardInput(50.0f, fElapsedTime);
		AgentInput(fElapsedTime);


		//Draw Splines (always before control points)
		for (float t = 0.0f; t < (float)path.vecPoints2d.size(); t += 0.01f)
		{
			//create a spline point base upon a t value
			olc::v2d_generic<float> position = path.getSplinePoint(t, true);
			Draw(position.x, position.y, olc::RED);
		}

		//draw constrol points on screen
		int size = 10;
		path.totalLength = 0.0f;
		for (int i = 0; i < path.vecPoints2d.size(); i++)
		{
			path.totalLength +=	(path.vecLength[i] = path.CalculateSegmentLength(i, true));
			int xCoord = path.vecPoints2d[i].x;
			int yCoord = path.vecPoints2d[i].y;
			FillRect(xCoord, yCoord, size, size, olc::BLUE);
			DrawString(path.vecPoints2d[i].x, path.vecPoints2d[i].y + size, std::to_string(i), olc::MAGENTA);
			DrawString(path.vecPoints2d[i].x +  10, path.vecPoints2d[i].y + size, std::to_string(path.vecLength[i]), olc::WHITE);
		}

		//highlight selected point
		FillRect(path.vecPoints2d[lineSelected].x, path.vecPoints2d[lineSelected].y, size, size, olc::DARK_GREEN);
		DrawString(path.vecPoints2d[lineSelected].x, path.vecPoints2d[lineSelected].y + size, std::to_string(lineSelected), olc::GREEN);
		DrawString(10, ScreenHeight() - 55, "Line Selected: " + std::to_string(lineSelected), olc::GREEN);

		//Draw Agent on screen
		float offset = path.getNormalizedOffset(marker);
		olc::v2d_generic<float> agentPos = path.getSplinePoint(offset, true);
		olc::v2d_generic<float> agentGrad = path.getGradientPoint(offset, true);
		float alfa = std::atan2f(-agentGrad.y, agentGrad.x);

		DrawLine(agentPos.x + sin(alfa) * 5.0f, agentPos.y + cos(alfa) * 5.0f, agentPos.x + sin(alfa) * -5.0f, agentPos.y + cos(alfa) * -5.0f, olc::YELLOW);
		std::string agentText = "x: " + std::to_string(agentPos.x) + " y: " + std::to_string(agentPos.y) + " offset: " + std::to_string(offset) + " marker:" + std::to_string(marker);
		DrawString(10, ScreenHeight() - 25, agentText, olc::GREEN);
		DrawString(10, 20, "total Length: " + std::to_string(path.totalLength));

		if (GetKey(olc::ESCAPE).bPressed) return false;

		return true;
	}

	void KeyboardInput(float speed, float deltaTime)
	{
		//new Coordinates
		if (GetKey(olc::UP).bHeld) path.vecPoints2d[lineSelected].y -= speed * deltaTime;
		else if (GetKey(olc::DOWN).bHeld) path.vecPoints2d[lineSelected].y += speed * deltaTime;
		if (GetKey(olc::LEFT).bHeld) path.vecPoints2d[lineSelected].x -= speed * deltaTime;
		else if (GetKey(olc::RIGHT).bHeld) path.vecPoints2d[lineSelected].x += speed * deltaTime;

		//out of screen x,y
		if (path.vecPoints2d[lineSelected].y < 0) path.vecPoints2d[lineSelected].y = ScreenHeight() - 1;
		else if (path.vecPoints2d[lineSelected].y > ScreenHeight()) path.vecPoints2d[lineSelected].y = 1;
		if (path.vecPoints2d[lineSelected].x < 0) path.vecPoints2d[lineSelected].x = ScreenWidth() - 1;
		else if (path.vecPoints2d[lineSelected].x > ScreenWidth())path.vecPoints2d[lineSelected].x = 1;



	}

	void MouseInput()
	{
		int mouseX = GetMouseX();
		int mouseY = GetMouseY();

		if (GetMouse(0).bPressed)
			for (int i = 0; i < path.vecPoints2d.size(); i++)
			{
				if (mouseX >= path.vecPoints2d[i].x && mouseX <= path.vecPoints2d[i].x + 10 && mouseY >= path.vecPoints2d[i].y && mouseY <= path.vecPoints2d[i].y + 10)
					lineSelected = i;
			}

		if (GetMouse(0).bHeld)
		{
			path.vecPoints2d[lineSelected].x = (float)mouseX;
			path.vecPoints2d[lineSelected].y = (float)mouseY;
		}


	}

	void AgentInput(float deltaTime)
	{
			//agent marker input
		float speed = 35.0f;
		if (GetKey(olc::A).bHeld) marker -= speed * deltaTime;
		if (GetKey(olc::D).bHeld) marker += speed * deltaTime;

		//agent boundaries
		if (marker >= (float)path.totalLength) marker -= (float)path.totalLength;
		if (marker < 0.0f) marker += (float)path.totalLength;

	}
private:
	Splines path;
	int lineSelected = 0;

	//agent vars
	float marker = 0.0f;
};



int main()
{
	SplinesPractice app;

	if (app.Construct(640, 480, 2, 2))
		app.Start();
	else std::cout << "ERROR LOADING SCREEN";
	
	return 0; 

}