/* OpenGL example code - Indexed VBO
 * 
 * same as the "Shader and VBO" example, only with an indexed vbo.
 * 
 * Autor: Jakob Progsch
 */

#include <GL/glew.h>
#include <GL/glfw.h>

#include <iostream>
#include <string>
#include <vector>

bool running;

// window close callback function
int closedWindow()
{
    running = false;
    return GL_TRUE;
}

// helper to check and display for shader compiler errors
bool check_shader_compile_status(GLuint obj)
{
    GLint status;
    glGetShaderiv(obj, GL_COMPILE_STATUS, &status);
    if(status == GL_FALSE)
    {
        GLint length;
        glGetShaderiv(obj, GL_INFO_LOG_LENGTH, &length);
        std::vector<char> log(length);
        glGetShaderInfoLog(obj, length, &length, &log[0]);
        std::cerr << &log[0];
        return false;
    }
    return true;
}

// helper to check and display for shader linker error
bool check_program_link_status(GLuint obj)
{
    GLint status;
    glGetProgramiv(obj, GL_LINK_STATUS, &status);
    if(status == GL_FALSE)
    {
        GLint length;
        glGetProgramiv(obj, GL_INFO_LOG_LENGTH, &length);
        std::vector<char> log(length);
        glGetProgramInfoLog(obj, length, &length, &log[0]);
        std::cerr << &log[0];
        return false;   
    }
    return true;
}

int main()
{
    int width = 640;
    int height = 480;
    
    glfwInit();

    // sadly glew doesn't play nice with core profiles... 
    glfwOpenWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
    glfwOpenWindowHint(GLFW_OPENGL_VERSION_MAJOR, 3);
    glfwOpenWindowHint(GLFW_OPENGL_VERSION_MINOR, 3);
 
    // create a window
    glfwOpenWindow(width, height, 0, 0, 0, 0, 0, 0, GLFW_WINDOW);
    
    // setup windows close callback
    glfwSetWindowCloseCallback(closedWindow);
    
    glewExperimental = GL_TRUE;
    if (GLEW_OK != glewInit())
    {
        std::cerr << "failed to init GLEW" << std::endl;
        glfwTerminate();
        return 1;
    }

    // shader source code
    std::string vertex_source =
        "#version 330\n"
        "in vec4 vposition;\n"
        "in vec4 vcolor;\n"
        "out vec4 fcolor;\n"
        "void main() {\n"
        "   fcolor = vcolor;\n"
        "   gl_Position = vposition;\n"
        "}\n";
        
    std::string fragment_source =
        "#version 330\n"
        "in vec4 fcolor;\n"
        "out vec4 FragColor;\n"
        "void main() {\n"
        "   FragColor = fcolor;\n"
        "}\n";
   
    // program and shader handles
    GLuint shader_program, vertex_shader, fragment_shader;
    
    // we need these to properly pass the strings
    const char *source;
    int length;

    // create and compiler vertex shader
    vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    source = vertex_source.c_str();
    length = vertex_source.size();
    glShaderSource(vertex_shader, 1, &source, &length); 
    glCompileShader(vertex_shader);
    if(!check_shader_compile_status(vertex_shader))
    {
        return 1;
    }
 
    // create and compiler fragment shader
    fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    source = fragment_source.c_str();
    length = fragment_source.size();
    glShaderSource(fragment_shader, 1, &source, &length);   
    glCompileShader(fragment_shader);
    if(!check_shader_compile_status(fragment_shader))
    {
        return 1;
    }
    
    // create program
    shader_program = glCreateProgram();
    
    // attach shaders
    glAttachShader(shader_program, vertex_shader);
    glAttachShader(shader_program, fragment_shader);
        
    // bind the attribute locations (inputs)
    glBindAttribLocation(shader_program, 0, "vposition");
    glBindAttribLocation(shader_program, 1, "vcolor");
    
    // bind the FragDataLocation (output)
    glBindFragDataLocation(shader_program, 0, "FragColor");
    
    // link the program and check for errors
    glLinkProgram(shader_program);
    check_program_link_status(shader_program);
    
    
    // vao and vbo handle
    GLuint vao, vbo, ibo;
 
    // generate and bind the vao
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    
    // generate and bind the vertex buffer object
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
            
    // data for a fullscreen quad
    GLfloat vertexData[] = {
    //  X     Y     Z           R     G     B
       1.0f, 1.0f, 0.0f,       1.0f, 0.0f, 0.0f, // vertex 0
      -1.0f, 1.0f, 0.0f,       0.0f, 1.0f, 0.0f, // vertex 1
       1.0f,-1.0f, 0.0f,       0.0f, 0.0f, 1.0f, // vertex 2
      -1.0f,-1.0f, 0.0f,       1.0f, 0.0f, 0.0f, // vertex 3
    }; // 4 vertices with 6 components (floats) each

    // fill with data
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*4*6, vertexData, GL_STATIC_DRAW);
                    
           
    // set up generic attrib pointers
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6*sizeof(GLfloat), (char*)0 + 0*sizeof(GLfloat));
 
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6*sizeof(GLfloat), (char*)0 + 3*sizeof(GLfloat));
    
    
    // generate and bind the index buffer object
    glGenBuffers(1, &ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
            
    GLuint indexData[] = {
        0,1,2, // first triangle
        2,1,3, // second triangle
    };

    // fill with data
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint)*2*3, indexData, GL_STATIC_DRAW);
    
    // "unbind" vao
    glBindVertexArray(0);

    running = true;
    while(running)
    {    
        // terminate on excape 
        if(glfwGetKey(GLFW_KEY_ESC))
        {
            running = false;
        }
        
        // clear first
        glClear(GL_COLOR_BUFFER_BIT);
        
        // use the shader program
        glUseProgram(shader_program);
        
        // bind the vao
        glBindVertexArray(vao);
       
        // draw
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
       
        // check for errors
        GLenum error = glGetError();
        if(error != GL_NO_ERROR)
        {
            std::cerr << gluErrorString(error);
            running = false;       
        }
        
        // finally swap buffers
        glfwSwapBuffers();       
    }

    glfwTerminate();
    return 0;
}
