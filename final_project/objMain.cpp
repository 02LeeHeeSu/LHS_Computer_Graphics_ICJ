// Reading Obj file
#define _CRT_SECURE_NO_WARNINGS

#include "shader.h"
#include "objRead.h"

using namespace std;


GLuint g_window_w = 800;
GLuint g_window_h = 800;

const int road_number3 = 5;
const int corner_number3 = 4;
const int obj_num3 = 1 + road_number3 * 3 + corner_number3;

const int obj_num4 = 50;

objRead objReader;
GLint s1 = objReader.loadObj_normalize_center("cube.obj");

unsigned int stage_number = 4;

GLuint VAO_3[obj_num3];
GLuint VAO_4[obj_num4];
GLuint VBO[2];

int polygon_mode = 1;

const int num_vertices = 3;
const int num_triangles = 1;

float road_length = 10.f;
float corner_length = 0.6f;

float dx = 0.f, dz = 0.f;

glm::mat4 model{ 1.f }, view{ 1.f }, proj{ 1.f };

void Display();
void Reshape(int w, int h);
void Keyboard(unsigned char key, int x, int y);
void InitBuffer();

void define_color_stage3();
void Stage3();
void Stage4();

GLfloat color_stage3[5][12][3][3]{};
GLfloat color_stage4[2][12][3][3];

GLfloat character_color[3] = { 0.f, 1.f, 0.f };
GLfloat road_color[3][3] = { {1.f, 0.f, 0.f}, {1.f, 0.8f, 0.f}, {1.f, 1.f, 0.f} };
GLfloat corner_color[3] = { 1.f, 1.f, 1.f };

glm::vec3 cameraPos = glm::vec3(0.0f, 1.0f, 1.4f);
glm::vec3 cameraDirection = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

void define_color_stage3()
{
	for (int a = 0; a < 5; a++) {
		for (int i = 0; i < 12; i++)
			for (int j = 0; j < 3; j++)
				for (int k = 0; k < 3; k++) {
					if (a == 0) {
						if (k != 1)
							color_stage3[a][i][j][k] = 0.0f;
						else
							color_stage3[a][i][j][k] = 1.f;
					}
					else if (a == 1) {
						if (k == 0)
							color_stage3[a][i][j][k] = 1.f;
						else
							color_stage3[a][i][j][k] = 0.f;
					}
					else if (a == 2) {
						if (k == 0)
							color_stage3[a][i][j][k] = 1.f;
						else if (k == 1)
							color_stage3[a][i][j][k] = 0.8f;
						else
							color_stage3[a][i][j][k] = 0.f;
					}
					else if (a == 3) {
						if (k != 2)
							color_stage3[a][i][j][k] = 1.f;
						else
							color_stage3[a][i][j][k] = 0.f;
					}
					else if (a == 4) {
						color_stage3[a][i][j][k] = 1.f;
					}
				}
	}
}
void define_color_stage4() {
	for (int a = 0; a < 2; a++) {
		for (int i = 0; i < 12; i++)
			for (int j = 0; j < 3; j++)
				for (int k = 0; k < 3; k++) {
					if (a == 0) {
						if (k != 2)
							color_stage4[a][i][j][k] = 0.0f;
						else
							color_stage4[a][i][j][k] = 0.8f;
					}
					else if (a == 1) {
						if (i < 4) {
							if (j == 0) {
								if (k == 0)
									color_stage4[a][i][j][k] = 0.8f;
								else if (k == 1)
									color_stage4[a][i][j][k] = 0.0f;
								else if (k == 2)
									color_stage4[a][i][j][k] = 0.0f;
							}
						}
						else if (i >= 4 && i < 8) {
							if (j == 1) {
								if (k == 0)
									color_stage4[a][i][j][k] = 0.9f;
								else if (k == 1)
									color_stage4[a][i][j][k] = 0.0f;
								else if (k == 2)
									color_stage4[a][i][j][k] = 0.0f;
							}
						}
						else if (i >= 8 && i < 12) {
							if (j == 0) {
								if (k == 0)
									color_stage4[a][i][j][k] = 1.0f;
								else if (k == 1)
									color_stage4[a][i][j][k] = 0.0f;
								else if (k == 2)
									color_stage4[a][i][j][k] = 0.0f;
							}
						}
					}
				}
	}
}

void Hero() {
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

	model = glm::scale(model, glm::vec3(0.2f, 0.3f, 0.2f));

	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));

	
}
void Stage3()
{
	// 5 + 4 + 1
	Hero();
	glBindVertexArray(VAO_3[0]);
	glDrawArrays(GL_TRIANGLES, 0, s1);
	for (int objnumber = 1; objnumber < obj_num3; objnumber++) {
		model = glm::mat4(1.0f);
		view = glm::mat4(1.0f);
		proj = glm::mat4(1.0f);

		glUseProgram(s_program[0]);

		unsigned int modelLocation = glGetUniformLocation(s_program[0], "m");
		unsigned int viewLocation = glGetUniformLocation(s_program[0], "v");
		unsigned int projectionLocation = glGetUniformLocation(s_program[0], "p");

		proj = glm::perspective(glm::radians(45.0f), 1.f, 0.1f, 100.0f);
		proj = glm::translate(proj, glm::vec3(0.0, 0.0, -2.0));

		glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, &proj[0][0]);

		view = glm::lookAt(cameraPos, cameraDirection, cameraUp);

		glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &view[0][0]);

		if (objnumber >= 1 && objnumber <= road_number3 * 3) {
			switch ((objnumber - 1) / 3) {
				// 세로길 폭 조절
			case 0:
			case 1:
			case 3: {
				if (objnumber % 3 == 0) {
					model = glm::translate(model, glm::vec3(-0.4f, -0.4f, -road_length));
				}

				else if (objnumber % 3 == 1) {
					model = glm::translate(model, glm::vec3(0.f, -0.4f, -road_length));
				}

				else {
					model = glm::translate(model, glm::vec3(0.4f, -0.4f, -road_length));
				}

				break;
			}
				  // 가로길 폭 조절
			case 2:
			case 4: {
				if (objnumber % 3 == 0) {
					model = glm::translate(model, glm::vec3(road_length, -0.4f, 0.4f));
				}

				else if (objnumber % 3 == 1) {
					model = glm::translate(model, glm::vec3(road_length, -0.4f, 0.f));
				}

				else {
					model = glm::translate(model, glm::vec3(road_length, -0.4f, -0.4f));
				}

				break;
			}
			}

			// 위치에 맞게 길 놓기
			switch ((objnumber - 1) / 3) {
			case 0: {
				break;
			}
			case 1: {
				model = glm::translate(model, glm::vec3(0.f, 0.f, -2 * (road_length + corner_length)));

				break;
			}
			case 2: {
				if (objnumber % 3 == 0) {
					model = glm::translate(model, glm::vec3(0.f, 0.f, 0.8f));
				}

				else if (objnumber % 3 == 1) {
					model = glm::translate(model, glm::vec3(0.f, 0.f, -0.8f));
				}

				model = glm::translate(model, glm::vec3(corner_length, 0.f, -4 * road_length - 3 * corner_length));

				break;
			}
			case 3: {
				model = glm::translate(model, glm::vec3(0.f, 0.f, -2 * (road_length + corner_length)));
				model = glm::translate(model, glm::vec3(2 * (road_length + corner_length), 0.f, 0.f));

				if (objnumber % 3 == 0) {
					model = glm::translate(model, glm::vec3(-0.8f, 0.f, 0.f));
				}

				else if (objnumber % 3 == 1) {
					model = glm::translate(model, glm::vec3(0.8f, 0.f, 0.f));
				}

				break;
			}
			case 4: {
				model = glm::translate(model, glm::vec3(corner_length, 0.f, -2 * road_length - corner_length));
				break;
			}
			}
		}

		else if (objnumber) {
			model = glm::translate(model, glm::vec3(0.f, -0.4f, -2 * road_length - corner_length));

			// 위치에 맞게 코너 놓기
			switch (objnumber - road_number3 * 3) {
			case 1: {
				break;
			}
			case 2: {
				model = glm::translate(model, glm::vec3(0.f, 0.f, -2 * (road_length + corner_length)));

				break;
			}
			case 3: {
				model = glm::translate(model, glm::vec3(0.f, 0.f, -2 * (road_length + corner_length)));
				model = glm::translate(model, glm::vec3(2 * (road_length + corner_length), 0.f, 0.f));

				break;
			}
			case 4: {
				model = glm::translate(model, glm::vec3(2 * (road_length + corner_length), 0.f, 0.f));

				break;
			}
			}
		}


		// 사이즈 조절
		if (objnumber == 0) {
			model = glm::scale(model, glm::vec3(0.2f, 0.3f, 0.2f));
		}

		else if (objnumber >= 1 && objnumber <= road_number3 * 3) {
			switch ((objnumber - 1) / 3) {
				// 세로길
			case 0:
			case 1:
			case 3: {
				model = glm::scale(model, glm::vec3(0.2f, 0.1f, road_length));

				break;
			}
				  // 가로길
			case 2:
			case 4: {
				model = glm::scale(model, glm::vec3(road_length, 0.1f, 0.2f));

				break;
			}
			}
		}

		else {
			model = glm::scale(model, glm::vec3(corner_length, 0.1f, corner_length));
		}

		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));

		glBindVertexArray(VAO_3[objnumber]);
		glDrawArrays(GL_TRIANGLES, 0, s1);
	}
}
void Stage4() {
	// 49 + 1
	Hero();
	glBindVertexArray(VAO_4[0]);
	glDrawArrays(GL_TRIANGLES, 0, s1);
	GLfloat stage4_dir_x = 1, stage4_dir_z = 1;
	for (int i = 1; i <= sqrt(obj_num4); i++) {
		if (i > 4)
			stage4_dir_x = -1;
		else
			stage4_dir_x = 1;
		for (int k = 1; k <= sqrt(obj_num4); k++) {
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

			if (k > 4)
				stage4_dir_z = -1;
			else
				stage4_dir_z = 1;

			GLfloat size_stage4 = 1.4f;

			model = glm::translate(model, glm::vec3(size_stage4 * float(i % 4) * stage4_dir_x, -0.4f, size_stage4 * float(k % 4) * stage4_dir_z));

			model = glm::scale(model, glm::vec3(size_stage4 / 2, 0.1f, size_stage4 / 2));

			glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));

			if (i == 4 && k == 4) {
				glBindVertexArray(VAO_4[k + 7 * (i - 1)]);
				glDrawArrays(GL_TRIANGLES, 0, s1);
			}
			else if ((i % 2 == 1)
				|| (k % 2 == 1)) {
				glBindVertexArray(VAO_4[k + 7 * (i - 1)]);
				glDrawArrays(GL_TRIANGLES, 0, s1);
			}
		}
	}
}

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

	vShader[0] = MakeVertexShader("vertex.glsl", 0);
	fShader[0] = MakeFragmentShader("fragment.glsl", 0);

	s_program[0] = glCreateProgram();
	glAttachShader(s_program[0], vShader[0]);
	glAttachShader(s_program[0], fShader[0]);
	glLinkProgram(s_program[0]);
	checkCompileErrors(s_program[0], "PROGRAM");

	define_color_stage3();
	define_color_stage4();

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
		for (int i = 0; i < obj_num3; i++) {
			glGenVertexArrays(1, &VAO_3[i]);
			glGenBuffers(2, VBO);

			glUseProgram(s_program[0]);
			glBindVertexArray(VAO_3[i]);
			glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
			glBufferData(GL_ARRAY_BUFFER, objReader.outvertex.size() * sizeof(glm::vec3), &objReader.outvertex[0], GL_STATIC_DRAW);
			GLint pAttribute = glGetAttribLocation(s_program[0], "aPos");
			glVertexAttribPointer(pAttribute, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
			glEnableVertexAttribArray(pAttribute);

			glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
			if (i == 0)
				glBufferData(GL_ARRAY_BUFFER, objReader.outnormal.size() * sizeof(glm::vec3), color_stage3[0], GL_STATIC_DRAW);
			else if (i >= 1 && i <= 3 * road_number3)
				glBufferData(GL_ARRAY_BUFFER, objReader.outnormal.size() * sizeof(glm::vec3), color_stage3[i % 3 + 1], GL_STATIC_DRAW);
			else
				glBufferData(GL_ARRAY_BUFFER, objReader.outnormal.size() * sizeof(glm::vec3), color_stage3[4], GL_STATIC_DRAW);
			GLint nAttribute = glGetAttribLocation(s_program[0], "in_Color");
			glVertexAttribPointer(nAttribute, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
			glEnableVertexAttribArray(nAttribute);
		}

			for (int i = 0; i < obj_num4; i++) {
				glGenVertexArrays(1, &VAO_4[i]);
				glGenBuffers(2, VBO);

				glUseProgram(s_program[0]);
				glBindVertexArray(VAO_4[i]);
				glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
				glBufferData(GL_ARRAY_BUFFER, objReader.outvertex.size() * sizeof(glm::vec3), &objReader.outvertex[0], GL_STATIC_DRAW);
				GLint pAttribute = glGetAttribLocation(s_program[0], "aPos");
				glVertexAttribPointer(pAttribute, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
				glEnableVertexAttribArray(pAttribute);

				glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
				if (i == 0)
					glBufferData(GL_ARRAY_BUFFER, objReader.outnormal.size() * sizeof(glm::vec3), color_stage4[0], GL_STATIC_DRAW);
				else
					glBufferData(GL_ARRAY_BUFFER, objReader.outnormal.size() * sizeof(glm::vec3), color_stage4[1], GL_STATIC_DRAW);
				GLint nAttribute = glGetAttribLocation(s_program[0], "in_Color");
				glVertexAttribPointer(nAttribute, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
				glEnableVertexAttribArray(nAttribute);
			}

	glEnable(GL_DEPTH_TEST);
}

void Display()
{
	glClearColor(0.0f, 0.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (polygon_mode == 1)
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	else if (polygon_mode == 2)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	if(stage_number == 3)
		Stage3();
	else if (stage_number == 4)
		Stage4();
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
	switch (key) {
	case '3':
		stage_number = 3;
		break;
	case '4':
		stage_number = 4;
		break;
		{
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
		}
	case 'q': case 'Q':
		for (int i = 0; i < obj_num3; i++) {
			glDeleteVertexArrays(1, &VAO_3[i]);
			glDeleteBuffers(2, VBO);
		}
		for (int i = 0; i < obj_num4; i++) {
			glDeleteVertexArrays(1, &VAO_4[i]);
			glDeleteBuffers(2, VBO);
		}

		glutLeaveMainLoop();

		break;
	}

	glutPostRedisplay();
}
