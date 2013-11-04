// ADS_Texture.frag
#version 400

in vec3 position;
in vec3 normal;
in vec2 textureCoord;

layout (location = 0) out vec4 fragColor;

uniform vec3 ambientIntensity; // Environmental ambient light intensity for each RGB component.
uniform sampler2D texture2d;

struct LightProperties {
    vec3 position;      // Light position in eye coordinate space.
    vec3 rgbIntensity;  // Light intensity for each RGB component.
};
uniform LightProperties lightSource;

struct MaterialProperties {
    vec3 emission;  // Emission light intensity from material for each RGB component.
    vec3 Ka;        // Coefficients of ambient reflectivity for each RGB component.
    vec3 Kd;        // Coefficients of diffuse reflectivity for each RGB component.
    float Ks;       // Coefficient of specular reflectivity, uniform across each RGB component.
    float shininessFactor;   // Specular shininess factor.
};
uniform MaterialProperties material;

void phongModel(in vec3 fragPosition,
                in vec3 fragNormal,
                out vec3 ambientDiffuse,
		        out vec3 specular) {

    vec3 l = normalize(lightSource.position - fragPosition); // Direction from fragment to light source.
    vec3 v = normalize(-fragPosition.xyz); // Direction from fragment to viewer (origin - fragPosition).
    vec3 h = normalize(v + l); // Halfway vector.

    vec3 ambient = ambientIntensity * material.Ka;

    float n_dot_l = max(dot(fragNormal, l), 0.0);
    vec3 diffuse = material.Kd * n_dot_l;
    
    ambientDiffuse = lightSource.rgbIntensity * (ambient + diffuse); 

    specular = vec3(0.0);
    if (n_dot_l > 0.0) {
        float n_dot_h = max(dot(fragNormal, h), 0.0);
        specular = vec3(material.Ks * pow(n_dot_h, material.shininessFactor)); 
    }    
}

void main() {
	vec4 textureColor = texture(texture2d, textureCoord);

    vec3 ambientDiffuse;
    vec3 specular;
	phongModel(position, normal, ambientDiffuse, specular);
	
	fragColor = vec4(material.emission + ambientDiffuse, 1.0) * textureColor + vec4(specular, 1.0);
}
