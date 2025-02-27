#include "Animator.h"

#include "AssetManager.h"

Animator::Animator()
{
    currentAnimSpeed = 1.0f;

    tweenBlendFactor = 0.0f;
    tweenTime = 0.0f;
    tweenTimeCurrent = 0.0f;
    isTweening = false;
    currentFrame=0;
    pendingAnim = nullptr;
}

Animator::~Animator()
{
    
}

void Animator::Update(float dt)
{
    if (!currentAnim) return;

    if (isTweening && pendingAnim != nullptr)
    {
        tweenBlendFactor += dt * (1.0f / tweenTime);
        tweenTimeCurrent -= dt;
        if (tweenTimeCurrent <= 0.0f)
        {
            tweenTimeCurrent = 0.0f;
            isTweening = false;
            currentFrame = 0;
            currentAnim = pendingAnim;
        }
    }
    else
    {
        frameTime -= dt * currentAnimSpeed;
        while (frameTime < 0.0f) 
        {
            currentFrame = (currentFrame + 1) % currentAnim->GetFrameCount();
            nextFrame = (currentFrame + 1) % currentAnim->GetFrameCount();
            frameTime += 1.0f / currentAnim->GetFrameRate();
        }
    }
}

bool Animator::LoadAnimation(const std::string& animationName)
{
    meshAnims[animationName] = AssetManager::Instance().GetAnimation(animationName);
    return (meshAnims[animationName]!=nullptr);
}

void Animator::Draw(RenderObject* renderObj)
{
    Mesh* mesh = renderObj->GetMesh();
    
    
    Shader* shader = renderObj->GetShader();
    const Matrix4* invBindPose = mesh->GetInverseBindPose().data();
    
    if (isTweening && (pendingAnim != nullptr))
    {
        const Matrix4* animCurrentFrame = currentAnim->GetJointData(currentFrame);
        
        const Matrix4* animPendingFrame = pendingAnim->GetJointData(0);
    
        std::vector<Matrix4> finalBlending;
        for (size_t i = 0; i < mesh->GetJointCount(); i++)
            finalBlending.emplace_back( LerpMat(animCurrentFrame[i], animPendingFrame[i], tweenBlendFactor));
    
        for (size_t i = 0; i < mesh->GetJointCount(); i++)
            frameMatrices.emplace_back(finalBlending[i] * invBindPose[i]);
    }
    else
    {
        const Matrix4* frameData = currentAnim->GetJointData(currentFrame);
    
        for (unsigned int i = 0; i < mesh->GetJointCount(); i++)
            frameMatrices.emplace_back(frameData[i] * invBindPose[i]);
    }

    int j = glGetUniformLocation(GetProcessId(shader), "joints");
    glUniformMatrix4fv(j, frameMatrices.size(), false, (float*)frameMatrices.data());
    
    frameMatrices.clear();
    
}

void Animator::Play(const std::string& anim, bool tween, float animSpeed)
{
    if (anim.empty() || currentAnim == meshAnims.at(anim) || meshAnims.at(anim) == nullptr)
        return;

    if(currentAnim == nullptr)
        currentAnim = meshAnims[anim];

    if (tween)
    {
        pendingAnim = meshAnims[anim];
        TweenAnim(0.15f);
    }
    else
    {
        currentFrame = 0;
        currentAnimSpeed = animSpeed;
        currentAnim = meshAnims[anim];
    }
}

void Animator::TweenAnim(const float& time)
{
    if (isTweening) return;

    isTweening = true;
    tweenTime = time;
    tweenTimeCurrent = time;
    tweenBlendFactor = 0.0f;
}


Matrix4 Animator::LerpMat(const Matrix4& a, const Matrix4& b, float t)
{
    Matrix4 res;

    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            res.array[i][j] = std::lerp(a.array[i][j], b.array[i][j], t);
        }
    }
    return res;
}







