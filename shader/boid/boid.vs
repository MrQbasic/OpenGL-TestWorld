#version 430 core

layout(location = 0) in vec3 aPos;

layout(std430, binding = 0) buffer InstanceData {
    float boidData[];
};

uniform mat4 u_projection;
uniform mat4 u_view;
uniform vec3 u_cagePos;
uniform vec3 u_cageSize;

out int id;

out vec3 tex;

void main() {
    id = gl_InstanceID;
    vec3 position = vec3(boidData[id*6],boidData[id*6+1],boidData[id*6+2]);
    vec3 vel = vec3(boidData[id*6+3],boidData[id*6+4],boidData[id*6+5]);

    if(any(lessThan(position, vec3(0))) || any(greaterThan(position, u_cageSize))){
        tex = vec3(1,0,0);
    }else if(any(lessThan(position, vec3(1.0f))) || any(greaterThan(position, u_cageSize-vec3(1.0f)))){
        tex = vec3(1,1,0);
    }else{
        tex = vec3(0,1,1);
    }


    //calc rotation from position data
    vec3 velNormalized = normalize(vel);

    vec3 rotation = vec3(0);
    rotation.x = asin(velNormalized.y);
    rotation.y = acos(velNormalized.x) * sign(velNormalized.z) - 3.1415926535 * 0.5;


    float cx = cos(rotation.x), sx = sin(rotation.x);
    float cy = cos(rotation.y), sy = sin(rotation.y);
    float cz = cos(rotation.z), sz = sin(rotation.z);

    mat3 rotMat = mat3(
        cy * cz,                      cy * sz,                      -sy,
        sx * sy * cz - cx * sz,       sx * sy * sz + cx * cz,       sx * cy,
        cx * sy * cz + sx * sz,       cx * sy * sz - sx * cz,       cx * cy
    );


    vec3 vertexPos = (aPos * rotMat) + position + u_cagePos;

    gl_Position = u_projection * u_view * vec4(vertexPos, 1.0);
}



