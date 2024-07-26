#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

void handle_close(GLFWwindow * window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
	glfwSetWindowShouldClose(window, 1);
    }
}

void window_size_callback(GLFWwindow * window, int width, int height) {
    glViewport(0, 0, width, height);
}

int main(int argc, char * argv[]) {

    if (argc != 2) {
	printf("Usage: view <filename>\n");
	exit(0);
    }

    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(1);
    unsigned char * data = stbi_load(argv[1], &width, &height, &nrChannels, 0);
    if (!data) {
	printf("Invalid Image.\n");
	exit(0);
    }

    glfwInit(); 
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow * window = glfwCreateWindow(1920, 1080, "Test", NULL, NULL);

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, window_size_callback);
    glfwPollEvents(); 

    glewExperimental = GL_TRUE;
    glewInit();
    
    glViewport(0, 0, 1920, 1080);

    float verts [] = {
	-1,-1, 0, 0,
	 1, -1, 1.0, 0,
	 -1, 1, 0, 1.0,
	 1, -1, 1.0, 0.0, //bottom right
	 1, 1, 1.0, 1.0, //top right
	 -1, 1, 0, 1.0,  //top left
    };

    unsigned int VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    unsigned int VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);

    int success;
    const char * vertex_shader_source = "#version 330 core\n"
	"layout (location = 0) in vec2 aPos;\n"
	"layout (location = 1) in vec2 aTexCoord;\n"
	"out vec2 TexCoord;\n"
	"void main()\n"
	"{\n"
	"gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0);\n"
	"TexCoord = aTexCoord;"
	"}\0";

    const char * fragment_shader_source = "#version 330 core\n"
	"out vec4 FragColor;\n"
	"in vec2 TexCoord;\n"
	"uniform sampler2D ourTexture;\n"
	"void main()\n"
	"{\n"
	"FragColor = texture(ourTexture,TexCoord);\n"
	"}\0";

    unsigned int vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vertex_shader_source, NULL);
    glCompileShader(vertex_shader);
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS,&success);
    assert(success);
    
    unsigned int fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fragment_shader_source, NULL);
    glCompileShader(fragment_shader);
    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS,&success);
    assert(success);

    unsigned int shader_program = glCreateProgram();
    glAttachShader(shader_program, fragment_shader);
    glAttachShader(shader_program, vertex_shader);
    glLinkProgram(shader_program);
    glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
    assert(success);
    
    glUseProgram(shader_program);

    glVertexAttribPointer(0,2,GL_FLOAT,GL_FALSE,4*sizeof(float), (void *) 0);
    glVertexAttribPointer(1,2,GL_FLOAT,GL_FALSE,4*sizeof(float), (void *)(2*sizeof(float)));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(data);


    while (!glfwWindowShouldClose(window)) {
	handle_close(window);
	glClearColor(0.0, 0.0, 1.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);

	glBindVertexArray(VAO);
	glUseProgram(shader_program);
	glDrawArrays(GL_TRIANGLES, 0, 6); //this may need to be 6...

	glfwSwapBuffers(window);
	glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
