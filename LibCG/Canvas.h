#pragma once

#include "KDMath.h"
#include <vector>

enum class LineCap
{
	Butt,
	Round,
	Square
};

enum class LineJoin
{
	Bevel,
	Round,
	Miter
};

//enum class LineScaleMode
//{
//	None,
//	Normal
//};
//
//enum class FillRule
//{
//	Nonzero,
//	Evenodd
//};

class Canvas
{
public:
	unsigned fillStyle = 0x000000FF;

	unsigned strokeStyle = 0x000000FF;

	float lineWidth = 2.0f;

	LineCap lineCap = LineCap::Butt;

	LineJoin lineJoin = LineJoin::Miter;

	float miterLimit = 10.0f;

	void beginPath();

	void closePath();

	void moveTo(float x, float y);

	void lineTo(float x, float y);

	void rect(float x, float y, float width, float height);

	void circle(float x, float y, float r);

	void arc(float x, float y, float radius, float startAngle, float endAngle, bool counterclockwise = false);

	void ellipse(float x, float y, float width, float height);

	void quadraticCurveTo(float cpx, float cpy, float x, float y);

	void bezierCurveTo(float cp1x, float cp1y, float cp2x, float cp2y, float x, float y);

	void fill();

	void stroke();

	/**
	* @param a Horizontal scaling
	* @param b Vertical skewing
	* @param c Horizontal skewing
	* @param d Vertical scaling
	* @param e Horizontal translation
	* @param f Vertical translation
	*/
	void transform(float a, float b, float c, float d, float e, float f);

	bool isPointInPath(float x, float y, int path_index = -1);

	/**
	* @param vertices [float2 pos, float3 color]
	*/
	void triangulate(std::vector<float>* vertices);

private:
	struct PathState
	{
		bool done = false;
		bool fill = true;
		unsigned color = 0x000000FF;
		float lineWidth = 2.0f;
		LineCap lineCap = LineCap::Butt;
		LineJoin lineJoin = LineJoin::Miter;
		std::vector<vec2<f32>> points;
		std::vector<vec2<f32>> triangles;
	};

	bool nextNewPath = true;

	std::vector<PathState> paths;
	mat2x3<f32> transformMat;

	void tessellateBezier(float startX, float startY, float cp1x, float cp1y,
		float cp2x, float cp2y, float x, float y, int depth);

	void triangulateFill(PathState& path);
	void triangulateEarClip(const std::vector<vec2<f32>>& points, std::vector<vec2<f32>>& triangles);

	void triangulateStroke(PathState& path);
};
