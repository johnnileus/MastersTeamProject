#include "CatCoin.h"
using namespace NCL;
using namespace CSC8503;

CatCoin::CatCoin()
{
    tag = "Coin";
}

void CatCoin::Init()
{
    renderObject->SetColour(Vector4(1,0.988,0.396,1) );
}

CatCoin::~CatCoin()
{
    
}


