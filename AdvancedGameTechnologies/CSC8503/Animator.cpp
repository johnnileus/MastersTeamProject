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
    Draw(currentFrame);
}

bool Animator::LoadAnimation(const std::string& animationName)
{
    meshAnims[animationName] = AssetManager::Instance().GetAnimation(animationName);
    return (meshAnims[animationName]!=nullptr);
}

void Animator::Draw(int nFrame)
{
#pragma region Old function
    // //get mesh and shader data
    // Mesh* mesh = renderObj->GetMesh();
    // Shader* shader = renderObj->GetShader();
    //
    // if (!currentAnim)
    // {
    //     return;
    // }
    //
    // //get skeleton data
    // const Matrix4* invBindPose = mesh->GetInverseBindPose().data();
    // size_t jointCount = mesh->GetJointCount();
    //
    // if (jointCount == 0) {
    //     std::cout<<"!!!!!Warning: mesh"<<mesh->GetAssetID()<<" doesn't have skeleton data!!!!!"<<std::endl;
    //     return; 
    // }
    //
    // //clear last frame's buffer first
    // frameMatrices.clear();
    // frameMatrices.reserve(jointCount);
    //
    // //if is tweening, interpolate the next and current frame 
    // if (isTweening && (pendingAnim != nullptr)) {
    //     //get current animation data
    //     const Matrix4* currentAnimFrameData = currentAnim->GetJointData(currentFrame);
    //     //get new animation's first frame data for interpolation
    //     const Matrix4* pendingAnimFrameData = pendingAnim->GetJointData(0);
    //
    //     //do interpolation
    //     for (size_t i = 0; i < jointCount; i++) {
    //         Matrix4 blended = LerpMat(currentAnimFrameData[i], pendingAnimFrameData[i], tweenBlendFactor);
    //         frameMatrices.emplace_back(blended * invBindPose[i]);
    //     }
    // }
    // //if not, just use current frame data
    // else {
    //     const Matrix4* frameData = currentAnim->GetJointData(currentFrame);
    //     //multiply invBindPose get skin matrix
    //     for (size_t i = 0; i < jointCount; i++) {
    //         frameMatrices.emplace_back(frameData[i] * invBindPose[i]);
    //     }
    // }
    //
    // GLint programID = 13;
    // GLint loc = glGetUniformLocation(programID, "joints"); 
    // glUniformMatrix4fv(loc, (GLsizei)frameMatrices.size(), GL_FALSE, (float*)frameMatrices.data());
    //
    //
    // for (int i = 0; i < (int)mesh->GetSubMeshCount(); i++)
    // {
    //     //TODO
    //     //Draw animation and submesh texture
    // }
#pragma endregion

    //get mesh and joints count
    OGLMesh* animMesh = (OGLMesh*)renderObject->GetMesh();
    size_t jointCount = animMesh->GetJointCount();

    //target frame data
    const Matrix4* frameData = meshAnims["Role_Walk"]->GetJointData(nFrame);
    
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
        std::cout<<"!!!!!!!ERROE:Uniform 'joints' not found in shader !!!!!!!!!!";
    }
    else
    {
        glUniformMatrix4fv(jointsLoc, (GLsizei)jointCount, GL_FALSE, reinterpret_cast<const float*>(finalMatrices.data()));
    }

    glBindVertexArray(animMesh->GetVAO());
    
    glDrawElements(GL_TRIANGLES, animMesh->GetIndexCount(), GL_UNSIGNED_INT, nullptr);

    glBindVertexArray(0);

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







