varying vec3 normal, lightDir;
uniform sampler2D tex0, tex1, sun3, earth;
vec4 kolor0, kolor1;

void main() {
	//obliczenie natezenia swiatla - iloczyn kartezjanski wektorow
    	float intensity = max(dot(normal, lightDir), 0.0);

	// obliczenie rozproszenia swiatla, swiatla otoczenia i globalnych warunkow swiatla
	vec4 diffuse = gl_FrontMaterial.diffuse * gl_LightSource[0].diffuse;
	vec4 ambient = gl_FrontMaterial.ambient* gl_LightSource[0].ambient;
	vec4 globalAmbient = gl_LightModel.ambient * gl_FrontMaterial.ambient;

	// zadeklarowanie wektora dla swiatla odbitego
   	vec4 specular = vec4(0.0);
	// obliczenie swiatla odbitego, gdy natezenie oswietlenia > 0
	if (intensity > 0.0) {

		// normalizacja wektora polowkowego i obliczenie kosiuna kata
		// pomiedzy wektorem polowkowym a normalnym
		float intensityHV = max(dot(normal, gl_LightSource[0].halfVector.xyz),0.0);
		// potega okresla stopien skupienia plamy rozblysku
		specular = gl_FrontMaterial.specular * gl_LightSource[0].specular *
				pow(intensityHV,gl_FrontMaterial.shininess);
	}

   	// Wyliczenie koloru piksela na podstawie tekstury:
	kolor0 = texture2D(tex0, gl_TexCoord[0].st);
	kolor1 = texture2D(tex1, gl_TexCoord[1].st);

	// kolor piksela jest proporcjonalny do koloru pikseli tekstur, natezenia i rozproszenia swiatla
	// oraz uzalezniony do swiatla otoczenia, globalnych warunkow otoczenia
	// i swiatla odbitego
	gl_FragColor = kolor0 * kolor1 * intensity * diffuse + ambient + globalAmbient + specular;

}

