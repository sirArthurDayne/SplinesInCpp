#define OLC_PGE_APPLICATION
#include"olcPixelGameEngine.h"
#include<iostream>


struct Splines
{
	std::vector<olc::v2d_generic<float>> vecPoints2d;

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
};


class SplinesPractice : public olc::PixelGameEngine
{
public:
	SplinesPractice() { sAppName = "Splines practice in Cpp"; }

private:
	bool OnUserCreate() override
	{
		for (int i = 1; i <= 8; i++)
		{
			olc::v2d_generic<float> point( i * 50,100);
			path.vecPoints2d.push_back(point);
		}

		return true;
	}
	bool OnUserUpdate(float deltaTime) override
	{
		Clear(olc::BLACK);



		//input handler
		if		(GetKey(olc::W).bPressed) lineSelected++;
		else if (GetKey(olc::S).bPressed) lineSelected--;
		if (lineSelected > path.vecPoints2d.size() - 1 || lineSelected < 0) lineSelected = 0;

		//new Coordinates
		float speed = 50.0;
		if (GetKey(olc::UP).bHeld) path.vecPoints2d[lineSelected].y			-= speed * deltaTime;
		else if (GetKey(olc::DOWN).bHeld) path.vecPoints2d[lineSelected].y  += speed * deltaTime;
		else if (GetKey(olc::LEFT).bHeld) path.vecPoints2d[lineSelected].x  -= speed * deltaTime;
		else if (GetKey(olc::RIGHT).bHeld) path.vecPoints2d[lineSelected].x += speed * deltaTime;

		//out of screen x,y
		if (path.vecPoints2d[lineSelected].y < 0) path.vecPoints2d[lineSelected].y = ScreenHeight() - 1;
		else if (path.vecPoints2d[lineSelected].y > ScreenHeight()) path.vecPoints2d[lineSelected].y = 1;
		if (path.vecPoints2d[lineSelected].x < 0) path.vecPoints2d[lineSelected].x = ScreenWidth() - 1;
		else if (path.vecPoints2d[lineSelected].x > ScreenWidth())path.vecPoints2d[lineSelected].x = 1;

		//Draw Splines (always before control points)
		for (float t = 0.0f; t < (float)path.vecPoints2d.size(); t+= 0.01f)
		{
			//create a spline point base upon a t value
			olc::v2d_generic<float> position = path.getSplinePoint(t, true);
			Draw(position.x, position.y, olc::RED);
		}
		//draw constrol points on screen

		int size = 10;
		for (int i = 0; i < path.vecPoints2d.size(); i++)
		{
			int xCoord = path.vecPoints2d[i].x;
			int yCoord = path.vecPoints2d[i].y;
			FillRect(xCoord, yCoord, size, size, olc::BLUE);
			DrawString(path.vecPoints2d[i].x, path.vecPoints2d[i].y + size, std::to_string(i), olc::WHITE);
		}
		//highlight selected point
		FillRect(path.vecPoints2d[lineSelected].x, path.vecPoints2d[lineSelected].y, size, size, olc::GREEN);
		DrawString(path.vecPoints2d[lineSelected].x, path.vecPoints2d[lineSelected].y + size, std::to_string(lineSelected), olc::WHITE);
		DrawString(10, ScreenHeight() - 25, "Line Selected: " + std::to_string(lineSelected), olc::GREEN);


		if (GetKey(olc::ESCAPE).bPressed) return false;

		return true;
	}
private:
	Splines path;
	int lineSelected = 0;
};



int main()
{
	SplinesPractice app;

	if (app.Construct(640, 480, 2, 2))
		app.Start();
	else std::cout << "ERROR LOADING SCREEN";
	
	return 0; 

}