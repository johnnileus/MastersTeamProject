#pragma once
#include "MeshAnimation.h"
#include "GameTechRenderer.h"
#include "MeshMaterial.h"
#include "RenderObject.h"
//#include  "bthdef.h"
#include "Enums.h"

using namespace Enums;
namespace NCL
{
    namespace CSC8503
    {
        class Animator
        {
        public:
            Animator();
            Animator(RenderObject* renderObject);
            ~Animator();

            void Update(float dt);

            bool LoadAnimation(AnimationType);

            void Draw(int nFrame, MeshAnimation* meshAni);

            void Play(AnimationType animation, bool tween, float animSpeed = 1, bool loop = true);

            void TweenAnim(const float& time);

            void TestLerp();

            static Matrix4 LerpMat(const Matrix4& a, const Matrix4& b, float t);

        protected:
            MeshMaterial* meshMaterial;

            int currentFrame;
            int nextFrame;
            float frameTime;

            bool isTweening;
            float tweenTime;
            float tweenTimeCurrent;
            float tweenBlendFactor;

            float	currentAnimSpeed;
            MeshAnimation* currentAnim;
            MeshAnimation* pendingAnim;
            RenderObject* renderObject;
            std::vector<Matrix4> frameMatrices;
            
            bool loopAnimation = true;

            std::map<AnimationType, MeshAnimation*> meshAnims;
        };
    }
}
