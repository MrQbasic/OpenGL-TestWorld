#version 430 core

in vec2 posSurf;

out vec4 color;

void main(){

    vec2 complexCord = (posSurf * vec2(4)) - vec2(2);

    int n=0;
    int iterMax = 100;
    vec2 iterCord = complexCord;

    while(n < iterMax){
        //calc new cord
        vec2 newComplexCord = vec2(pow(iterCord.x,2)-pow(iterCord.y,2), 2 * iterCord.x * iterCord.y);
        iterCord = newComplexCord + complexCord;
        //check if out of bound

        if((iterCord.x + iterCord.y) > 20) break;

        n++;
    }

    if(n == iterMax){
        color = vec4(0, 0, 0, 1);
    }else{

        float brightness = 1 - (n / iterMax);

        color = vec4(vec3(brightness), 1);
    }
}