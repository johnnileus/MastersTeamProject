#include "Animator.h"

#include "AssetManager.h"

Animator::Animator(RenderObject* renderObject)
{
    currentAnimSpeed = 1.0f;

    tweenBlendFactor = 0.0f;
    tweenTime = 0.0f;
    tweenTimeCurrent = 0.0f;
    isTweening = false;
    currentFrame=0;
    pendingAnim = nullptr;

    this->renderObject = renderObject;
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
    
    Draw(currentFrame,currentAnim);
}

bool Animator::LoadAnimation(const std::string& animationName)
{
    meshAnims[animationName] = AssetManager::Instance().GetAnimation(animationName);
    return (meshAnims[animationName]!=nullptr);
}

/// The actual skinning rendering method
/// @param nFrame target frame
/// @param meshAni target animation
void Animator::Draw(int nFrame,MeshAnimation* meshAni)
{
    //get mesh and joints count
    OGLMesh* animMesh = (OGLMesh*)renderObject->GetMesh();
    size_t jointCount = animMesh->GetJointCount();

    //target frame data
    const Matrix4* frameData = currentAnim->GetJointData(nFrame);
    
    // Mesh can use GetInverseBindPose() get each joint's inverse bind pose matrix
    const auto& invBindPose = animMesh->GetInverseBindPose();
    
    std::vector<Matrix4> finalMatrices(jointCount);
    for (size_t i = 0; i < jointCount; ++i) {
        finalMatrices[i] = frameData[i] * invBindPose[i];
    }

    //get shader and activate gl program
    OGLShader* shader = (OGLShader*)renderObject->GetShader();
    GLuint programID = shader->GetProgramID();
    glUseProgram(programID);
    
    //upload these matrices data to GPU
    GLint jointsLoc = glGetUniformLocation(programID, "joints");
    if (jointsLoc==-1)
    {
        std::cout<<"!!!!!!!ERROR: Uniform 'joints' not found in shader !!!!!!!!!!";
    }
    else
    {
        glUniformMatrix4fv(jointsLoc, (GLsizei)jointCount, GL_FALSE, reinterpret_cast<const float*>(finalMatrices.data()));
    }

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







