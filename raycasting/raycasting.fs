#version 330 core

in vec3 EntryPoint;
in vec4 ExitPointCoord;

uniform sampler2D ExitPoints;
uniform sampler3D VolumeTex;
//uniform sampler1D TransferFunc;
uniform float     StepSize;
uniform vec2      ScreenSize;
layout (location = 0) out vec4 FragColor;

uniform sampler2D TransferFunc;

void main()
{
    vec3 exitPoint = texture(ExitPoints, gl_FragCoord.st/ScreenSize).xyz;

    if (EntryPoint == exitPoint)
        //background need no raycasting
        discard;

    vec3 dir = exitPoint - EntryPoint;
    // the length from front to back is calculated and used to terminate the ray
    float len = length(dir);
    // delta length for sampling
    vec3 deltaDir = normalize(dir) * StepSize;
    float deltaDirLen = length(deltaDir);
    vec3 voxelCoord = EntryPoint;
    // The destination color
    vec4 colorAcum = vec4(0.0);
//    float intensity;
    
    vec2 intensity;

    float lengthAcum = 0.0;
    // The srouce color
    vec4 colorSample;

    // backgroundColor
    vec4 bgColor = vec4(1.0, 1.0, 1.0, 0.0);

    for(int i = 0; i < 1600; i++)
    {
//        intensity =  texture(VolumeTex, voxelCoord).x;
        intensity =  texture(VolumeTex, voxelCoord).xy;
        colorSample = texture(TransferFunc, intensity);
//        colorSample = vec4(texture(TransferFunc, intensity).rgb, intensity*0.3);
//        colorSample = vec4(texture(TransferFunc, vec2(intensity, 0.1)).rgb, intensity*0.1);

        // accomodate for variable sampling rates
        if (colorSample.a > 0.0) {
            colorSample.a = 1.0 - pow(1.0 - colorSample.a, StepSize*100.0f);
            colorAcum.rgb += (1.0 - colorAcum.a) * colorSample.rgb * colorSample.a;
            colorAcum.a += (1.0 - colorAcum.a) * colorSample.a;
        }

        voxelCoord += deltaDir;
        lengthAcum += deltaDirLen;

        // terminate if the ray is outside the volume
        if (lengthAcum >= len )
        {
            colorAcum.rgb = colorAcum.rgb*colorAcum.a + (1 - colorAcum.a)*bgColor.rgb;
            break;
        }
        // terminate if opacity > 1
        else if (colorAcum.a > 1.0)
        {
            colorAcum.a = 1.0;
            break;
        }
    }

    FragColor = colorAcum;
//     for test
//     FragColor = vec4(EntryPoint, 1.0);
//    FragColor = vec4(exitPoint, 1.0);
//    FragColor = texture(ExitPoints, ExitPointCoord.xy);
    
//    FragColor = texture(TransferFunc, EntryPoint.x);
}

