#version 460 core
//定义灯光组的最大灯光数量
#define MAX_LIGHTS_NUM 16

//input
in VS_OUT{
    vec3 globalPos;
    vec3 globalNormal;
    vec2 texCoord;
}fs_in;

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

uniform vec3 cameraPos;
uniform sampler2D texture0;
uniform samplerCube skybox;

//output
out vec4 fragColor;

vec3 Lighting(int i);

void main(){
    vec3 outputColor=vec3(0.f);
    for(int i=0;i<MAX_LIGHTS_NUM;i++){
        outputColor+=Lighting(i);
    }
    vec3 ambient=texture(skybox,reflect(fs_in.globalPos-cameraPos,fs_in.globalNormal)).xyz;
    fragColor=vec4(outputColor,1.f);
}

vec3 Lighting(int i){
    if(lights[i].lightType==-1){
        return vec3(0);
    }
    
    vec3 dispToLight=lights[i].position-fs_in.globalPos;
    vec3 dirToLight=normalize(dispToLight);
    if(lights[i].lightType==1){
        //日光
        dirToLight=-normalize(lights[i].rotation);
    }
    vec3 viewDir=normalize(cameraPos-fs_in.globalPos);
    
    //光源衰减
    float lightDistDropoff=1;
    if(lights[i].lightType!=1){
        //日光，不计算距离
        float lightDist=distance(dispToLight,vec3(0));
        // float lightDistDropoff=1/dot(vec3(1,lightDist,pow(lightDist,2)),vec3(1,.09,.032));
        float lightDistDropoff=1/pow(lightDist,2);
    }
    // 聚光灯裁切
    float spotLightCutOff=1;
    if(lights[i].lightType==2){
        spotLightCutOff=dot(-dirToLight,lights[i].rotation);
        float cutOffRange=lights[i].innerCutOff-lights[i].outerCutOff;
        spotLightCutOff=clamp((spotLightCutOff-lights[i].outerCutOff)/cutOffRange,0.f,1.f);
    }
    
    //diffusion
    float diffuseFac=max(dot(dirToLight,fs_in.globalNormal),0.f);
    vec3 diffuseColor=lights[i].color*texture(texture0,fs_in.texCoord).rgb;
    vec3 diffuse=diffuseColor*diffuseFac;
    
    //specular
    vec3 halfVec=normalize(dirToLight+viewDir);
    float specularFac=pow(max(dot(halfVec,fs_in.globalNormal),0.f),64);
    vec3 specularColor=texture(texture0,fs_in.texCoord).rgb*lights[i].color;
    vec3 specular=specularColor*specularFac;
    
    //ambient
    // vec3 ambient=texture(skybox,reflect(-viewDir,fs_in.globalNormal)).xyz;
    
    //combine
    return(diffuse+specular)*lights[i].intensity*lightDistDropoff*spotLightCutOff;
}
