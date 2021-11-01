#include "Canvas.h"
#include "Util.h"
#include <list>
#include <algorithm>

void Canvas::beginPath()
{
	nextNewPath = false;
	paths.push_back(PathState());
}

void Canvas::closePath()
{
	nextNewPath = true;
}

void Canvas::moveTo(float x, float y)
{
	lineTo(x, y);
}

void Canvas::lineTo(float x, float y)
{
	if (nextNewPath)
		beginPath();

	vec2<f32> p = transform_point(transformMat, x, y);
	PathState& path = paths.back();
	path.points.push_back({ p.x, p.y });
}

void Canvas::rect(float x, float y, float width, float height)
{
	moveTo(x, y);
	lineTo(x + width, y);
	lineTo(x + width, y + height);
	lineTo(x, y + height);
	lineTo(x, y);
}

void Canvas::circle(float x, float y, float r)
{
	arc(x, y, r, 0, F32_2PI, false);
}

void Canvas::arc(float x, float y, float radius, float startAngle, float endAngle, bool counterclockwise)
{
	float angle = endAngle - startAngle;
	if (angle <= 0) return;
	if (angle >= F32_2PI)
	{
		startAngle = 0;
		endAngle = F32_2PI;
		angle = F32_2PI;
	}

	//圆心
	lineTo(x, y);

	//旋转依据点
	vec2<f32> p = vec2<f32>(radius, 0);
	//细分若干次
	static float tessAngle = radians(15.f);
	for (float i = startAngle; i < endAngle; i = i + tessAngle)
	{
		float theta = i;
		float sinTheta = sin(theta);
		float cosTheta = cos(theta);
		float _x = p.x * cosTheta + p.y * sinTheta;
		float _y = p.x * sinTheta + p.y * cosTheta;
		lineTo(x + _x, y + _y);
	}

	//补余角
	if (int(degrees(angle)) % int(degrees(tessAngle)))
	{
		float theta = endAngle;
		float sinTheta = sin(theta);
		float cosTheta = cos(theta);
		float _x = p.x * cosTheta + p.y * sinTheta;
		float _y = p.x * sinTheta + p.y * cosTheta;
		lineTo(x + _x, y + _y);
	}
}

void Canvas::ellipse(float x, float y, float width, float height)
{
	moveTo(x - width, y);

	const static float KAPPA90 = 0.5522847493f;
	bezierCurveTo(x - width, y + height * KAPPA90, x - width * KAPPA90, y + height, x, y + height);
	bezierCurveTo(x + width * KAPPA90, y + height, x + width, y + height * KAPPA90, x + width, y);
	bezierCurveTo(x + width, y - height * KAPPA90, x + width * KAPPA90, y - height, x, y - height);
	bezierCurveTo(x - width * KAPPA90, y - height, x - width, y - height * KAPPA90, x - width, y);
}

void Canvas::quadraticCurveTo(float cpx, float cpy, float x, float y)
{
	vec2<f32> sp = paths.back().points.back();
	float x0 = sp.x;
	float y0 = sp.y;
	bezierCurveTo(x0 + 2.0f / 3.0f * (cpx - x0), y0 + 2.0f / 3.0f * (cpy - y0),
		x + 2.0f / 3.0f * (cpx - x), y + 2.0f / 3.0f * (cpy - y),
		x, y);
}

void Canvas::bezierCurveTo(float cp1x, float cp1y, float cp2x, float cp2y, float x, float y)
{
	vec2<f32> sp = paths.back().points.back();
	vec2<f32> cp1 = transform_point(transformMat, cp1x, cp1y);
	vec2<f32> cp2 = transform_point(transformMat, cp2x, cp2y);
	vec2<f32> p = transform_point(transformMat, x, y);
	tessellateBezier(sp.x, sp.y, cp1.x, cp1.y, cp2.x, cp2.y, p.x, p.y, 0);
}

void Canvas::fill()
{
	PathState& path = paths.back();
	//自动闭合
	if (path.points[0] != path.points.back())
	{
		path.points.push_back({ path.points[0].x, path.points[0].y });
	}

	path.done = true;
	path.fill = true;
	path.color = fillStyle;
}

void Canvas::stroke()
{
	PathState& path = paths.back();
	path.done = true;
	path.fill = false;
	path.color = strokeStyle;
	path.lineWidth = lineWidth;
	path.lineCap = lineCap;
	path.lineJoin = lineJoin;
}

void Canvas::transform(float a, float b, float c, float d, float e, float f)
{
	transformMat = mat2x3<f32>(a, b, c, d, e, f);
}

void Canvas::tessellateBezier(float x1, float y1, float x2, float y2,
	float x3, float y3, float x4, float y4, int depth)
{
	if (depth > 10)
		return;

	float x12 = (x1 + x2) * 0.5f;
	float y12 = (y1 + y2) * 0.5f;
	float x23 = (x2 + x3) * 0.5f;
	float y23 = (y2 + y3) * 0.5f;
	float x34 = (x3 + x4) * 0.5f;
	float y34 = (y3 + y4) * 0.5f;
	float x123 = (x12 + x23) * 0.5f;
	float y123 = (y12 + y23) * 0.5f;

	float dx = x4 - x1;
	float dy = y4 - y1;
	float d2 = abs(((x2 - x4) * dy - (y2 - y4) * dx));
	float d3 = abs(((x3 - x4) * dy - (y3 - y4) * dx));

	if ((d2 + d3) * (d2 + d3) < (dx * dx + dy * dy) * 0.25f)
	{
		PathState& path = paths.back();
		path.points.push_back({ x4, y4 });
		return;
	}

	float x234 = (x23 + x34) * 0.5f;
	float y234 = (y23 + y34) * 0.5f;
	float x1234 = (x123 + x234) * 0.5f;
	float y1234 = (y123 + y234) * 0.5f;

	tessellateBezier(x1, y1, x12, y12, x123, y123, x1234, y1234, depth++);
	tessellateBezier(x1234, y1234, x234, y234, x34, y34, x4, y4, depth++);
}

bool Canvas::isPointInPath(float x, float y, int path_index)
{
	for (auto& path : paths)
	{
		for (size_t i = 1; i < path.triangles.size() - 1; i++)
		{
			bool hit = isPointInTriangle(vec2<f32>(x, y), path.triangles[i - 1], path.triangles[i], path.triangles[i + 1]);
			if (hit) return true;
		}
	}
	return false;
}

void Canvas::triangulate(std::vector<float>* vertices)
{
	//输出三角面顺时针绕序
	
	for (int i = 0; i < paths.size(); i++)
	{
		PathState& path = paths[i];
		if (path.done)
		{
			if (path.fill)
				triangulateFill(path);
			else
				triangulateStroke(path);
		}

		//顶点添加z和颜色
		vec4<f32> c = hex2rgba(path.color);
		for (u32 j = 0; j < path.triangles.size(); j++)
		{
			vertices->push_back(path.triangles[j].x);
			vertices->push_back(path.triangles[j].y);
			vertices->push_back(c.x);
			vertices->push_back(c.y);
			vertices->push_back(c.z);
		}
	}
}

void Canvas::triangulateFill(PathState& path)
{
	auto& points = path.points;
	auto& triangles = path.triangles;
	if (points.size() < 3) return;

	//保证路径顺时针绕序
	bool isClockwisePolygon = true;
	{
		float sum = 0.0;
		for (size_t i = 0; i < points.size() - 1; i++)
		{
			auto& p1 = points[i];
			auto& p2 = points[i + 1];
			sum += (p2.x - p1.x) * (p2.x + p1.x);
		}
		if (points[0] == points.back())
		{
			auto& p1 = points[points.size() - 1];
			auto& p2 = points[0];
			sum += (p2.x - p1.x) * (p2.x + p1.x);
		}
		isClockwisePolygon = sum <= 0;
	}
	if (!isClockwisePolygon)
	{
		std::reverse(points.begin(), points.end());
	}
	
	//判断凸多边形 线段不改变绕序
	bool isCovexPolygon = true;
	{
		bool isConvex = isConvexAngle(points[0], points[1], points[2]);
		for (size_t i = 2; i < points.size() - 1; i++)
		{
			if (isConvex != isConvexAngle(points[i - 1], points[i], points[i + 1]))
			{
				isCovexPolygon = false;
				break;
			}
		}
	}
	
	if (isCovexPolygon)
	{
		triangles.reserve((points.size() - 2) * 3);
		for (u32 i = 1; i < points.size() - 1; i++)
		{
			triangles.push_back(points[0]);
			triangles.push_back(points[i]);
			triangles.push_back(points[i + 1]);
		}
	}
	else
	{
		triangles.reserve((points.size() - 2) * 3); //不准确
		//凹多边形三角化 耳切法
		triangulateEarClip(path.points, path.triangles);
	}
}

void Canvas::triangulateEarClip(const std::vector<vec2<f32>>& points, std::vector<vec2<f32>>& triangles)
{
	//需要一个双向循环列表，快速删除+循环遍历
	std::list<vec2<f32>> vertexlist;
	for (size_t i = 0; i < points.size(); i++)
	{
		vertexlist.push_back(points[i]);
	}

	auto it = vertexlist.begin();
	++it;
	while (vertexlist.size() > 3)
	{
		auto itPre = it;
		--itPre;

		//结尾自动连接到首位置
		if (it == vertexlist.end())
			it = vertexlist.begin();

		auto itNext = it;
		++itNext;
		if (itNext == vertexlist.end())
			itNext = vertexlist.begin();

		vec2<f32>& p1 = *itPre;
		vec2<f32>& p2 = *it;
		vec2<f32>& p3 = *itNext;

		//凸点才可以转化三角面
		if (isConvexAngle(p1, p2, p3))
		{
			bool isEar = true;
			//判断耳朵，多边形所有顶点都不在这个三角面里面
			auto itChecker = itNext;
			++itChecker;
			if (itChecker == vertexlist.end())
				itChecker = vertexlist.begin();
			while (itChecker != itPre)
			{
				vec2<f32> pChecker = *itChecker;
				if (isPointInTriangle(pChecker, p1, p2, p3))
				{
					isEar = false;
					break;
				}

				++itChecker;
				if (itChecker == vertexlist.end())
					itChecker = vertexlist.begin();
			}

			if (isEar)
			{
				//耳朵确定一个三角面。并且从列表移除
				triangles.push_back(p1);
				triangles.push_back(p2);
				triangles.push_back(p3);

				it = vertexlist.erase(it);
			}
			else
			{
				++it;
			}
		}
		else
		{
			++it;
		}
	}

	//最后一个三角面
	it = vertexlist.begin();
	triangles.push_back(*it++);
	triangles.push_back(*it++);
	triangles.push_back(*it++);
}

void Canvas::triangulateStroke(PathState& path)
{
	auto& points = path.points;
	auto& triangles = path.triangles;
	if (points.size() < 2) return;

	std::vector<vec2<f32>> vertices;
	vertices.reserve(points.size() * 2);

	//支持单独缩放一个轴
	vec2<f32> thickness(path.lineWidth * 0.5f);

	//线段不闭合，才有端点cap
	bool isClose = (points[0] == points.back());

	//起点
	if (isClose)
	{
		vec2<f32> vecA = normalize(points[0] - points[points.size() - 2]).perpendicular();
		vec2<f32> vecB = normalize(points[1] - points[0]).perpendicular();
		vec2<f32> vecC = (vecA + vecB) * 0.5f;

		vec2<f32> vec = vecC / (vecC.x * vecC.x + vecC.y * vecC.y);
		vertices.push_back(points[0] + vec * thickness);
		vertices.push_back(points[0] - vec * thickness);
	}
	else
	{
		if (path.lineCap == LineCap::Round)
		{
			vec2<f32> vec = (points[1] - points[0]).perpendicular();
			vec = normalize(vec);
			vec2<f32> n = vec;

			//18度一次细分
			float sinTheta = 0.309f;
			float cosTheta = 0.951f;
			for (int i = 0; i < 11; i++)
			{
				vertices.push_back(points[0]);
				vertices.push_back(points[0] - n * thickness);

				//旋转
				float x = n.x * cosTheta - n.y * sinTheta;
				float y = n.x * sinTheta + n.y * cosTheta;
				n.x = x;
				n.y = y;
			}

			//补点让下一点连接起来
			vertices.push_back(points[0] + vec * thickness);
			vertices.push_back(points[0] - vec * thickness);
		}
		else
		{
			//连线的垂直方向缩放线宽
			vec2<f32> vec = (points[1] - points[0]).perpendicular();
			vec = normalize(vec);
			vertices.push_back(points[0] + vec * thickness);
			vertices.push_back(points[0] - vec * thickness);
		}
	}

	//拐点
	for (size_t i = 1; i < points.size() - 1; i++)
	{
		vec2<f32> v1 = normalize(points[i] - points[i - 1]);
		vec2<f32> v2 = normalize(points[i + 1] - points[i]);

		//拐点挤出向量
		vec2<f32> vec = (v1 + v2).perpendicular();
		vec = normalize(vec);

		//拐点挤出长度 = cosTheta * 线宽/2
		int outSign = cross(v1, v2) > 0 ? 1 : -1; //判断内外测
		auto v1p = v1.perpendicular() * outSign;
		vec = vec / dot(vec, v1p);
		vertices.push_back(points[i] + vec * outSign * thickness);
		vertices.push_back(points[i] - vec * outSign * thickness);
	}

	//结束点
	if (isClose)
	{
		size_t i = points.size() - 1;
		vec2<f32> vecA = normalize(points[i] - points[i - 1]).perpendicular();
		vec2<f32> vecB = normalize(points[1] - points[i]).perpendicular();
		vec2<f32> vecC = (vecA + vecB) * 0.5f;

		vec2<f32> vec = vecC / (vecC.x * vecC.x + vecC.y * vecC.y);
		vertices.push_back(points[i] + vec * thickness);
		vertices.push_back(points[i] - vec * thickness);
	}
	else
	{
		size_t i = points.size() - 1;

		if (path.lineCap == LineCap::Round)
		{
			vec2<f32> vec = (points[i] - points[i - 1]).perpendicular();
			vec = normalize(vec);
			vec2<f32> n = vec;

			vertices.push_back(points[i] + vec * thickness);
			vertices.push_back(points[i] - vec * thickness);

			float sinTheta = 0.309f;
			float cosTheta = 0.951f;
			for (int j = 0; j < 11; j++)
			{
				vertices.push_back(points[i]);
				vertices.push_back(points[i] + n * thickness);

				float x = n.x * cosTheta - n.y * sinTheta;
				float y = n.x * sinTheta + n.y * cosTheta;
				n.x = x;
				n.y = y;
			}
		}
		else
		{
			vec2<f32> vec = (points[i] - points[i - 1]).perpendicular();
			vec = normalize(vec);
			vertices.push_back(points[i] + vec * thickness);
			vertices.push_back(points[i] - vec * thickness);
		}
	}

	//Strip -> List
	triangles.reserve((vertices.size() - 2) * 3);
	for (size_t i = 1; i < vertices.size() - 1; i++)
	{
		triangles.push_back(vertices[i - 1]);
		triangles.push_back(vertices[i]);
		triangles.push_back(vertices[i + 1]);
	}
}
