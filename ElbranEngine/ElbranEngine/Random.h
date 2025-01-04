#pragma once

class Random
{
	friend class Application;

public:
	int Generate(int min, int max) const;
	float Generate(float min, float max) const;

	// prevent copying
	Random(const Random&) = delete;
	void operator=(const Random&) = delete;

private:
	Random();
	~Random();
};

