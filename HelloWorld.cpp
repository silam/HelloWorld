/*
 Si Lam
 Earth
 **/


#include <stdio.h>
#include <GL/Angel.h>
#include <stdlib.h>
#include <math.h>
#include <IL/il.h> //notice the OpenIL include

#pragma comment(lib, "glew32.lib")
//We have additional libraries to link to now as well
#pragma comment(lib,"ILUT.lib")
#pragma comment(lib,"DevIL.lib")
#pragma comment(lib,"ILU.lib")

//store window width and height
int ww=500, wh=500;

#define M_PI 3.14159265358979323846

//If you want more than one type of shader, store references to your shader program objects
GLuint program1, program2;

GLuint vao[1];
GLuint vbo[2];
int spherevertcount;

//Let's have some mouse dragging rotation
int right_button_down = FALSE;
int left_button_down = FALSE;

int prevMouseX;
int prevMouseY;

double view_rotx = 0.0;
double view_roty = 0.0;
double view_rotz = 0.0;
double z_distance;

int mode = 0; //0 is sphere, 1 is teapot


//our modelview and perspective matrices
mat4 mv, p;

//and we'll need pointers to our shader variables
GLuint model_view;
GLuint projection;

//material properties
GLuint vPosition; //
GLuint vAmbientDiffuseColor; //Ambient and Diffuse can be the same for the material
GLuint vSpecularColor;
GLuint vSpecularExponent;
GLuint vNormal; //this is the standard name

//Some light properties
GLuint light_position;
GLuint light_color;
GLuint ambient_light;

vec4* sphereverts;
vec4* sphere_verts;
vec3* sphere_normals;

int space = 10;
int VertexCount = (180 / space) * (360 / space) * 4;


// EARTH Day time
vec2 * earthtexCoord;
GLuint earthtexMap;
GLuint * earthvao;
GLuint * earthvbo;
static GLuint earchtexName[3];

//We need three texture files
static GLuint * texName;


/// square vertex
vec4 * squareverts;
vec2 * texcoords;

void reshape(int width, int height){
	ww= width;
	wh = height;
	//field of view angle, aspect ratio, closest distance from camera to object, largest distanec from camera to object
	p = Perspective(45.0, (float)width/(float)height, 1.0, 100.0);
	glUniformMatrix4fv(projection, 1, GL_TRUE, p);
	glViewport( 0, 0, width, height );
}

void createSquare()
{
   squareverts = new vec4[6];
   texcoords  = new vec2[6];
   
   squareverts[0] = vec4(-1, -1, 0, 1);
   texcoords[0] = vec2(0, 0);
   squareverts[1] = vec4(1, -1, 0, 1);
   texcoords[1] = vec2(1, 0);
   squareverts[2] = vec4(1, 1, 0, 1);
   texcoords[2] = vec2(1, 1);
   squareverts[3] = vec4(1, 1, 0, 1);
   texcoords[3] = vec2(1, 1);
   squareverts[4] = vec4(-1, 1, 0, 1);
   texcoords[4] = vec2(0, 1);
   squareverts[5] = vec4(-1, -1, 0, 1);
   texcoords[5] = vec2(0, 0);
}
void CreateSphere (double R, double H, double K, double Z) {
    
	int n;
    double a;
    double b;
    n = 0;
    
	sphereverts = new vec4[VertexCount];
	earthtexCoord = new vec2[VertexCount];

	for( b = 0; b <= 90 - space; b+=space){
       
		for( a = 0; a <= 360 - space; a+=space){
            
			sphereverts[n] = vec4( R * sin((a) / 180 * M_PI) * sin((b) / 180 * M_PI) - H,
								   R * cos((a) / 180 * M_PI) * sin((b) / 180 * M_PI) + K,
								   R * cos((b) / 180 * M_PI) - Z, 
								   1);
            earthtexCoord[n] = vec2((2 * b) / 360, 
									(a) / 360 );
            

			n++;
            
			sphereverts[n] = vec4(R * sin((a) / 180 * M_PI) * sin((b + space) / 180 * M_PI) - H,
					             R * cos((a) / 180 * M_PI) * sin((b + space) / 180 * M_PI) + K,
								 R * cos((b + space) / 180 * M_PI) - Z,
								 1);

			earthtexCoord[n] = vec2((2 *  (b + space)) / 360, 
									(a) / 360 );
            
            
			n++;
            

            
			sphereverts[n] = vec4(R * sin((a + space) / 180 * M_PI) * sin((b) / 180 * M_PI) - H,
								R * cos((a + space) / 180 * M_PI) * sin((b) / 180 * M_PI) + K,
								R * cos((b) / 180 * M_PI) - Z,
								1);
         
			earthtexCoord[n] = vec2((2 *  (b)) / 360, 
									(a + space) / 360 );


			n++;
            
			sphereverts[n] = vec4( R * sin((a + space) / 180 * M_PI) * sin((b + space) /180 * M_PI) - H,
									R * cos((a + space) / 180 * M_PI) * sin((b + space) / 180 * M_PI) + K,
									 R * cos((b + space) / 180 * M_PI) - Z,
									 1);
            
			earthtexCoord[n] = vec2((2 *  (b + space)) / 360, 
									(a + space) / 360 );

			n++;
            

            
		}
    
	}
}

//In this particular case, our normal vectors and vertex vectors are identical since the sphere is centered at the origin
//For most objects this won't be the case, so I'm treating them as separate values for that reason
//This could also be done as separate triangle strips, but I've chosen to make them just triangles so I don't have to execute multiple glDrawArrays() commands
int generateSphere(float radius, int subdiv){


	float step = (360.0/subdiv)*(M_PI/180.0);

	int totalverts = ceil(subdiv/2.0)*subdiv * 6;

	if(sphere_normals){
		delete[] sphere_normals;
	}
	sphere_normals = new vec3[totalverts];
	if(sphere_verts){
		delete[] sphere_verts;
	}
	sphere_verts = new vec4[totalverts];

	int k = 0;
	for(float i = -M_PI/2; i<=M_PI/2; i+=step){
		for(float j = -M_PI; j<=M_PI; j+=step){
			//triangle 1
			sphere_normals[k]= vec3(radius*sin(j)*cos(i), radius*cos(j)*cos(i), radius*sin(i));
			sphere_verts[k]=   vec4(radius*sin(j)*cos(i), radius*cos(j)*cos(i), radius*sin(i), 1.0);
			k++;
	
			sphere_normals[k]= vec3(radius*sin(j)*cos(i+step), radius*cos(j)*cos(i+step), radius*sin(i+step));
			sphere_verts[k]=   vec4(radius*sin(j)*cos(i+step), radius*cos(j)*cos(i+step), radius*sin(i+step), 1.0);
			k++;
			
			sphere_normals[k]= vec3(radius*sin((j+step))*cos((i+step)), radius*cos(j+step)*cos(i+step), radius*sin(i+step));
			sphere_verts[k]=   vec4(radius*sin((j+step))*cos((i+step)), radius*cos(j+step)*cos(i+step), radius*sin(i+step), 1.0);
			k++;

			//triangle 2
			sphere_normals[k]= vec3(radius*sin((j+step))*cos((i+step)), radius*cos(j+step)*cos(i+step), radius*sin(i+step));
			sphere_verts[k]=   vec4(radius*sin((j+step))*cos((i+step)), radius*cos(j+step)*cos(i+step), radius*sin(i+step), 1.0);
			k++;

			sphere_normals[k]= vec3(radius*sin(j+step)*cos(i), radius*cos(j+step)*cos(i), radius*sin(i));
			sphere_verts[k]=   vec4(radius*sin(j+step)*cos(i), radius*cos(j+step)*cos(i), radius*sin(i), 1.0);
			k++;

			sphere_normals[k]= vec3(radius*sin(j)*cos(i), radius*cos(j)*cos(i), radius*sin(i));
			sphere_verts[k]=   vec4(radius*sin(j)*cos(i), radius*cos(j)*cos(i), radius*sin(i), 1.0);
			k++;
		}
	}
	return totalverts;
}

void display(void)
{
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  
	
    mat4 camera = mv =  LookAt(vec4(0,0,5.0+z_distance,1),vec4(0,0,0,1),vec4(0,1,0,0))* RotateX(view_rotx) * RotateY(view_roty) * RotateZ(view_rotz);
	glUniformMatrix4fv(model_view, 1, GL_TRUE, mv*Translate(0,0,1));
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texName[0]); //which texture do we want?
	glDrawArrays( GL_TRIANGLES, 0, 6 );

	//mv = camera * RotateY(90)* Translate(0,0,1);
	//glUniformMatrix4fv(model_view, 1, GL_TRUE, mv);
	//
	//glBindTexture(GL_TEXTURE_2D, texName[1]); //which texture do we want?
	//glDrawArrays( GL_TRIANGLES, 0, 6 );

	//mv = camera* RotateY(-90) * Translate(0,0,1);
	//glUniformMatrix4fv(model_view, 1, GL_TRUE, mv);
	//glBindTexture(GL_TEXTURE_2D, texName[2]); //which texture do we want?
	//glDrawArrays( GL_TRIANGLES, 0, 6 );

   glutSwapBuffers();


}

void displaylight(void)
{
  /*clear all pixels*/
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
  //Take care of any mouse rotations or panning
    mv = LookAt(vec4(0, 0, 10+z_distance, 1.0), vec4(0, 0, 0, 1.0), vec4(0, 1, 0, 0.0));

	mv = mv * RotateX(view_rotx) * RotateY(view_roty) * RotateZ(view_rotz);

	glUniformMatrix4fv(model_view, 1, GL_TRUE, mv);
	
	
	glUniform4fv(ambient_light, 1, vec4(0.2, 0.2, 0.2, 1));
	glUniform4fv(light_color, 1, vec4(1, 0, 0, 1));
	glUniform4fv(light_position, 1, mv*vec4(50,50, 50, 1));

	glVertexAttrib4fv(vAmbientDiffuseColor, vec4(0, .5, 0, 1));
	glVertexAttrib4fv(vSpecularColor, vec4(1, 1, 1, 1));
	glVertexAttrib1f(vSpecularExponent, 10);

    
	if(mode == 0){
		glBindVertexArray( vao[0] );
		glDrawArrays( GL_TRIANGLES, 0, spherevertcount );    // draw the sphere 
	}else{
		//WARNING:
		//Nvidia kludge: If you want the teapot to show up correctly, make sure that 
		//vPosition is the first attribute listed in your vertex shader, and vNormal
		//is the 3rd attribute listed.  Not sure what ATI cards will do with this
		glBindVertexArray(0);
		glutSolidTeapot(2); //not very bandwidth efficient
	}

    
    glFlush();
  /*start processing buffered OpenGL routines*/
  glutSwapBuffers();
}

//Connect all of the shader variables to local pointers
void setupShader(GLuint prog){
	glUseProgram( prog );
	model_view = glGetUniformLocation(prog, "model_view");
	projection = glGetUniformLocation(prog, "projection");
	
	vAmbientDiffuseColor = glGetAttribLocation(prog, "vAmbientDiffuseColor");
	vSpecularColor = glGetAttribLocation(prog, "vSpecularColor");
	vSpecularExponent = glGetAttribLocation(prog, "vSpecularExponent");
	light_position = glGetUniformLocation(prog, "light_position");
	light_color = glGetUniformLocation(prog, "light_color");
	ambient_light = glGetUniformLocation(prog, "ambient_light");

	//Our vertex array has to deal with a different shader now
	glBindVertexArray( vao[0] );

	glBindBuffer( GL_ARRAY_BUFFER, vbo[0] );
	vPosition = glGetAttribLocation(prog, "vPosition");
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer( GL_ARRAY_BUFFER, vbo[1] );
	vNormal = glGetAttribLocation(prog, "vNormal");
	glEnableVertexAttribArray(vNormal);
	glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 0, 0);

	//Don't forget to send the projection matrix to our new shader!
	glUniformMatrix4fv(projection, 1, GL_TRUE, p);
}


void Keyboard(unsigned char key, int x, int y) {
	/*exit when the escape key is pressed*/
	if (key == 27)
		exit(0);

	if (key == 'g'){
		setupShader(program1);
	}
	//eventually we'll have a second shader program
	/*if (key == 'p'){
		setupShader(program2);
	}*/  

	if (key == 's'){
		mode = 0;
	}
	if (key == 't'){
		mode = 1;
	}
	glutPostRedisplay();

}

//Mouse drags = rotation
void mouse_dragged(int x, int y) {
	double thetaY, thetaX;
	if (left_button_down) {
		thetaY = 360.0 *(x-prevMouseX)/ww;    
		thetaX = 360.0 *(prevMouseY - y)/wh;
		prevMouseX = x;
		prevMouseY = y;
		view_rotx += thetaX;
		view_roty += thetaY;
	}
	else if (right_button_down) {
		z_distance = 5.0*(prevMouseY-y)/wh;
	}
  glutPostRedisplay();
}


void mouse(int button, int state, int x, int y) {
  //establish point of reference for dragging mouse in window
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
      left_button_down = TRUE;
	  prevMouseX= x;
      prevMouseY = y;
    }

	else if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {
      right_button_down = TRUE;
      prevMouseX = x;
      prevMouseY = y;
    }
    else if (state == GLUT_UP) {
      left_button_down = FALSE;
	  right_button_down = FALSE;
	}
}
 
//Modified slightly from the OpenIL tutorials
ILuint loadTexFile(const char* filename){
	
	ILboolean success; /* ILboolean is type similar to GLboolean and can equal GL_FALSE (0) or GL_TRUE (1)
    it can have different value (because it's just typedef of unsigned char), but this sould be
    avoided.
    Variable success will be used to determine if some function returned success or failure. */


	/* Before calling ilInit() version should be checked. */
	  if (ilGetInteger(IL_VERSION_NUM) < IL_VERSION)
	  {
		/* wrong DevIL version */
		printf("Wrong IL version");
		exit(1);
	  }
 
	  success = ilLoadImage((LPTSTR)filename); /* Loading of image from file */
	if (success){ /* If no error occured: */
		//We need to figure out whether we have an alpha channel or not
		  if(ilGetInteger(IL_IMAGE_BPP) == 3){
			success = ilConvertImage(IL_RGB, IL_UNSIGNED_BYTE); /* Convert every color component into
		  unsigned byte. If your image contains alpha channel you can replace IL_RGB with IL_RGBA */
		  }else if(ilGetInteger(IL_IMAGE_BPP) == 4){
			  success = ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);
		  }else{
			  success = false;
		  }
		if (!success){
		  /* Error occured */
		 printf("failed conversion to unsigned byte");
		 exit(1);
		}
	}else{
		/* Error occured */
	   printf("Failed to load image ");
	   printf(filename);
		exit(1);
	}
}

void SetupTextureShader(GLuint program1, 
						GLuint * vao, 
						GLuint * vbo, 
						vec2 * texcoords, 
						vec4  * verts, 
						GLuint * texName, 
						ILuint * ilTexID,
						int    imagenumber,
						int    counts)
{
	// Create a vertex array object
    glGenVertexArrays( 1, &vao[0] );

    // Create and initialize any buffer objects
	glBindVertexArray( vao[0] );
	glGenBuffers( 2, &vbo[0] );
    glBindBuffer( GL_ARRAY_BUFFER, vbo[0] );
    glBufferData( GL_ARRAY_BUFFER, counts*sizeof(vec4), verts, GL_STATIC_DRAW);

	glBindBuffer( GL_ARRAY_BUFFER, vbo[1] );
    glBufferData( GL_ARRAY_BUFFER, counts*sizeof(vec2), texcoords, GL_STATIC_DRAW);

 
	ilBindImage(ilTexID[imagenumber]); /* Binding of IL image name */
	loadTexFile("images/Earth.png");
	glBindTexture(GL_TEXTURE_2D, texName[imagenumber]); //bind OpenGL texture name

   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
   //Note how we depend on OpenIL to supply information about the file we just loaded in
   glTexImage2D(GL_TEXTURE_2D, 0, ilGetInteger(IL_IMAGE_BPP), ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT),0,
	   ilGetInteger(IL_IMAGE_FORMAT), ilGetInteger(IL_IMAGE_TYPE), ilGetData());



    GLuint texMap = glGetUniformLocation(program1, "texture");
	glUniform1i(texMap, 0);//assign this one to texture unit 0


	glBindBuffer( GL_ARRAY_BUFFER, vbo[0] );
	vPosition = glGetAttribLocation(program1, "vPosition");
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer( GL_ARRAY_BUFFER, vbo[1] );
	GLuint vtexCoord = glGetAttribLocation(program1, "texCoord");
	glEnableVertexAttribArray(vtexCoord);
	glVertexAttribPointer(vtexCoord, 2, GL_FLOAT, GL_FALSE, 0, 0);



	//Now repeat the process for the second image
	//ilBindImage(ilTexID[1]);
}
void init() {

  /*select clearing (background) color*/
  glClearColor(0.0, 0.0, 0.0, 0.0);


  //populate our arrays
  spherevertcount = generateSphere(2, 100);
  
	//CreateSphere(5, 0,0,0);


	createSquare();

   // Load shaders and use the resulting shader program
    program1 = InitShader( "vshader-lighting.glsl", "fshader-lighting.glsl" );
	//program2 = InitShader( "vshader-phongshading.glsl", "fshader-phongshading.glsl" );

	ILuint * ilTexID = new ILuint[3]; /* ILuint is a 32bit unsigned integer.
    //Variable texid will be used to store image name. */
	texName   = new GLuint[3];


	ilInit(); /* Initialization of OpenIL */
	ilGenImages(3, ilTexID); /* Generation of three image names for OpenIL image loading */
	glGenTextures(3, texName); //and we eventually want the data in an OpenGL texture

	earthvao = new GLuint[1];
	earthvbo = new GLuint[2];
	




	//SetupTextureShader(program1, earthvao, earthvbo, earthtexCoord, sphereverts, texName, ilTexID, 0, VertexCount);
	SetupTextureShader(program1, earthvao, earthvbo, texcoords, squareverts, texName, ilTexID, 0, 6);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


	/*
	// Create a vertex array object
    glGenVertexArrays( 1, &vao[0] );

    // Create and initialize any buffer objects
	glBindVertexArray( vao[0] );
	glGenBuffers( 2, &vbo[0] );
    glBindBuffer( GL_ARRAY_BUFFER, vbo[0] );
    glBufferData( GL_ARRAY_BUFFER, spherevertcount*sizeof(vec4), sphere_verts, GL_STATIC_DRAW);
	

	//and now our colors for each vertex
	glBindBuffer( GL_ARRAY_BUFFER, vbo[1] );
	glBufferData( GL_ARRAY_BUFFER, spherevertcount*sizeof(vec3), sphere_normals, GL_STATIC_DRAW );

	setupShader(program1);
	*/



  //Only draw the things in the front layer
	glEnable(GL_DEPTH_TEST);
}

int main(int argc, char **argv)
{
  /*set up window for display*/
  glutInit(&argc, argv);
  glutInitWindowPosition(0, 0); 
  glutInitWindowSize(ww, wh);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
  glutCreateWindow("Lighting Exercise");  

  glewExperimental = GL_TRUE;

	glewInit();
  init();

  glutDisplayFunc(display);
  glutKeyboardFunc(Keyboard);
  glutReshapeFunc(reshape);
  //glutIdleFunc(idle);
  glutMouseFunc(mouse);
  glutMotionFunc(mouse_dragged);

  glutMainLoop();
  return 0;
}