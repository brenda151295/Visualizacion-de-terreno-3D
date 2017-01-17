#define GLUT_DISABLE_ATEXIT_HACK
//#include <windows.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>

#include <vector>
#include <string>
#include <map>
#include <string.h>
#include <fstream>
#include <pthread.h>
#include <utility> 

#include <GL/glut.h>
using namespace std;
#include "TextureManager.h"



#define RED 0	
#define GREEN 0
#define BLUE 0
#define ALPHA 1

#define ECHAP 27


void init_scene();
void render_scene();
GLvoid initGL();
GLvoid window_display();
GLvoid window_reshape(GLsizei width, GLsizei height);
GLvoid window_key(unsigned char key, int x, int y);


GLint texture;
float delta_x = 0.0; 
float delta_y = 0.0;
float mouse_x, mouse_y;
float var_x = 0.0;
float var_z = -1000.0;
float step = 0; 

GLfloat light0Position[4];
GLfloat light_axis_x=0,light_axis_y=0,light_axis_z=0;



struct Point
{
	GLfloat x,y,z; 
	GLfloat Normal[3]; 
	GLfloat s,t; 
};
struct Triangulo
{
	GLfloat x[3];
	GLfloat y[3];
	GLfloat z[3];
	GLfloat Normal[3];
};

class Terreno
{
private:
	int num_tri;
	int size_lista_puntos;
public:
	vector<Point> lista_puntos;
	vector<Triangulo> lista_triangulos;
	int n_x=101, m_z=101;
	int tri_x = n_x - 1;
	int tri_z = m_z - 1;
	float size_x=30, size_z=30;
	Terreno(string filename)
	{	
		ifstream archivo(filename);
		string x,y,z;
		string::size_type sz;
		int cont = 0;
		int cont_x = 0,cont_z = 0;
		float cont_s = 0.0, cont_t = 0.0;
		while(!archivo.eof()) 
		{
			if(cont==n_x)
			{
				cont_z += size_z;
				cont_t += 0.0099;
				cont_s = 0.0;
				cont_x = 0;
				cont = 0;
				
			}
			Point point;
      		archivo >> y;
      		point.x = cont_x;
      		point.y = stof(y,&sz);
      		point.z = cont_z;
      		point.s = cont_s;
      		point.t = cont_t;
      		lista_puntos.push_back(point);
      		cont+=1;
      		cont_x+=size_x;
      		cont_s+=0.0099;
   		}
   		archivo.close();
   		computeNormals();
	}
	void calcularNormal(GLfloat p1x, GLfloat p1y, GLfloat p1z, GLfloat p2x, GLfloat p2y, GLfloat p2z, 
	GLfloat p3x, GLfloat p3y, GLfloat p3z, GLfloat N[])
	{	
		GLfloat V1x = p2x-p1x;
		GLfloat V1y = p2y-p1y;
		GLfloat V1z = p2z-p1z;

		GLfloat V2x = p3x-p2x;
		GLfloat V2y = p3y-p2y;
		GLfloat V2z = p3z-p2z;

		N[0] = V1y*V2z - V1z*V2y;
		N[1] = V1z*V2x - V1x*V2z;
		N[2] = V1x*V2y - V1y*V2x;

		Normaliza(N);
	}

	GLfloat Modulo(GLfloat N[])
	{
		GLfloat len;
	 	len = N[0]*N[0]+ N[1]*N[1] + N[2]*N[2];
	 	return (sqrt(len));
	}

	GLvoid Normaliza(GLfloat N[])
	{
	 	GLfloat len;
		len = Modulo(N);
		len = 1.0/len;
		N[0] *= len;
		N[1] *= len;
		N[2] *= len;
	}

	vector<Triangulo> encontrar_vecinos(int i,int j)
	{
		vector<Triangulo> vecinos;
		int pos = i*m_z+j;
		int pos_tri = (j*2)+(i*((m_z-1)*2));
		if(j==0)
		{
			if(i!=(n_x-1))
				vecinos.push_back(lista_triangulos[pos_tri]);
		}
		else
		{
			if(i!=(n_x-1))
			{
				vecinos.push_back(lista_triangulos[pos_tri-1]);
				vecinos.push_back(lista_triangulos[pos_tri-2]);
				if(j!=(m_z-1))
					vecinos.push_back(lista_triangulos[pos_tri]);
			}
		}
		if(i>0)
		{
			pos_tri = pos_tri-((m_z-1)*2);
			if(j==0)
			{
				vecinos.push_back(lista_triangulos[pos_tri]);
				vecinos.push_back(lista_triangulos[pos_tri+1]);
			}
			else
			{
				if(j==(m_z-1))
					vecinos.push_back(lista_triangulos[pos_tri-1]);
				else
				{
					vecinos.push_back(lista_triangulos[pos_tri-1]);
					vecinos.push_back(lista_triangulos[pos_tri]);
					vecinos.push_back(lista_triangulos[pos_tri+1]);
				}
			}
		}
		return vecinos;
	}

	void computeNormals()
	{
		for(int i=0;i<n_x-1;i++)
		{
			for(int j=0;j<m_z-1;j++)
			{
				Triangulo pri,seg;
	        	calcularNormal(lista_puntos[i*m_z+j+1].x, lista_puntos[i*m_z+j+1].z, lista_puntos[i*m_z+j+1].y,
	        					lista_puntos[(i+1)*m_z+j].x, lista_puntos[(i+1)*m_z+j].z, lista_puntos[(i+1)*m_z+j].y,
	        					lista_puntos[i*m_z+j].x, lista_puntos[i*m_z+j].z, lista_puntos[i*m_z+j].y,
	        					pri.Normal);

	        	calcularNormal(lista_puntos[(i+1)*m_z+j+1].x, lista_puntos[(i+1)*m_z+j+1].z, lista_puntos[(i+1)*m_z+j+1].y,
	        					lista_puntos[(i+1)*m_z+j].x, lista_puntos[(i+1)*m_z+j].z, lista_puntos[(i+1)*m_z+j].y,
	        					lista_puntos[i*m_z+j+1].x, lista_puntos[i*m_z+j+1].z, lista_puntos[i*m_z+j+1].y,
	        					seg.Normal);

	        	lista_triangulos.push_back(pri);
	        	lista_triangulos.push_back(seg);
			}
		}
		GLfloat x,y,z;
		for(int i=0;i<n_x;i++)
		{
			for(int j=0;j<m_z;j++)
			{
				x = 0.0,y = 0.0,z = 0.0;
				vector<Triangulo> vecinos;
				vecinos = encontrar_vecinos(i,j);
				
				for(int i=0;i<vecinos.size();i++)
				{
					x+=vecinos[i].Normal[0];
					y+=vecinos[i].Normal[1];
					z+=vecinos[i].Normal[2];
				}
				lista_puntos[i*m_z+j].Normal[0] = x/float(vecinos.size());
				lista_puntos[i*m_z+j].Normal[1] = y/float(vecinos.size());
				lista_puntos[i*m_z+j].Normal[2] = z/float(vecinos.size());
			}
		}
	}
	void display(GLint texture)
	{

		for(int i=0;i<n_x-1;i++)
		{
			for(int j=0;j<m_z-1;j++)
			{
				GLfloat Normal[3];
				glBegin(GL_TRIANGLES);

	        	glPushMatrix();
	        	glBindTexture(GL_TEXTURE_2D, texture);
	        	glNormal3fv(lista_puntos[i*m_z+j+1].Normal);
	        	glTexCoord2f(lista_puntos[i*m_z+j+1].s, lista_puntos[i*m_z+j+1].t);
	        	glVertex3f(lista_puntos[i*m_z+j+1].x, lista_puntos[i*m_z+j+1].z, lista_puntos[i*m_z+j+1].y);

	        	glNormal3fv(lista_puntos[(i+1)*m_z+j].Normal);
	        	glTexCoord2f(lista_puntos[(i+1)*m_z+j].s, lista_puntos[(i+1)*m_z+j].t);
	        	glVertex3f(lista_puntos[(i+1)*m_z+j].x, lista_puntos[(i+1)*m_z+j].z, lista_puntos[(i+1)*m_z+j].y);

	        	glNormal3fv(lista_puntos[i*m_z+j].Normal);
	        	glTexCoord2f(lista_puntos[i*m_z+j].s, lista_puntos[i*m_z+j].t);
	        	glVertex3f(lista_puntos[i*m_z+j].x, lista_puntos[i*m_z+j].z, lista_puntos[i*m_z+j].y);

	        	glPopMatrix();

	        	glPushMatrix();
	        	glBindTexture(GL_TEXTURE_2D, texture);

	        	glNormal3fv(lista_puntos[(i+1)*m_z+j+1].Normal);
	        	glTexCoord2f(lista_puntos[(i+1)*m_z+j+1].s, lista_puntos[(i+1)*m_z+j+1].t);
	        	glVertex3f(lista_puntos[(i+1)*m_z+j+1].x, lista_puntos[(i+1)*m_z+j+1].z, lista_puntos[(i+1)*m_z+j+1].y);

	        	glNormal3fv(lista_puntos[(i+1)*m_z+j].Normal);
	        	glTexCoord2f(lista_puntos[(i+1)*m_z+j].s, lista_puntos[(i+1)*m_z+j].t);
	        	glVertex3f(lista_puntos[(i+1)*m_z+j].x, lista_puntos[(i+1)*m_z+j].z, lista_puntos[(i+1)*m_z+j].y);

	        	glNormal3fv(lista_puntos[i*m_z+j+1].Normal);
	        	glTexCoord2f(lista_puntos[i*m_z+j+1].s, lista_puntos[i*m_z+j+1].t);
	        	glVertex3f(lista_puntos[i*m_z+j+1].x, lista_puntos[i*m_z+j+1].z, lista_puntos[i*m_z+j+1].y);
	        	glPopMatrix();
	        	
	        	glEnd();
			}
		}
	}
};	



Terreno terreno("fontvieille.txt");


//function called on each frame
GLvoid window_idle()
{
	glutPostRedisplay();
}

GLvoid callback_special(int key, int x, int y)
{
	switch (key)
	{
	case GLUT_KEY_UP:
		var_z += 50.0;
		glutPostRedisplay();
		break;

	case GLUT_KEY_DOWN:
		var_z -= 50.0;
		glutPostRedisplay();
		break;

	case GLUT_KEY_LEFT:
		var_x += 50.0;
		glutPostRedisplay();
		break;

	case GLUT_KEY_RIGHT:
		var_x -= 50.0;
		glutPostRedisplay();
		break;

	case GLUT_KEY_PAGE_UP:
		light_axis_z+=15;
		glutPostRedisplay();
		break;
	case GLUT_KEY_PAGE_DOWN:
		light_axis_z-=15;
		glutPostRedisplay();
		break;
	}
}


GLvoid window_key(unsigned char key, int x, int y)
{
	switch (key) {
	case ECHAP:
		exit(1);
		break;
	case 99:
		light_axis_x+=15;
		glutPostRedisplay();
		break;
	case 122:
		light_axis_x-=15;
		glutPostRedisplay();
		break;
	case 115:
		light_axis_y+=15;
		glutPostRedisplay();
		break;
	case 120:
		light_axis_y-=15;
		glutPostRedisplay();
		break;
	default:
		printf("La touche %d non active.\n", key);
		break;
	}
}

GLvoid callback_mouse(int button, int state, int x, int y)
{
	if (state == GLUT_DOWN && button == GLUT_LEFT_BUTTON)
	{
		mouse_x = x;
		mouse_y = y;
	}
}

GLvoid callback_motion(int x, int y)
{
	
	delta_x += x - mouse_x;
	delta_y += y - mouse_y;
	mouse_x = x;
	mouse_y = y;
	glutPostRedisplay();
}



int main(int argc, char **argv)
{
	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);

	glutInitWindowSize(800, 800);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("TP 2 : Transformaciones");


	initGL();

	texture = TextureManager::Inst()->LoadTexture("fontvieille.tga", GL_BGR_EXT, GL_RGB);

	init_scene();
	glutDisplayFunc(&window_display);
	glutReshapeFunc(&window_reshape);

	
	glutMouseFunc(&callback_mouse);
	glutMotionFunc(&callback_motion);
	glutKeyboardFunc(&window_key);

	glutSpecialFunc(&callback_special);
	glutIdleFunc(&window_idle);
	glutMainLoop();
	return 1;
}


void init_scene(){}



void LPosition(GLfloat x,GLfloat y,GLfloat z)
{
	glPushMatrix();
	glTranslatef(x,y,z);
	glutSolidSphere(30,20,20);	
	glPopMatrix();
}

GLvoid initGL()
{
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glShadeModel(GL_SMOOTH);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_COLOR_MATERIAL);
	glClearColor(RED, GREEN, BLUE, ALPHA);


	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_NORMALIZE);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);
	glClearColor(RED, GREEN, BLUE, ALPHA);
	glEnable(GL_TEXTURE_2D);
	glShadeModel(GL_SMOOTH); 
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL); 
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST); 
}

GLvoid window_display()
{
	light0Position[0]=light_axis_x;
	light0Position[1]=light_axis_y;
	light0Position[2]=light_axis_z;
	light0Position[3]=1.0f;
	
	GLfloat light0Ambient [] = {0.1f, 0.1f, 0.1f, 1.0f};
	GLfloat light0Difusa [] = {0.5f, 0.5f, 0.5f, 1.0f};
	GLfloat light0Especular [] = {1.0f, 1.0f, 1.0f, 1.0f};


	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective(90.0f, 1.0f, 0.01f, 15050.0f);
	glTranslatef(var_x, 0.0, var_z);

	glRotatef(delta_x, 0.0, 1.0, 0.0);
	glRotatef(delta_y, 1.0, 0.0, 0.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glLightfv(GL_LIGHT0, GL_AMBIENT, light0Ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light0Difusa);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light0Especular);

	glLightfv(GL_LIGHT0, GL_POSITION, light0Position);
	LPosition(light_axis_x,light_axis_y,light_axis_z);

	terreno.display(texture);
	

	glutSwapBuffers();
	glFlush();
}

GLvoid window_reshape(GLsizei width, GLsizei height)
{
	glViewport(0, 0, width, height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-25.0f, 25.0f, -25.0f, 25.0f, -25.0f, 25.0f);

	glMatrixMode(GL_MODELVIEW);
}