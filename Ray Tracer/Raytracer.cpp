#include <iostream>
#include <cmath>
#include <vector>
#include <glm/glm.hpp>
#define MAX_RECURSIVE 3

using namespace std;
using namespace glm;

struct Camera {
	// USE GLM VECTOR
	vec3 lookAt; // camera direction/ ray direction/ -w
	vec3 pos;
	vec3 up; // orientation
	vec3 u; // u vector
	vec3 v; // v vector
	float l, r, top, bottom;

	Camera() {}
	Camera(vec3& e, vec3& target, vec3& up_, int width, int height) {
		pos = e;
		lookAt = normalize(target);
		up = up_;
		u = cross(up, -lookAt);
		v = cross(-lookAt, u);
		l = -500 / 2;
		r = 500/2;
		top = 500/2;
		bottom = -500 / 2;
	}
};

class Shape {
public:
	vec3 color;
	vec3 normal;
	float t1 = 0.0f;
	bool isGlaze = false;

	virtual ~Shape() {}
};

class Plane : public Shape {
	vec3 v1, v2, v3, v4;
public:
	Plane() {
		v1 = vec3(-150, 300, -110);
		v2 = vec3(500, 300, -110);
		v3 = vec3(-250, -300, -110);
		v4 = vec3(500, -300, -110);
		color = vec3(150, 200, 255);
		isGlaze = true;
	}

	bool planeIntersect(vec3& rayOrigin, vec3& rayDir) {
		normal = normalize(cross(v2 - v1, v3 - v1));
		float d = -dot(normal, v1);
		if (fabs(dot(rayDir, normal)) < 0.00000001f)
			return false;

		t1 = -(dot(normal, rayOrigin) + d) / dot(rayDir, normal);
		if (t1 < 0)
			return false;
		vec3 point = rayOrigin + (t1 * rayDir);
		float test0 = dot(cross(v2 - v1, point - v1), normal);
		float test1 = dot(cross(v3 - v4, point - v4), normal);
		float test2 = dot(cross(v1 - v3, point - v3), normal);
		float test3 = dot(cross(v4 - v2, point - v2), normal);

		return (test0 > 0 && test1 > 0 && test2 > 0 && test3 > 0);
	}
};

class Sphere : public Shape {
	vec3 pos; // center
	float radius;

public:
	Sphere() {
		pos = vec3(-50.0f, 80.0f, -60.0f);
		color = vec3(200, 0, 255); // purple
		radius = 50.0f;
	}
	Sphere(vec3& e, vec3& c) {
		pos = e;
		color = c;
		radius = 90.0f;
	}
	
	vec3 getPos() { return pos; }
	float getRadius() { return radius; }

	bool intersect(vec3& rayOrigin, vec3& rayDir) {
		vec3 dist = rayOrigin - pos;
		float a = dot(rayDir, rayDir);
		float b = 2.0f * dot(dist, rayDir);
		
		float c = dot(dist, dist) - (radius * radius);
		float discriminant = b * b - 4 * a * c;
		if (discriminant < 0)
			return false;
		else {
			t1 = (-b - sqrt(discriminant)) / (2.0f * a);
			if (t1 < 0)
				return false;
			vec3 point = rayOrigin + (t1 * rayDir);
			normal = normalize(point - pos);
			return true;
		}
	}
};

class Tetrahedron : public Shape {
	vec3 v1, v2, v3, v4;
	vector<vector<vec3>> tStrip;
public:
	float t2, t3, t4;
	vec3 n2, n3, n4;

	Tetrahedron() {
		v1 = vec3(40.0f, 100.0f, -110.0f);
		v2 = vec3(120.0f, 100.0f, -10.0f);
		v3 = vec3(120.0f, 50.0f, -60.0f);
		v4 = vec3(200.0f, 100.0f, -110.0f);
		color = vec3(250, 200, 0);
		t1 = t2 = t3 = t4 = 0.0f;

		tStrip = {
			{v1, v2, v3},
			{v2, v3, v4},
			{v3, v4, v1},
			{v4, v1, v2}
		};
	}
	
	vector<vector<vec3>> getTStrip() { return tStrip; }

	bool intersect(vec3& rayOrigin, vec3& rayDir, vector<vec3>& face, int index) {
		t1 = t2 = t3 = t4 = 0.0f;

		vec3 u = face[1] - face[0];
		vec3 v = face[2] - face[0];
		vec3 n = normalize(cross(u, v));

		float d = dot(-n, face[0]);
		if (fabs(dot(rayDir, n)) < 0.00000001f)
			return false;

		vec3 point;
		switch (index) {
		case 0:
			t1 = -(dot(n, rayOrigin) + d) / dot(rayDir, n);
			if (t1 < 0)
				return false;
			normal = -n;
			point = rayOrigin + (t1 * rayDir);
			break;
		case 1:
			t2 = -(dot(n, rayOrigin) + d) / dot(rayDir, n);
			if (t2 < 0)
				return false;
			n2 = -n;
			point = rayOrigin + (t2 * rayDir);
			break;
		case 2:
			t3 = -(dot(n, rayOrigin) + d) / dot(rayDir, n);
			if (t3 < 0)
				return false;
			n3 = -n;
			point = rayOrigin + (t3 * rayDir);
			break;
		case 3:
			t4 = -(dot(n, rayOrigin) + d) / dot(rayDir, n);
			if (t4 < 0)
				return false;
			n4 = -n;
			point = rayOrigin + (t4 * rayDir);
			break;
		}
		float test0 = dot(cross(face[1] - face[0], point - face[0]), n);
		float test1 = dot(cross(face[2] - face[1], point - face[1]), n);
		float test2 = dot(cross(face[0] - face[2], point - face[2]), n);

		return (test0 > 0 && test1 > 0 && test2 > 0);
	}
};

struct Light {
	float intensityD;
	float intensityA;
	float intensityS;
	vec3 dir;
	Light() {
		intensityD = 0.5f;
		intensityA = 0.3f;
		intensityS = 0.75f;
		dir = normalize(vec3(1.0f, 1.5f, -1.0f));
	}
};

class RayTracer {
	int width;
	int height;
	bool isOrtho;

	Sphere s1, s2;
	Tetrahedron tetra;
	Camera cam;
	Light light;
	Plane plane;
public:
	RayTracer(int w, int h, vec3& e, vec3& target, vec3& u, bool isOrtho_) {
		width = w;
		height = h;
		isOrtho = isOrtho_;

		s1 = Sphere();
		// temp define pos and color of s2
		vec3 pos2(20.0f, 230.0f, -20.0f);
		vec3 c(0, 200, 150); // green
		s2 = Sphere(pos2, c);
		tetra = Tetrahedron();
		cam = Camera(e, target, u, w, h);
		light = Light();
		plane = Plane();
	}

	bool getIsOrtho() { return isOrtho; }
	void setIsOrtho(bool isOrtho_) { isOrtho = isOrtho_; }
	Camera getCam() { return cam; }

	vec3 makeRay(int i, int j) {
		// rayOrigin = e + u x + v y
		float u = cam.l + (cam.r - cam.l) * (j + 0.5f) / width;
		float v = cam.bottom + (cam.top - cam.bottom) * (i + 0.5f) / height;

		vec3 rayOrigin;
		vec3 rayDir;
		if (isOrtho) {
			rayOrigin = cam.pos + u * cam.u + v * cam.v;
			rayDir = normalize(cam.lookAt);
		}
		else {
			rayOrigin = cam.pos;
			float d = 350.0f;
			rayDir = normalize((-d) * (-cam.lookAt) + u * cam.u + v * cam.v);
		}
		return trace(rayOrigin, rayDir, 0);
	}

	bool shadow(vec3& point) {
		vec3 reLightDir = -light.dir;
		point += 0.00000001f * reLightDir;
		return (s1.intersect(point, reLightDir) || s2.intersect(point, reLightDir) ||
			tetra.intersect(point, reLightDir, tetra.getTStrip()[0], 0) || tetra.intersect(point, reLightDir, tetra.getTStrip()[1], 1)
			|| tetra.intersect(point, reLightDir, tetra.getTStrip()[2], 2) || tetra.intersect(point, reLightDir, tetra.getTStrip()[3], 3));
	}

	vec3 diffuse(Shape *s, int tIndex = 0) {
		vec3 diffColor;
		if (tIndex == 0) {
			diffColor = s->color * light.intensityD * std::max(0.0f, dot(s->normal, -light.dir));
		}
		else if (tIndex == 1 || tIndex == 2 || tIndex == 3) {
			Tetrahedron *tetra = dynamic_cast<Tetrahedron*>(s);
			if (tetra != nullptr) {
				if (tIndex == 1) {
					diffColor = s->color * light.intensityD * std::max(0.0f, dot(tetra->n2, -light.dir));
				}
				else if (tIndex == 2) {
					diffColor = s->color * light.intensityD * std::max(0.0f, dot(tetra->n3, -light.dir));
				}
				else if (tIndex == 3) {
					diffColor = s->color * light.intensityD * std::max(0.0f, dot(tetra->n4, -light.dir));
				}
			}
		}
		return clamp(diffColor, 0.0f, 255.0f);
	}

	vec3 ambient(Shape* s) {
		vec3 ambColor = s->color * light.intensityA;
		return clamp(ambColor, 0.0f, 255.0f);
	}

	vec3 blingPhong(vec3& rayDir, Shape* s, int n) {
		vec3 vH = (-light.dir - rayDir) / length(-light.dir - rayDir);
		vec3 phongColor = vec3(250, 250, 250) * light.intensityS * pow(std::max(0.0f, dot(s->normal, vH)), (float)n);
		return clamp(phongColor, 0.0f, 255.0f);
	}

	// returns rgb of current pixel
	vec3 trace(vec3& rayOrigin, vec3&rayDir, const int depth) {
		float tmin = INFINITY;
		Shape* shape = nullptr;
		int tIndex = -1;
		if (tetra.intersect(rayOrigin, rayDir, tetra.getTStrip()[0], 0)) {
			if (tetra.t1 < tmin) {
				tIndex = 0;
				shape = &tetra;
				tmin = tetra.t1;
			}
		}
		if (tetra.intersect(rayOrigin, rayDir, tetra.getTStrip()[1], 1)) {
			if (tetra.t2 < tmin) {
				tIndex = 1;
				shape = &tetra;
				tmin = tetra.t2;
			}
		}
		if (tetra.intersect(rayOrigin, rayDir, tetra.getTStrip()[2], 2)) {
			if (tetra.t3 < tmin) {
				tIndex = 2;
				shape = &tetra;
				tmin = tetra.t3;
			}
		}
		if (tetra.intersect(rayOrigin, rayDir, tetra.getTStrip()[3], 3)) {
			if (tetra.t4 < tmin) {
				tIndex = 3;
				shape = &tetra;
				tmin = tetra.t4;
			}
		}
		if (s1.intersect(rayOrigin, rayDir)) {
			if (s1.t1 < tmin) {
				tIndex = 4;
				shape = &s1;
				tmin = s1.t1;
			}
		}
		if (s2.intersect(rayOrigin, rayDir)) {
			if (s2.t1 < tmin) {
				tIndex = 5;
				shape = &s2;
				tmin = s2.t1;
			}
		}
		if (plane.planeIntersect(rayOrigin, rayDir)) {
			if (plane.t1 < tmin) {
				tIndex = 6;
				shape = &plane;
				tmin = plane.t1;
			}
		}
		if (tIndex == 0) { // tetra face0
			vec3 point = rayOrigin + tetra.t1 * rayDir;
			if (shadow(point))
				return vec3(0, 0, 0) + ambient((Shape*)&tetra);
			return clamp(diffuse((Shape*)&tetra) + ambient((Shape*)&tetra), 0.0f, 255.0f);
		}
		else if (tIndex == 1) { // tetra face1
			vec3 point = rayOrigin + tetra.t2 * rayDir;
			if (shadow(point))
				return vec3(0, 0, 0) + ambient((Shape*)&tetra);
			return clamp(diffuse((Shape*)&tetra, 1) + ambient((Shape*)&tetra), 0.0f, 255.0f);
		}
		else if (tIndex == 2) { // tetra face2
			vec3 point = rayOrigin + tetra.t3 * rayDir;
			if (shadow(point))
				return vec3(0, 0, 0) + ambient((Shape*)&tetra);
			return clamp(diffuse((Shape*)&tetra, 2) + ambient((Shape*)&tetra), 0.0f, 255.0f);
		}
		else if (tIndex == 3) { // tetra face3
			vec3 point = rayOrigin + tetra.t4 * rayDir;
			if (shadow(point))
				return vec3(0, 0, 0) + ambient((Shape*)&tetra);
			return clamp(diffuse((Shape*)&tetra, 3) + ambient((Shape*)&tetra), 0.0f, 255.0f);
		}
		else if (tIndex == 4) { // s1 sphere
			vec3 point = rayOrigin + s1.t1 * rayDir;
			if (shadow(point))
				return vec3(0, 0, 0) + ambient((Shape*)&s1);
			return clamp(diffuse((Shape*)&s1) + ambient((Shape*)&s1) + blingPhong(rayDir, (Shape*)&s1, 20), 0.0f, 255.0f);
		}
		else if (tIndex == 5) { // s2 sphere
			vec3 point = rayOrigin + s2.t1 * rayDir;
			if (shadow(point))
				return vec3(0, 0, 0) + ambient((Shape*)&s2);
			return clamp(diffuse((Shape*)&s2) + ambient((Shape*)&s2), 0.0f, 255.0f);
		}
		else if (tIndex == 6) { // plane
			vec3 point = rayOrigin + plane.t1 * rayDir;
			vec3 r = rayDir - 2 * dot(rayDir, plane.normal) * plane.normal;
			if (plane.isGlaze && depth < MAX_RECURSIVE) {
				vec3 newP = point + r;
				if (shadow(point))
					return clamp(vec3(0, 0, 0) + 0.8f * trace(newP, r, depth + 1), 0.0f, 255.0f);
				else
					return clamp(diffuse((Shape*)&plane) + ambient((Shape*)&plane) + 0.5f * trace(newP, r, depth + 1), 0.0f, 255.0f);
			}
			if (shadow(point))
				return vec3(0, 0, 0);
			return clamp(diffuse((Shape*)&plane) + ambient((Shape*)&plane), 0.0f, 255.0f);
		}
		return vec3(80, 80, 80);
	}
};