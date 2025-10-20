#version 430 core

in vec3 pos;
in vec3 toSunVec;
in vec3 normal;

out vec4 color;

void main(){

    vec3 normalNormalized = normalize(normal);
    vec3 toSunVecNormal = normalize(toSunVec);
    float dotProduct = dot(normalNormalized, toSunVecNormal);
    float brightness = max(dotProduct, 0.2f);

    vec3 surfCol = vec3(1.0f, 0.0f, 0.0f);

    //outline
    float threshold = 0.004;
    bvec3 lowerCorner = lessThan(pos, vec3(threshold));
    bvec3 upperCorner = greaterThan(pos, vec3(1-threshold));

    int lowerCornerCnt = int(lowerCorner.x) + int(lowerCorner.y) + int(lowerCorner.z);

    //check w
    if(lowerCornerCnt > 1 ||
       (upperCorner.x && (lowerCorner.y || upperCorner.y)) ||
       (upperCorner.x && (lowerCorner.z || upperCorner.z)) ||
       (upperCorner.y && (lowerCorner.x || upperCorner.x)) ||
       (upperCorner.y && (lowerCorner.z || upperCorner.z)) ||
       (upperCorner.z && (lowerCorner.y || upperCorner.y)) ||
       (upperCorner.z && (lowerCorner.x || upperCorner.x))
    ){
        color = vec4(1, 0, 0, 1);
    }else{
        color = vec4(surfCol*0.3f, 0.2f);
    }

    //color = vec4(pos, 1);
}