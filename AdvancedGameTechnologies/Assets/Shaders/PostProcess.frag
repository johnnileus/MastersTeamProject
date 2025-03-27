#version 330 core

in  vec2 uv;
out vec4 fragColor;

// 场景颜色与深度
uniform sampler2D sceneTex;
uniform sampler2D depthTex;

// 与雾相关
uniform float cameraNear;
uniform float cameraFar;
uniform vec3  fogColor      = vec3(0.5, 0.5, 0.5);
uniform float fogDensity    = 0.02;

// 与Bloom相关
uniform float bloomThreshold = 1.0;  // 亮度阈值
uniform float bloomIntensity = 0.5;  // 最终叠加的强度

// 计算线性深度
float LinearizeDepth(float ndcDepth) {
    // NDC: 0~1 -> clipSpace -1~1
    float z = ndcDepth * 2.0 - 1.0;
    // 透视投影的线性深度
    return (2.0 * cameraNear * cameraFar) / (cameraFar + cameraNear - z * (cameraFar - cameraNear));
}

void main() {
    // 1) 先从场景纹理中取颜色
    vec4 sceneColor = texture(sceneTex, uv);

    // 2) 根据深度计算雾因子
    float ndcDepth  = texture(depthTex, uv).r;
    float linearDist = LinearizeDepth(ndcDepth);
    float fogFactor  = 1.0 - exp(-fogDensity * linearDist);

    // 3) 混合出雾后的基本颜色
    vec3 baseColor = mix(sceneColor.rgb, fogColor, fogFactor);

    // 4) 做一个"阈值提取"：只取高亮区域
    //    若像素亮度<某个阈值，则该区域不参与Bloom
    vec3 bright = max(baseColor - vec3(bloomThreshold), vec3(0.0));
    // 若三通道全都 <= 0，说明不够亮，不需要Bloom
    if (dot(bright, vec3(1.0)) <= 0.0) {
        // 不够亮，直接输出baseColor
        fragColor = vec4(baseColor, sceneColor.a);
        return;
    }

    // 5) 做一个简易的3x3模糊，把周围的高亮像素累加
    //    注意：这是非常简化的做法，正式Bloom会拆成多Pass
    float kernel[9] = float[](1,2,1,2,4,2,1,2,1);
    float kernelSum  = 16.0; // 1+2+1+2+4+2+1+2+1 =16

    // 计算采样步长(像素大小)
    vec2 texSize   = textureSize(sceneTex, 0);
    vec2 texelSize = 1.0 / texSize;

    int index  = 0;
    vec3 bloomSum = vec3(0.0);
    for (int y = -1; y <= 1; y++) {
        for (int x = -1; x <= 1; x++) {
            // 取周围像素的亮区
            vec3 c = texture(sceneTex, uv + vec2(x, y) * texelSize).rgb;
            // 再做与阈值对比
            c = max(c - vec3(bloomThreshold), vec3(0.0));
            bloomSum += c * kernel[index++];
        }
    }
    vec3 bloom = bloomSum / kernelSum;

    // 6) 将Bloom结果与baseColor叠加
    vec3 finalColor = baseColor + bloom * bloomIntensity;

    // 输出
    fragColor = vec4(finalColor, sceneColor.a);
}
