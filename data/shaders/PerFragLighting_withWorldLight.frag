// PerFragLighting_withWorldLight.frag
#version 400

in vec3 position;
in vec3 normal;

out layout (location = 0) vec4 fragColor;

struct LightSource {
    vec3 position;      // Light position in eye coordinate space.
    vec3 rgbIntensity;  // Light intensity for each RGB component.
};
uniform LightSource lightSource;

uniform vec3 ambientIntensity; // Environmental ambient light intensity for each RGB component.

struct MaterialProperties {
    vec3 emission;  // Emission light intensity from material for each RGB component.
    vec3 Ka;        // Coefficients of ambient reflectivity for each RGB component.
    vec3 Kd;        // Coefficients of diffuse reflectivity for each RGB component.
    float Ks;       // Coefficient of specular reflectivity, uniform across each RGB component.
    float shininess;  // Specular shininess factor.
};
uniform MaterialProperties material;

vec3 eadsLightLevel(vec3 fragPosition, vec3 fragNormal) {
    vec3 l = normalize(lightSource.position - fragPosition); // Direction from fragment to light source.
    vec3 v = normalize(-fragPosition); // Direction from fragment to viewer (origin - fragPosition).
    vec3 h = normalize(v + l); // Halfway vector.

    vec3 ambient = ambientIntensity * material.Ka;

    float n_dot_l = max(dot(fragNormal, l), 0.0);
    vec3 diffuse = material.Kd * n_dot_l;
    
    vec3 specular = vec3(0.0);
    if (n_dot_l > 0.0) {
        float n_dot_h = max(dot(fragNormal, h), 0.0);
        specular = vec3(material.Ks * pow(n_dot_h, material.shininess)); 
    }    
   
    return material.emission + ambient + lightSource.rgbIntensity * (diffuse + specular);
}

void main() {
    fragColor = vec4(eadsLightLevel(position, normal), 1.0);
}
