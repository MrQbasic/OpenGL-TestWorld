#version 430 core
in vec3 FragPos;

out vec4 FragColor;

uniform vec3 groundColor;
uniform vec3 cameraPos;

void main()
{
    // Compute ray direction from camera
    vec3 rayDir = normalize(FragPos - cameraPos);

    // Ground plane normal and position
    vec3 planeNormal = vec3(0.0, 1.0, 0.0);
    float planeHeight = 0.0;

    // Ray-plane intersection
    float t = (planeHeight - cameraPos.y) / rayDir.y;
    vec3 intersection = cameraPos + t * rayDir;

    // Infinite plane visualization (simple grid effect)
    float gridSize = 1.0;
    float checker = mod(floor(intersection.x / gridSize) + floor(intersection.z / gridSize), 2.0);
    vec3 color = mix(groundColor, groundColor * 0.5, checker);



    FragColor = vec4(color, 1.0);
}