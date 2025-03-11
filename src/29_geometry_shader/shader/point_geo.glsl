#version 330 core
layout (points) in;
layout (triangle_strip, max_vertices = 5) out;

out vec3 outPosition;

void build_house(vec4 position)
{   
    gl_Position = position + vec4(-0.2, -0.2, 0.0, 0.0);    // 1:左下  
    outPosition = gl_Position.xyz;
    EmitVertex();   

    gl_Position = position + vec4( 0.2, -0.2, 0.0, 0.0);    // 2:右下
    outPosition = gl_Position.xyz;
    EmitVertex();

    gl_Position = position + vec4(-0.2,  0.2, 0.0, 0.0);    // 3:左上
    outPosition = gl_Position.xyz;
    EmitVertex();

    gl_Position = position + vec4( 0.2,  0.2, 0.0, 0.0);    // 4:右上
    outPosition = gl_Position.xyz;
    EmitVertex();

    gl_Position = position + vec4( 0.0,  0.4, 0.0, 0.0);    // 5:顶部
    outPosition = gl_Position.xyz;
    EmitVertex();
    
    EndPrimitive();  
}

void main() 
{    
    build_house(gl_in[0].gl_Position);
}