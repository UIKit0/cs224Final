#ifndef MYCONTACTMODIFICATION_H
#define MYCONTACTMODIFICATION_H

#include <iostream>
#include "PxPhysicsAPI.h"

using namespace physx;
class MyContactModification : public PxContactModifyCallback
{
    void onContactModify(PxContactModifyPair* const pairs, PxU32 count);
};

#endif
