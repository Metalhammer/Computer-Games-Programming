#version 330

uniform sampler2D tex;

in vec2 texCoord;
in vec3 vertPos;
in vec3 N;
in vec3 lightPos;

out vec4 FragColour;

uniform vec3 Kd;            // Diffuse reflectivity
uniform vec3 Ld;            // Diffuse light intensity
uniform vec3 La;			//ambient light intensity
uniform vec3 Ka;			//ambient reflection coefficient
uniform vec3 Ks;			//specular reflection coefficient
uniform int n;				//specular reflection exponent

void main() {

//Calculate the light vector
   vec3 L = normalize(lightPos - vertPos);  
    
	//Calculate ambient light intensity
	vec4 Ia = vec4(La,1.0) * vec4(Ka,1.0);

   //calculate Diffuse Light Intensity
   vec4 Id = vec4(Ld,1.0)* max(dot(N,L), 0.0);
   Id = clamp(Id, 0.0, 1.0);

   //calculate specular
   float nl = clamp(dot(N,L),0.0,1.0);
   vec3 R = (2 * N * nl) -L;
   vec3 V = normalize(-vertPos);
   float RV = dot(R,V);
   RV = clamp(RV,0.0,1.0);

   vec4 Is = vec4(Ld,1.0) * vec4(Ks,1.0) * pow(RV,n);   
   FragColour = (Id*vec4(Kd,1.0)  + Ia + Is) * texture(tex,texCoord);
}