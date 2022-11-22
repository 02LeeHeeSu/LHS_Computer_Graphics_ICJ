// Reading Obj file
#define  _CRT_SECURE_NO_WARNINGS

#include "shader.h"
#include "objRead.h"

using namespace std;


GLuint g_window_w = 600;
GLuint g_window_h = 600;

const int obj_num = 5;
GLuint VAO[obj_num];
GLuint VBO[2];
int polygon_mode = 2;

const int num_vertices = 3;
const int num_triangles = 1;

glm::mat4 model, view, proj;

void Display();
void Reshape(int w, int h);
void Keyboard(unsigned char key, int x, int y);
void InitBuffer();
GLfloat colors[obj_num][12][3][3];

glm::vec3 cameraPos = glm::vec3(0.0f, 1.0f, -1.4f);
glm::vec3 cameraDirection = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);


objRead objReader;
GLint s1[5] = {
	objReader.loadObj_normalize_center("cube.obj"),
	objReader.loadObj_normalize_center("cube.obj"),
	objReader.loadObj_normalize_center("cube.obj"),
	objReader.loadObj_normalize_center("cube.obj"),
	objReader.loadObj_normalize_center("cube.obj")
};
int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(g_window_w, g_window_h);
	glutInitWindowPosition(0, 0);

	glutCreateWindow("Computer Graphics");

	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
	{
		std::cerr << "Unable to initialize GLEW ... exiting" << std::endl;
		exit(EXIT_FAILURE);
	}
	else
	{
		std::cout << "GLEW OK\n";
	}

	GLuint vShader[4];
	GLuint fShader[4];

	vShader[0]= MakeVertexShader("vertex.glsl", 0);
	fShader[0] = MakeFragmentShader("fragment.glsl", 0);

	s_program[0] = glCreateProgram();
	glAttachShader(s_program[0], vShader[0]);
	glAttachShader(s_program[0], fShader[0]);
	glLinkProgram(s_program[0]);
	checkCompileErrors(s_program[0], "PROGRAM");

	for (int a = 0; a < obj_num; a++) {
		for (int i = 0; i < 12; i++) {
			for (int j = 0; j < 3; j++)
				for (int k = 0; k < 3; k++) {
					if (a == 0) {
						if (k != 2)
							colors[a][i][j][k] = 0.0f;
						else
							colors[a][i][j][k] = 1.0f;
					}
					else if (a == 1) {
						if (k != 1)
							colors[a][i][j][k] = 0.0f;
						else
							colors[a][i][j][k] = 1.0f;
					}
					else if (a == 2) {
						if (k == 0)
							colors[a][i][j][k] = 1.0f;
						else if (k == 1)
							colors[a][i][j][k] = 0.8f;
						else
							colors[a][i][j][k] = 0.0f;
					}
					else if (a == 3) {
						if (k == 0)
							colors[a][i][j][k] = 1.0f;
						else
							colors[a][i][j][k] = 0.0f;
					}
					else if (a == 4) {
						if (k == 0)
							colors[a][i][j][k] = 0.0f;
						else
							colors[a][i][j][k] = 1.0f;
					}
				}
		}
	}

	InitBuffer();

	glutDisplayFunc(Display);
	glutReshapeFunc(Reshape);
	glutKeyboardFunc(Keyboard);

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glutMainLoop();

	return 0;
}
void InitBuffer()
{
	for (int i = 0; i < obj_num; i++) {
		glGenVertexArrays(1, &VAO[i]);
		glGenBuffers(2, VBO);

		glUseProgram(s_program[0]);
		glBindVertexArray(VAO[i]);
		glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
		glBufferData(GL_ARRAY_BUFFER, objReader.outvertex.size() * sizeof(glm::vec3), &objReader.outvertex[0], GL_STATIC_DRAW);
		GLint pAttribute = glGetAttribLocation(s_program[0], "aPos");
		glVertexAttribPointer(pAttribute, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
		glEnableVertexAttribArray(pAttribute);

		glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
		glBufferData(GL_ARRAY_BUFFER, objReader.outnormal.size() * sizeof(glm::vec3), colors[i], GL_STATIC_DRAW);
		GLint nAttribute = glGetAttribLocation(s_program[0], "in_Color");
		glVertexAttribPointer(nAttribute, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
		glEnableVertexAttribArray(nAttribute);
	}
	glEnable(GL_DEPTH_TEST);
}
void Display()
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (polygon_mode == 1)
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	else if (polygon_mode == 2)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	for (int i = 0; i < obj_num; i++) {
		model = glm::mat4(1.0f);
		view = glm::mat4(1.0f);
		proj = glm::mat4(1.0f);

		unsigned int modelLocation = glGetUniformLocation(s_program[0], "m");
		unsigned int viewLocation = glGetUniformLocation(s_program[0], "v");
		unsigned int projectionLocation = glGetUniformLocation(s_program[0], "p");

		glUseProgram(s_program[0]);

		proj = glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 100.0f);
		proj = glm::translate(proj, glm::vec3(0.0, 0.0, -2.0));

		glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, &proj[0][0]);

		view = glm::lookAt(cameraPos, cameraDirection, cameraUp);

		glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &view[0][0]);

		if (i == 1)
			model = glm::translate(model, glm::vec3(0.0f, 0.3f, 0.0f));
		else if (i == 2)
			model = glm::translate(model, glm::vec3(-0.4f, 0.05f, 0.0f));
		else if (i == 3)
			model = glm::translate(model, glm::vec3(0.0f, 0.05f, 0.0f));
		else if (i == 4)
			model = glm::translate(model, glm::vec3(0.4f, 0.05f, 0.0f));

		if (i == 0)
			model = glm::scale(model, glm::vec3(30.0f, 30.0f, 30.0f));
		else if (i == 1)
			model = glm::scale(model, glm::vec3(0.2f, 0.3f, 0.2f));
		else
			model = glm::scale(model, glm::vec3(0.2f, 0.1f, 20.0f));

		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));

		glBindVertexArray(VAO[i]);
		glDrawArrays(GL_TRIANGLES, 0, s1[i]);
	}
	glutSwapBuffers();
}
void Reshape(int w, int h)
{
	g_window_w = w;
	g_window_h = h;
	glViewport(0, 0, w, h);
}
void Keyboard(unsigned char key, int x, int y)
{
	if (key == '1')
		polygon_mode = 1;
	else if (key == '2')
		polygon_mode = 2;
	switch (key) {
	case 'x': 
		cameraPos.x += 0.1f;
		break;
	case 'X':
		cameraPos.x -= 0.1f;
		break;
	case 'y':
		cameraPos.y += 0.1f;
		break;
	case 'Y':
		cameraPos.y -= 0.1f;
		break;
	case 'z':
		cameraPos.z += 0.1f;
		break;
	case 'Z':
		cameraPos.z -= 0.1f;
		break;
	case 'e':
		printf("%f %f %f", cameraPos.x, cameraPos.y, cameraPos.z);
		break;
	case 'q': case 'Q':
		for (int i = 0; i < obj_num; i++) {
			glDeleteVertexArrays(1, &VAO[i]);
			glDeleteBuffers(2, VBO);
		}
		exit(0);
		break;
	}

	glutPostRedisplay();
}
