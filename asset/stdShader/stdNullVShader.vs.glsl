#version 460 core
//input
layout(location=0)in vec3 aPos;
// layout(location=1)in vec3 aNormal;
// layout(location=2)in vec2 aTexCoord;
uniform mat4 model,view,projection;

//output
// out VS_OUT{
    //     vec3 globalPos;
    //     vec3 globalNormal;
    //     vec2 texCoord;
// }vs_out;

void main(){
    vec4 globalPos4=model*vec4(aPos,1);
    // vs_out.globalPos=globalPos4.xyz;
    // vs_out.globalNormal=normalize(mat3(transpose(inverse(model)))*aNormal);
    // vs_out.texCoord=aTexCoord;
    
    gl_Position=projection*view*globalPos4;
}