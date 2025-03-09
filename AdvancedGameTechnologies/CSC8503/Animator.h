#pragma once
#include "MeshAnimation.h"
#include "GameTechRenderer.h"
#include "MeshMaterial.h"
#include "RenderObject.h"
#include  "bthdef.h"
#include "Enums.h"

using namespace Enums;
namespace NCL
{
    namespace CSC8503
    {
        class Animator
        {
        public:
            Animator(RenderObject* renderObject);
            ~Animator();

            void Update(float dt);

            bool LoadAnimation(PlayerAnimation);

            void Draw(int nFrame,MeshAnimation* meshAni);

            void Play(PlayerAnimation animation, bool tween, float animSpeed);

            void TweenAnim(const float& time);

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
            
            std::map<PlayerAnimation, MeshAnimation*> meshAnims;
        };
    }
}
