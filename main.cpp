//=========================================================
//
// Simple SDL and OpenGL implementation
// Feel free to take this and use this, butcher it
// however you want.
//
// Created by Jason Chung
//
//=========================================================

#include <GL\glew.h>
#include <SDL\SDL.h>
#include <SDL\SDL_opengl.h>

#include <iostream> //This is so you can print out to the Output with _RPT1

//The string for a vertex shader
const GLchar* vertexShaderString =
	"#version 150 core\n"
	
	"in vec2 position;"
	"in vec3 colour;"
	
	"out vec3 Colour;"

	"void main() { "
	"	Colour = colour;"
	"	gl_Position = vec4( position, 0.0, 1.0 ); "
	"}";
	
	
//The string for a fragment shader
const GLchar* fragmentShaderString =
	"#version 150 core\n"
	 
	"in vec3 Colour;"
	 
	"out vec4 outColour;"
	
	"void main() { "
	"	outColour = vec4( Colour, 1.0 );"
	"}";

/**
Creates and returns a shader based on the type of shader passed in and
the string of shader data

@param shaderType 	The type of shader like GL_VERTEX_SHADER or GL_FRAGMENT_SHADER
@param shaderData	The string of data that will be read in

@return shader		The compiled shader, otherwise 0 will be returned which is like
					a null shader.
*/
GLuint CreateShader( GLenum shaderType, const GLchar* shaderData )
{
	GLuint shader;
	
	shader = glCreateShader( shaderType );
	
	glShaderSource( shader, 1, &shaderData, NULL );
	glCompileShader( shader );
	
	GLint status;
	
	glGetShaderiv( shader, GL_COMPILE_STATUS, &status );
	
	if( status == GL_FALSE )
	{
		GLint logLength;
		glGetShaderiv( shader, GL_INFO_LOG_LENGTH, &logLength );
		GLchar* log = new GLchar[logLength + 1];
		glGetShaderInfoLog( shader, logLength, NULL, log );
		
		const char* strShaderType = NULL;
		switch( shaderType )
		{
		case GL_VERTEX_SHADER:
			strShaderType = "vertex";
			break;
		case GL_FRAGMENT_SHADER:
			strShaderType = "fragment";
			break;
		case GL_GEOMETRY_SHADER:
			strShaderType = "geometry";
			break;
		}
		
		_RPT1( 0, "%s shader compilation failed: %s\n", strShaderType, log );
		
		delete[] log;
		return 0;
	}
	
	return shader;
}

/**
Creates and returns a compiled program with a vertex and fragment shader
attached to it.

@param _vertexShader	A reference to the vertex shader that will have been
						created by the CreateShader( .. , .. ) function
@param _fragmentShader	A reference to the fragment shader that will have been
						created by the CreateShader( .. , .. ) function
						
@return program			The compiled program, otherwise 0 will be returned
						which is like a null program
*/
GLuint CreateProgram( GLuint& _vertexShader, GLuint& _fragmentShader )
{
	GLuint program;

	program = glCreateProgram();
	
	glAttachShader( program, _vertexShader );
	glAttachShader( program, _fragmentShader );
	
	glBindFragDataLocation( program, 0, "outColour" );
	
	glLinkProgram( program );
	
	GLint status;
	glGetProgramiv( program, GL_LINK_STATUS, &status );
	
	if( status == GL_FALSE )
	{
		GLint logLength;
		glGetProgramiv( program, GL_INFO_LOG_LENGTH, &logLength );
		GLchar* log = new GLchar[logLength + 1];
		glGetProgramInfoLog( program, logLength, NULL, log );
		
		_RPT1( 0, "Failed to link shaders: %s\n", log );
		
		delete[] log;
	}
	
	//glDetachShader( program, _vertexShader );
	//glDetachShader( program, _fragmentShader );
	
	return program;
}


const GLfloat vertexData[] = {
	//Position			//Colour
	//X,    Y,      R,    G,     B,
	0.0f,  0.5f,  1.0f,  0.0f,  0.0f,
	0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
   -0.5f, -0.5f,  0.0f,  0.0f,  1.0f
};


/**
Entry point

As far as I know, you need to have these parameters
@param argc The number of arguments passed in
@param argv[] The arguments in char strings

@return 0 for success, 1 for failure
*/
int main( int argc, char* argv[] )
{
	//The SDL Window you will be rendering to
	SDL_Window* window;
	//The OpenGL context that SDL will be using
	SDL_GLContext context;
	//A Vertex Array object
	GLuint vao;
	//A Vertex Buffer object
	GLuint vbo;
	//A vertex shader
	GLuint vertexShader;
	//A fragment shader
	GLuint fragmentShader;
	//The program that uses the vertex and fragment shader
	GLuint program;
	
	//For this basic program, we're just using video
	SDL_Init( SDL_INIT_VIDEO );
	
	//Setting the OpenGL profile core to 3.2
	SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE );
	SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 3 );
	SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 2 );
	
	//Setting some other stuff that aren't used in this program but will be
	//needed in the open.gl tutorials since these aren't defaulted like other
	//windowing programs like glfw
	SDL_GL_SetAttribute( SDL_GL_RED_SIZE, 8 );
	SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE, 8 );
	SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE, 8 );
	SDL_GL_SetAttribute( SDL_GL_ALPHA_SIZE, 8 );
	SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, 24 );
	SDL_GL_SetAttribute( SDL_GL_STENCIL_SIZE, 8 );
	
	window = SDL_CreateWindow( "OpenGL", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_OPENGL );
	
	if( !window )
	{
		_RPT1( 0, "Failed to create window: %s\n", SDL_GetError () );
		return 1;
	}
	
	context = SDL_GL_CreateContext( window );
	
	if( !context )
	{
		_RPT1( 0, "Failed to create GL context: %s\n", SDL_GetError() );
		return 1;
	}

	//Print out some info about the graphics drivers
	_RPT1( 0, "OpenGL version: %s\n", glGetString( GL_VERSION ) );
	_RPT1( 0, "GLSL version: %s\n", glGetString( GL_SHADING_LANGUAGE_VERSION ) );
	_RPT1( 0, "Vendor: %s\n", glGetString( GL_VENDOR ) );
	_RPT1( 0, "Renderer: %s\n", glGetString( GL_RENDERER ) );
	
	//Enable GLEW
	glewExperimental = GL_TRUE;
	GLenum glewError = glewInit();
	
	if( glewError != GLEW_OK )
	{
		_RPT1( 0, "Failed to initialize GLEW: %s\n", glewGetErrorString( glewError ) );
		return 1;
	}
	
	if( !GLEW_VERSION_3_2 )
	{
		//GLEW 3.2 isn't available on this computer
		return 1;
	}

	glGenVertexArrays( 1, &vao );
	glBindVertexArray( vao );

	glGenBuffers( 1, &vbo );
	
	glBindBuffer( GL_ARRAY_BUFFER, vbo );
	glBufferData( GL_ARRAY_BUFFER, sizeof( vertexData ), vertexData, GL_STATIC_DRAW );
	
	//Create and compile the shaders
	vertexShader = CreateShader( GL_VERTEX_SHADER, vertexShaderString );
	fragmentShader = CreateShader( GL_FRAGMENT_SHADER, fragmentShaderString );
	
	//Create and compile the program
	program = CreateProgram( vertexShader, fragmentShader );
	
	//Specify the layout of the vertex data
	GLint positionAttribute = glGetAttribLocation( program, "position" );
	glEnableVertexAttribArray( positionAttribute );
	glVertexAttribPointer( positionAttribute, 2, GL_FLOAT, GL_FALSE, 5 * sizeof( GL_FLOAT ), 0 );
	GLint colourAttribute = glGetAttribLocation( program, "colour" );
	glEnableVertexAttribArray( colourAttribute );
	glVertexAttribPointer( colourAttribute, 3, GL_FLOAT, GL_FALSE, 5 * sizeof( GL_FLOAT ), (void*)( 2 * sizeof( GL_FLOAT ) ) );
	
	//glEnable( GL_DEPTH_TEST );
	//glDepthFunc( GL_LESS );
	
	//Set the colour that OpenGL will clear the color buffer bit with
	glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
	
	SDL_Event e;
	
	while( true )
	{
		if( SDL_PollEvent( &e ) )
		{
			if( e.type == SDL_QUIT )
				break;
			if( e.type == SDL_KEYDOWN )
			{
				if( e.key.keysym.sym == SDLK_ESCAPE )
				{
					break;
				}
			}
		}
		
		//Clear the colour buffer bit
		glClear( GL_COLOR_BUFFER_BIT );
		
		glUseProgram( program );
		//Render stuff here
		glDrawArrays( GL_TRIANGLES, 0, 3 );
		
		glUseProgram( 0 );

		SDL_GL_SwapWindow( window );
	}
	
	//Cleanup
	glDeleteProgram( program );
	glDeleteShader( fragmentShader );
	glDeleteShader( vertexShader );
	
	glDeleteBuffers( 1, &vbo );
	glDeleteVertexArrays( 1, &vao );
	
	//Destroy the window and delete the context
	SDL_DestroyWindow( window );
	SDL_GL_DeleteContext( context );
	
	return 0;
}