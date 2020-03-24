#include "shader_utils.h"

GLuint LoadShaders(const char *vertexShader, const char *fragmentShader) {

	GLuint vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	std::stringstream ss;

	std::string vertexShaderCode;
	std::ifstream vertexfile(vertexShader, std::ios::in);

	std::cerr << "\nLoading Shaders : " << vertexShader << " ";
	if (vertexfile.is_open()) {
		ss << vertexfile.rdbuf();
		vertexShaderCode = ss.str();
		vertexfile.close();
	} else {
		std::cerr << "| Cannot open File \n";
		return 0;
	}

	std::string fragmentShaderCode;
	std::ifstream fragmentFile(fragmentShader, std::ios::in);

	std::cerr << "\nLoading Shaders : " << fragmentShader << " ";
	if (fragmentFile.is_open()) {
		ss.str(std::string());
		ss << fragmentFile.rdbuf();
		fragmentShaderCode = ss.str();
		fragmentFile.close();
	} else {
		std::cerr << "| Cannot open File \n";
		return 0;
	}

	GLint result = GL_FALSE;
	int infoLogLength = 0;

	//Compile Vertex Shader
	std::cerr << "\nCompiling Shader : " << vertexShader << " \n";
	char const *vertexCodeptr = vertexShaderCode.c_str();
	glShaderSource(vertexShaderID, 1, &vertexCodeptr, NULL);
	glCompileShader(vertexShaderID);

	glGetShaderiv(vertexShaderID, GL_COMPILE_STATUS, &result);
	glGetShaderiv(vertexShaderID, GL_INFO_LOG_LENGTH, &infoLogLength);

	if (!result) {
		std::string VertexShaderErrorMessage;
		VertexShaderErrorMessage.resize(infoLogLength + 1);
		glGetShaderInfoLog(vertexShaderID, infoLogLength, NULL, VertexShaderErrorMessage.data());
		std::cerr << VertexShaderErrorMessage << "\n";
	}

	//Compile Fragment Shader
	std::cerr << "Compiling Shader : " << fragmentShader << " \n";
	char const *fragmentCodeptr = fragmentShaderCode.c_str();
	glShaderSource(fragmentShaderID, 1, &fragmentCodeptr, NULL);
	glCompileShader(fragmentShaderID);

	glGetShaderiv(fragmentShaderID, GL_COMPILE_STATUS, &result);
	glGetShaderiv(fragmentShaderID, GL_INFO_LOG_LENGTH, &infoLogLength);

	if (!result) {
		std::string FragmentShaderErrorMessage;
		FragmentShaderErrorMessage.resize(infoLogLength + 1);
		glGetShaderInfoLog(fragmentShaderID, infoLogLength, NULL, FragmentShaderErrorMessage.data());
		std::cerr << FragmentShaderErrorMessage << "\n";
	}

	//Link
	std::cerr << "Linking Program\n";
	GLuint programID = glCreateProgram();
	glAttachShader(programID, vertexShaderID);
	glAttachShader(programID, fragmentShaderID);
	glLinkProgram(programID);

	glGetProgramiv(programID, GL_LINK_STATUS, &result);
	glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &infoLogLength);

	if (!result) {
		std::string ProgramErrorMessage;
		ProgramErrorMessage.resize(infoLogLength + 1);
		glGetShaderInfoLog(programID, infoLogLength, NULL, ProgramErrorMessage.data());
		std::cerr << ProgramErrorMessage << "\n";
	}

	glDetachShader(programID, vertexShaderID);
	glDetachShader(programID, fragmentShaderID);
	glDeleteShader(vertexShaderID);
	glDeleteShader(fragmentShaderID);

	return programID;
}

void debugCallback(GLenum source, GLenum type, GLuint id, GLenum severity,
                     GLsizei length, const GLchar *message, GLvoid *userParam)
{

    if(id == 131169 || id == 131185 || id == 131218 || id == 131204) return; 

	std::cout << "---------------------------------------" << std::endl;
    std::cout << "Debug : (" << id << ") " << message << std::endl;

	switch (source)
    {
        case GL_DEBUG_SOURCE_API:             std::cout << "Source: API"; break;
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   std::cout << "Source: Window System"; break;
        case GL_DEBUG_SOURCE_SHADER_COMPILER: std::cout << "Source: Shader Compiler"; break;
        case GL_DEBUG_SOURCE_THIRD_PARTY:     std::cout << "Source: Third Party"; break;
        case GL_DEBUG_SOURCE_APPLICATION:     std::cout << "Source: Application"; break;
        case GL_DEBUG_SOURCE_OTHER:           std::cout << "Source: Other"; break;
    } std::cout << std::endl;

    switch (type)
    {
        case GL_DEBUG_TYPE_ERROR:               std::cout << "Type: Error"; break;
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: std::cout << "Type: Deprecated Behaviour"; break;
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  std::cout << "Type: Undefined Behaviour"; break; 
        case GL_DEBUG_TYPE_PORTABILITY:         std::cout << "Type: Portability"; break;
        case GL_DEBUG_TYPE_PERFORMANCE:         std::cout << "Type: Performance"; break;
        case GL_DEBUG_TYPE_MARKER:              std::cout << "Type: Marker"; break;
        case GL_DEBUG_TYPE_PUSH_GROUP:          std::cout << "Type: Push Group"; break;
        case GL_DEBUG_TYPE_POP_GROUP:           std::cout << "Type: Pop Group"; break;
        case GL_DEBUG_TYPE_OTHER:               std::cout << "Type: Other"; break;
    } std::cout << std::endl;
    
    switch (severity)
    {
        case GL_DEBUG_SEVERITY_HIGH:         std::cout << "Severity: high"; break;
        case GL_DEBUG_SEVERITY_MEDIUM:       std::cout << "Severity: medium"; break;
        case GL_DEBUG_SEVERITY_LOW:          std::cout << "Severity: low"; break;
        case GL_DEBUG_SEVERITY_NOTIFICATION: std::cout << "Severity: notification"; break;
    } std::cout << std::endl;
    std::cout << std::endl;
}