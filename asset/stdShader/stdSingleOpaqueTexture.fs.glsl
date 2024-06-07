#version 460 core
//定义灯光组的最大灯光数量
#define MAX_LIGHTS_NUM 16
//input
in VS_OUT{
    vec3 globalPos;
    vec3 globalNormal;
    vec2 texCoord;
}fs_in;
uniform sampler2D texture0;

struct Light{
    int lightType;
    vec3 color;
    float intensity;
    vec3 position,rotation;
    float innerCutOff,outerCutOff;// for spot light
};

layout(std140,binding=0)uniform lightGroup{
    int numLights;
    Light lights[MAX_LIGHTS_NUM];
};

//output
out vec4 fragColor;

void main(){
    fragColor=vec4(texture(texture0,fs_in.texCoord).xyz,1);
}
