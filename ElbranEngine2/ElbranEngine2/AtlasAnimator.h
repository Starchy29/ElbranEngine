#pragma once
#include "IBehavior.h"

class AtlasAnimator :
    public IBehavior
{
public:
    AtlasAnimator();

    void Update(float deltaTime) override;

private:

};

