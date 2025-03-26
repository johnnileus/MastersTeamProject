#include "Animator.h"

#include <type_traits>
#include "AssetManager.h"

#ifdef USEAGC
template <typename T>
constexpr T lerp(T a, T b, T t) {
    static_assert(std::is_arithmetic<T>::value, "lerp requires numeric types");
    std::cout << "lerp called with a: " << a << ", b: " << b << ", t: " << t << " => result: " << a + t * (b - a) << std::endl;
    return a + t * (b - a);
}
#endif // USEAGC

Animator::Animator()
{
    std::cout << "Animator created: " << this << std::endl;
    currentAnimSpeed = 1.0f;

    tweenBlendFactor = 0.0f;
    tweenTime = 0.0f;
    tweenTimeCurrent = 0.0f;
    isTweening = false;
    currentFrame = 0;
	currentAnim = nullptr;
    pendingAnim = nullptr;
}

Animator::~Animator()
{
    std::cout << "Animator destroyed: " << this << std::endl;
}

void Animator::Update(float dt)
{
    std::cout << "Animator::Update called with dt: " << dt << std::endl;
    if (currentAnim) {
        std::cout << "Current animation: " << currentAnim << std::endl;
    }
    else {
        std::cout << "No current animation" << std::endl;
    }
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
            std::cout << "Current frame before update: " << currentFrame << std::endl;
            if (currentAnim) {
                currentFrame = (currentFrame + 1) % currentAnim->GetFrameCount();
                std::cout << "Current frame after update: " << currentFrame << std::endl;
                nextFrame = (currentFrame + 1) % currentAnim->GetFrameCount();
                frameTime += 1.0f / currentAnim->GetFrameRate();
            }
            else {
				std::cout << "No current animation" << std::endl;
                return;
            }
        }

    }
}
//void Animator::TestLerp() {
//    std::cout << "Testing lerp function:" << std::endl;
//    std::cout << "lerp(0, 10, 0.5) = " << lerp(0.0f, 10.0f, 0.5f) << std::endl; // Expected: 5
//    std::cout << "lerp(0, 10, 0) = " << lerp(0, 10, 0) << std::endl;     // Expected: 0
//    std::cout << "lerp(0, 10, 1) = " << lerp(0, 10, 1) << std::endl;     // Expected: 10
//    std::cout << "Testing lerp edge cases:" << std::endl;
//    std::cout << "lerp(0, 0, 0.5) = " << lerp(0.0f, 0.0f, 0.5f) << std::endl;   // Expected: 0
//    std::cout << "lerp(10, 10, 0.5) = " << lerp(10.0f, 10.0f, 0.5f) << std::endl; // Expected: 10
//    std::cout << "lerp(-10, 10, 0.5) = " << lerp(-10.0f, 10.0f, 0.5f) << std::endl; // Expected: 0
//}

bool Animator::LoadAnimation(const std::string& animationName)
{
    meshAnims[animationName] = AssetManager::Instance().GetAnimation(animationName);
    return (meshAnims[animationName] != nullptr);
}



void Animator::Draw(RenderObject* renderObj)
{
	/*std::cout << "Drawing render object: " << renderObj << std::endl;
    Mesh* mesh = renderObj->GetMesh();
    
    
    Shader* shader = renderObj->GetShader();
    const Matrix4* invBindPose = mesh->GetInverseBindPose().data();

    //if is tweening, interpolate the next and current frame 
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
    //if not, just use current frame data
    else
    {
        const Matrix4* frameData = currentAnim->GetJointData(currentFrame);
    
        for (unsigned int i = 0; i < mesh->GetJointCount(); i++)
            frameMatrices.emplace_back(frameData[i] * invBindPose[i]);
    }

    int j = glGetUniformLocation(GetProcessId(shader), "name");
    glUniformMatrix4fv(j, frameMatrices.size(), false, (float*)frameMatrices.data());
    
    frameMatrices.clear();

    
    for (int i = 0; i < (int)mesh->GetSubMeshCount(); i++)
    {
        
        // if (renderObj->GetShader() != nullptr)
        // {
        //     if (renderObj->GetMaterial() != nullptr && (int)renderObject->GetMaterial()->GetSubMaterials().size() > 0) {
        //         int materialIndexLocations = glGetUniformLocation(r.GetBoundShader()->GetProgramID(), "materialIndex");
        //         int index = renderObject->GetMaterial()->GetMaterialIndex()[i];
        //         glUniform1i(materialIndexLocations, index);
        //     }
        //
        // }
        std::cout<<"submesh count"<<(int)mesh->GetSubMeshCount()<<std::endl;
    }*/
}

void Animator::Play(const std::string& anim, bool tween, float animSpeed)
{
    std::cout << "Playing animation: " << anim << " with tween: " << tween << " and speed: " << animSpeed << std::endl;
    if (anim.empty() || currentAnim == meshAnims.at(anim) || meshAnims.at(anim) == nullptr)
        std::cout << "Animation is empty, already playing, or not found: " << anim << std::endl;
        return;

    if(currentAnim == nullptr)
        currentAnim = meshAnims[anim];
    std::cout << "Set currentAnim to: " << currentAnim << std::endl;

    if (tween)
    {
        pendingAnim = meshAnims[anim];
        std::cout << "Set pendingAnim to: " << pendingAnim << std::endl;
        TweenAnim(0.15f);
    }
    else
    {
        currentFrame = 0;
        currentAnimSpeed = animSpeed;
        currentAnim = meshAnims[anim];
        std::cout << "Set currentAnim to: " << currentAnim << std::endl;
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
#ifdef USEAGC
            res.array[i][j] = lerp(a.array[i][j], b.array[i][j], t);
#else
            res.array[i][j] = std::lerp(a.array[i][j], b.array[i][j], t);
#endif // USEAGC
            
        }
    }
    return res;
}


