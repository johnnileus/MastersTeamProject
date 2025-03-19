#pragma once
#include "MeshAnimation.h"
#include "GameTechRenderer.h"
#include "MeshMaterial.h"
#include "RenderObject.h"
//#include  "bthdef.h"
namespace NCL
{
    namespace CSC8503
    {
        class Animator
        {
        public:
            Animator();
            ~Animator();

            void Update(float dt);

            bool LoadAnimation(const std::string& animationName);

            void Draw(RenderObject* renderObj);

            void Play(const std::string& anim, bool tween, float animSpeed);

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
            std::vector<Matrix4> frameMatrices;
            
            
            std::map<std::string, MeshAnimation*> meshAnims;
        };
    }
}
