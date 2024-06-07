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
uniform sampler2D shadowMap;

uniform vec3 cameraPos;
uniform sampler2D texture0;
uniform samplerCube skybox;
uniform mat4 lightSpaceMatrix;

//output
out vec4 fragColor;

vec3 Lighting(int i);
float calculateShadow();

void main(){
    vec3 outputColor=vec3(0.f);
    for(int i=0;i<MAX_LIGHTS_NUM;i++){
        outputColor+=Lighting(i);
    }
    vec3 ambient=texture(skybox,reflect(fs_in.globalPos-cameraPos,fs_in.globalNormal)).xyz;
    fragColor=vec4(outputColor*calculateShadow(),1.f);
    
    // fragColor=vec4(vec3(calculateShadow()),1);
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

float calculateShadow(){
    vec4 fragLightSpacePos4=lightSpaceMatrix*vec4(fs_in.globalPos,1);
    vec3 fragLightSpacePos=(fragLightSpacePos4.xyz/fragLightSpacePos4.w+1)/2;//透视除法，转换到[0,1]
    /**FIXME - 错题本
    注意fragLightSpacePos.xy得范围是[-1, 1]，要转换成[0, 1]才能当作UV来采样*/
    if(fragLightSpacePos.z<0||fragLightSpacePos.z>1){
        return.5;
    }
    
    // float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
    float shadow=0,bias=.0001,s_col=.1,l_col=.5;
    vec2 texelSize=1./textureSize(shadowMap,0);
    /**FIXME - 错题本
    textureSize()返回整数int，所以1/textureSize(shadowMap,0)算整数除法=0*/
    for(int i=0;i<3;i++){
        for(int j=0;j<3;j++){
            float cloestDepth=texture(shadowMap,fragLightSpacePos.xy+vec2(i,j)*texelSize).r;
            shadow+=(fragLightSpacePos.z-bias>cloestDepth)?s_col:l_col;
        }
    }
    
    return shadow/9;
}
