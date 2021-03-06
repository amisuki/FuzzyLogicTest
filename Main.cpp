#include "stdafx.h"
#include <Math.h>
#include <assert.h>

//https://qiita.com/keny30827/items/c0b77f3c2b3336d47c70

#define L_PI        (3.1415f)
#define L_2PI       (L_PI * 2)

#define VECTOR SVector2D<float>
template <class T>
struct SVector2D
{
	typedef T DataType;
	T x;
	T y;
	SVector2D() { Init(); }
	void Init()
	{
		x = T();
		y = T();
	}
	SVector2D   operator +  (const SVector2D& e) const { SVector2D tmp; tmp.x = x + e.x; tmp.y = y + e.y; return tmp; }
	SVector2D&  operator += (const SVector2D& e) { x += e.x; y += e.y; return (*this); }
	SVector2D   operator -  (const SVector2D& e) const { SVector2D tmp; tmp.x = x - e.x; tmp.y = y - e.y; return tmp; }
	SVector2D&  operator -= (const SVector2D& e) { x -= e.x; y -= e.y; return (*this); }
	T           operator *  (const SVector2D& e) const { return (x * e.x) + (y * e.y); }
	SVector2D&  operator *= (const int e) { x *= e; y *= e; return (*this); }
	SVector2D&  operator *= (const float e) { x *= e; y *= e; return (*this); }
	SVector2D&  operator /= (const int e) { x /= e; y /= e; return (*this); }
	SVector2D&  operator /= (const float e) { x /= e; y /= e; return (*this); }
};

VECTOR::DataType GetScalar(VECTOR vec)
{
	return sqrtf(vec.x * vec.x + vec.y * vec.y);
}


float FuzzyAnd(float a, float b) { return fminf(a, b); }
float FuzzyOr(float a, float b) { return fmaxf(a, b); }
float FuzzyNot(float a) { return 1.0f - a; }

//대각선 소속함수
float FuzzyGrade(float value, float x0, float x1)
{
	assert(x0 <= x1);
	if (value < x0) { return 0.0f; }
	if (value >= x1) { return 1.0f; }
	if (x0 <= value && value < x1) {
		return (value - x0) / (x1 - x0);
	}
	return -1.0f;
}

//삼각형 소속함수
float FuzzyTriangle(float value, float x0, float x1, float x2)
{
	assert(x0 <= x1);
	assert(x1 <= x2);
	if (value < x0) { return 0.0f; }
	if (value >= x2) { return 0.0f; }
	if (x0 <= value && value < x1) {
		return FuzzyGrade(value, x0, x1);
	}
	if (x1 <= value && value < x2) {
		return FuzzyNot(FuzzyGrade(value, x1, x2));
	}
	return -1.0f;
}

//사다리꼴 소속함수
float FuzzyTrapenoid(float value, float x0, float x1, float x2, float x3)
{
	assert(x0 <= x1);
	assert(x1 <= x2);
	assert(x2 <= x3);
	if (value < x0) { return 0.0f; }
	if (value >= x3) { return 0.0f; }
	if (x1 <= value && value < x2) { return 1.0f; }
	if (x0 <= value && value < x1) {
		return FuzzyGrade(value, x0, x1);
	}
	if (x2 <= value && value < x3) {
		return FuzzyNot(FuzzyGrade(value, x2, x3));
	}
	return -1.0f;
}

//소속함수의 결과값 + 긍정적이라면 2제곱을하여 긍정적으로 변경.
float HedgeVery(float a) { return powf(a, 2.0f); }

//소속함수의 결과값 + 부정적이라면 
float HedgeNotVery(float a) { return powf(a, 0.5f); }


enum STATE_TYPE
{
	추적모드 = 0,
	순찰모드,
	STATE_TYPE_NUM,
	STATE_TYPE_INVALID = -1,
};

int main()
{


	//ファジー集合群.近い，普通，遠いの三状態.
	float distNear = 0.0f;
	float distNormal = 0.0f;
	float distFar = 0.0f;
	{
		VECTOR posA;
		posA.x = 150;
		posA.y = 0;
		
		VECTOR posB;
		posB.x = 40;
		posB.y = 0;
		//クリスプ集合の1要素.「近い」集合の1要素をファジー集合に当てはめる.
		float dist = GetScalar(posA - posB);
		distNear = FuzzyNot(FuzzyGrade(dist, 75.0f, 125.0f));
		//distNear = HedgeVery(distNear);
		distNormal = FuzzyTrapenoid(dist, 75.0f, 125.0f, 200.0f, 250.0f);
		distNormal = HedgeVery(distNormal);
		distNormal = HedgeVery(distNormal);
		distFar = FuzzyGrade(dist, 225.0f, 250.0f);
		float c = HedgeVery(distFar);
	}

	//적의 상태에 의해 추적 도주
	float 추적 = 0.0f;
	float 순찰 = 0.0f;
	{
		//입력시 퍼지집합이 가깝다일 경우 추적값을 알아내기.
		추적 = distNear;
		//입력시 퍼지집합이 보통거리, 먼거리를 or 연산하여 순찰값을 알아내기.
		순찰 = FuzzyOr(distNormal, distFar);
	}

	STATE_TYPE g_nextState;
	float fuzzyValue = 0.0f;
	{
		if (추적 >= 순찰) {
			fuzzyValue = 추적;
			g_nextState = 추적모드;
		}
		else {
			fuzzyValue = 순찰;
			g_nextState = 순찰모드;
		}
	}

    return 0;
}

