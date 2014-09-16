varying vec3 normal, lightDir;

void main() {
   	// skonwertowanie wektora normalnego do wsplrzednych swiata
	normal = normalize(gl_NormalMatrix * gl_Normal);
	// normalizacja polozenia swiatla
	lightDir = normalize(vec3(gl_LightSource[0].position));
	
	// Przypisanie wspolrzednych tekstur:
	gl_TexCoord[0] = gl_MultiTexCoord0;
	gl_TexCoord[1] = gl_MultiTexCoord0;
	gl_Position = ftransform();
}

