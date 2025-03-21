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

Animator::Animator(RenderObject* renderObject)
{
    std::cout << "Animator created: " << this << std::endl;
    currentAnimSpeed = 1.0f;

    tweenBlendFactor = 0.0f;
    tweenTime = 0.0f;
    tweenTimeCurrent = 0.0f;
    isTweening = false;
    currentFrame = 0;
	//currentAnim = nullptr;
    pendingAnim = nullptr;
    this->renderObject = renderObject;
    //renderObject->BindAnimator(this);
}

Animator::~Animator()
{
    std::cout << "Animator destroyed: " << this << std::endl;
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
            frameTime += 1.0f / currentAnim->GetFrameRate();

            if (loopAnimation)
            {
                currentFrame = (currentFrame + 1) % currentAnim->GetFrameCount();
            }
            else
            {
                if (currentFrame < (int)currentAnim->GetFrameCount() - 1)
                {
                    currentFrame++;
                }
                else
                {
                    currentFrame = currentAnim->GetFrameCount() - 1;
                }
            }
            nextFrame = (currentFrame + 1) % currentAnim->GetFrameCount();
        }
    }

    renderObject->currentFame = currentFrame;
    renderObject->currentAnimation = currentAnim;

    Draw(currentFrame, currentAnim);
}


bool Animator::LoadAnimation(AnimationType animation)
{
    meshAnims[animation] = AssetManager::Instance().GetAnimation(animation);
    return (meshAnims[animation] != nullptr);
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


/// The actual skinning rendering method
/// @param nFrame target frame
/// @param meshAni target animation
void Animator::Draw(int nFrame, MeshAnimation* meshAni)
{
    //get mesh and joints count
    OGLMesh* animMesh = (OGLMesh*)renderObject->GetMesh();
    size_t jointCount = animMesh->GetJointCount();

    //target frame data
    const Matrix4* frameData = currentAnim->GetJointData(nFrame);

    // Mesh can use GetInverseBindPose() get each joint's inverse bind pose matrix
    const auto& invBindPose = animMesh->GetInverseBindPose();

    std::vector<Matrix4> finalMatrices(jointCount);

    if (isTweening && pendingAnim != nullptr) {
        const Matrix4* pendingFrameData = pendingAnim->GetJointData(0);

        for (size_t i = 0; i < jointCount; ++i) {
            Matrix4 oldMat = frameData[i];
            Matrix4 newMat = pendingFrameData[i];
            // lerp old and new matrices then multiply inverse bind pose get the interpolated matrix
            Matrix4 blended = LerpMat(oldMat, newMat, tweenBlendFactor);
            finalMatrices[i] = blended * invBindPose[i];
        }
    }
    else {
        for (size_t i = 0; i < jointCount; ++i) {
            finalMatrices[i] = frameData[i] * invBindPose[i];
        }
    }

    //get shader and activate gl program
    OGLShader* shader = (OGLShader*)renderObject->GetShader();
    GLuint programID = shader->GetProgramID();
    glUseProgram(programID);

    //upload these matrices data to GPU
    GLint jointsLoc = glGetUniformLocation(programID, "joints");
    if (jointsLoc == -1)
    {
        std::cout << "!!!!!!!ERROR: Uniform 'joints' not found in shader !!!!!!!!!!";
    }
    else
    {
        glUniformMatrix4fv(jointsLoc, (GLsizei)jointCount, GL_FALSE,
            reinterpret_cast<const float*>(finalMatrices.data()));
    }
}

void Animator::Play(AnimationType anim, bool tween, float animSpeed, bool loop)
{
    if (currentAnim == meshAnims.at(anim) || meshAnims.at(anim) == nullptr)
        return;

    loopAnimation = loop;

    if (currentAnim == nullptr)
        currentAnim = meshAnims[anim];

    if (tween)
    {
        pendingAnim = meshAnims[anim];
        TweenAnim(0.1f);
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
#ifdef USEAGC
            res.array[i][j] = lerp(a.array[i][j], b.array[i][j], t);
#else
            res.array[i][j] = std::lerp(a.array[i][j], b.array[i][j], t);
#endif // USEAGC       
        }
    }

    return res;
}


