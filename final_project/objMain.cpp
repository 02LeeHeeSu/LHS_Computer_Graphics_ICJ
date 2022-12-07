// Reading Obj file
#define _CRT_SECURE_NO_WARNINGS

#include "shader.h"
#include "objRead.h"

using namespace std;

GLuint g_window_w = 800;
GLuint g_window_h = 800;

int polygon_mode = 1;

const int num_vertices = 3;
const int num_triangles = 1;

objRead objReader, obr;
GLint s1 = objReader.loadObj_normalize_center("cube.obj");
GLint s2 = obr.loadObj_normalize_center("sphere.obj");

GLuint VAO_portal;
GLuint VAO_block[3];	// 0: 점프, 1: 벽, 2: 슬라이딩
GLuint VAO_enemy[3];
GLuint VAO_0[obj_num0];
GLuint VAO_1[obj_num1];
GLuint VAO_2[obj_num2];
GLuint VAO_3[obj_num3];
GLuint VAO_4[obj_num4];
GLuint VBO[2];

glm::mat4 model{ 1.f }, view{ 1.f }, proj{ 1.f };

GLfloat color_portal[1124][3][3];
GLfloat color_enemy[1][1124][3][3];
GLfloat color_stage_0_to_3[5][12][3][3]{};
GLfloat color_stage4[2][12][3][3];

unsigned int stage_number = 4;

const int road_number2 = 3;
const int corner_number2 = 2;

const int road_number3 = 5;
const int corner_number3 = 4;

const int obj_num0 = 4;
const int obj_num1 = 4;
const int obj_num2 = 1 + road_number2 * 3 + corner_number2;
const int obj_num3 = 1 + road_number3 * 3 + corner_number3;
const int obj_num4 = 50;

glm::vec3 cameraPos = glm::vec3(0.0f, 1.0f, 1.4f);
glm::vec3 cameraDirection = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

float road_length = 10.f;
float corner_length = 0.6f;
float dx = 0.f, dz = 0.f;

GLfloat move_object_x = 0.0f, move_object_y = -0.23f, move_object_z = 0.0f;
GLfloat object_speed = 0.05f, average_speed = 0.05f, great_speed = 0.2f;
GLfloat sight_x = 0.0f, sight_y = 0.005f, sight_z = 0.01f;
GLfloat object_scale_y = 0.07f;
GLfloat object_jump_speed = object_speed / 2.0f;
GLfloat object_slide_speed = 0.01f;
BOOL JUMP = false, SLIDE = false;
GLfloat object_jump_dir = 1.0f, object_slide_dir = 1.0f;

void define_color_portal();
void define_color_enemy();
void define_color_stage_0_to_3();
void define_color_stage4();
void check_load_collide();
void Reset_Position();
void Enemy();
void Hero();
void Stage0();
void Stage1();
void Stage2();
void Stage3();
void Stage4();
void Block_jump();
void Block_wall();
void Block_slide();
void Portal();
void InitBuffer();
void Display();
void Reshape(int w, int h);
void Keyboard(unsigned char key, int x, int y);
void TimerFunction(int value);

void define_color_portal()
{
	for (int i = 0; i < 1124; i++) {
		switch (i % 3) {
		case 0: {
			for (int j = 0; j < 3; j++) {
				for (int k = 0; k < 3; k++) {
					switch (k) {
					case 0: {
						color_portal[i][j][k] = 0.6f;
						break;
					}
					case 1: {
						color_portal[i][j][k] = 1.f;
						break;
					}
					case 2: {
						color_portal[i][j][k] = 1.f;
						break;
					}
					}
				}
			}

			break;
		}
		case 1: {
			for (int j = 0; j < 3; j++) {
				for (int k = 0; k < 3; k++) {
					switch (k) {
					case 0: {
						color_portal[i][j][k] = 0.2f;
						break;
					}
					case 1: {
						color_portal[i][j][k] = 1.f;
						break;
					}
					case 2: {
						color_portal[i][j][k] = 0.8f;
						break;
					}
					}
				}
			}

			break;
		}
		case 2: {
			for (int j = 0; j < 3; j++) {
				for (int k = 0; k < 3; k++) {
					switch (k) {
					case 0: {
						color_portal[i][j][k] = 0.2f;
						break;
					}
					case 1: {
						color_portal[i][j][k] = 0.8f;
						break;
					}
					case 2: {
						color_portal[i][j][k] = 1.f;
						break;
					}
					}
				}
			}

			break;
		}
		}
	}
}
void define_color_enemy()
{
	for (int i = 0; i < 1124; i++)
		for (int j = 0; j < 3; j++)
			for (int k = 0; k < 3; k++) {
				color_enemy[0][i][j][k] = 0.5f;
			}
}
void define_color_stage_0_to_3()
{
	for (int a = 0; a < 5; a++) {
		for (int i = 0; i < 12; i++)
			for (int j = 0; j < 3; j++)
				for (int k = 0; k < 3; k++) {
					if (a == 0) {
						if (k != 1)
							color_stage_0_to_3[a][i][j][k] = 0.0f;
						else
							color_stage_0_to_3[a][i][j][k] = 1.f;
					}
					else if (a == 1) {
						if (k == 0)
							color_stage_0_to_3[a][i][j][k] = 1.f;
						else
							color_stage_0_to_3[a][i][j][k] = 0.f;
					}
					else if (a == 2) {
						if (k == 0)
							color_stage_0_to_3[a][i][j][k] = 1.f;
						else if (k == 1)
							color_stage_0_to_3[a][i][j][k] = 0.8f;
						else
							color_stage_0_to_3[a][i][j][k] = 0.f;
					}
					else if (a == 3) {
						if (k != 2)
							color_stage_0_to_3[a][i][j][k] = 1.f;
						else
							color_stage_0_to_3[a][i][j][k] = 0.f;
					}
					else if (a == 4) {
						if (k == 2)
							color_stage_0_to_3[a][i][j][k] = 1.0f;
						else if (k == 1)
							color_stage_0_to_3[a][i][j][k] = 0.6f;
						else 
							color_stage_0_to_3[a][i][j][k] = 1.f;
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
void check_load_collide() {
	switch (stage_number) {
	case 0: case 1:
		if (move_object_x > 0.4f)
			move_object_x = 0.4f;
		else if (move_object_x < -0.4f)
			move_object_x = -0.4f;
		if (move_object_z > 0.0f)
			move_object_z = 0.0f;
		else if (move_object_z < -20.0f + 0.025f)
			move_object_z = -20.0f + 0.025f;
		break;
	case 2:
		if (move_object_x > -0.4f && move_object_x <= 0.4f) {
			if (move_object_z > 0.0f)
				move_object_z = 0.0f;
			else if (move_object_z < -21.2f + 0.025f)
				move_object_z = -21.2f + 0.025f;
		}
		else if (move_object_x < -0.4f && move_object_x >= -20.0f - 0.4f) {
			if (move_object_z >= -20.0f + 0.025f)
				move_object_z -= object_speed;
			else if (move_object_z < -21.2f + 0.025f)
				move_object_z += object_speed;
		}
		else if (move_object_x < -20.0f - 0.4f && move_object_x >= -21.2f - 0.4f) {
			if (move_object_z >= -20.0f + 0.025f)
				move_object_z -= object_speed;
			else if (move_object_z < -41.2f + 0.025f)
				move_object_z = -41.2f + 0.025f;
		}

		if (move_object_z <= 0.0f && move_object_z > -20.0f + 0.025f) {
			if (move_object_x > 0.4f)
				move_object_x -= object_speed;
			else if (move_object_x < -0.4f) {
				move_object_x += object_speed;
				move_object_z += object_speed;
			}
		}
		else if (move_object_z <= -20.0f + 0.025f && move_object_z >= -21.2f + 0.025f) {
			if (move_object_x > 0.4f)
				move_object_x -= object_speed;
			else if (move_object_x < -21.2f - 0.4f)
				move_object_x += object_speed;
		}
		else if (move_object_z < -21.2f + 0.025f && move_object_z >= -41.2f + 0.025f) {
			if (move_object_x > -20.0f - 0.4f) {
				move_object_x -= object_speed;
				move_object_z -= object_speed;
			}
			else if (move_object_x <= -21.2f - 0.4f)
				move_object_x += object_speed;
		}

		break;
	case 4:
		if (move_object_x > 4.7f)
			move_object_x = 4.7f;
		else if (move_object_x < -4.7f)
			move_object_x = -4.7;
		if (move_object_z > 4.7f)
			move_object_z = 4.7f;
		else if (move_object_z < -4.7f)
			move_object_z = -4.7;
		break;
	}
}
void Reset_Position() {
	move_object_x = 0.0f, move_object_z = 0.0f;
}
void Enemy()
{
	for (int i = 0; i < 3; i++) {
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
		switch (i)
		{
		case 0:
			model = glm::translate(model, glm::vec3(-0.4f, -0.23f, 0.4f));
			break;
		case 1:
			model = glm::translate(model, glm::vec3(0.f, -0.23f, 0.4f));
			break;
		case 2:
			model = glm::translate(model, glm::vec3(0.4f, -0.23f, 0.4f));
			break;
		default:
			break;
		}

		model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));

		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));
		glBindVertexArray(VAO_enemy[i]);
		glDrawArrays(GL_TRIANGLES, 0, s2);
	}
}
void Hero()
{
	check_load_collide();

	cameraPos.x = move_object_x;
	cameraPos.y = sight_y;
	cameraPos.z = move_object_z + sight_z;

	cameraDirection.x = move_object_x;
	cameraDirection.z = move_object_z;

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

	model = glm::translate(model, glm::vec3(move_object_x, -0.23f, move_object_z));
	model = glm::scale(model, glm::vec3(0.2f, 0.07f, 0.05f));

	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));
}
void Stage0()
{
	// 3 + 0 + 1
	Hero();
	glBindVertexArray(VAO_0[0]);
	glDrawArrays(GL_TRIANGLES, 0, s1);

	Block_jump();
	Block_wall();
	Block_slide();
	Portal();

	for (int objnumber = 1; objnumber < obj_num0; objnumber++) {
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

		if (objnumber == 1) {
			model = glm::translate(model, glm::vec3(-0.4f, -0.4f, -road_length));
		}

		else if (objnumber == 2) {
			model = glm::translate(model, glm::vec3(0.f, -0.4f, -road_length));
		}

		else {
			model = glm::translate(model, glm::vec3(0.4f, -0.4f, -road_length));
		}

		// 사이즈 조절
		if (objnumber >= 1 && objnumber <= 3) {
			model = glm::scale(model, glm::vec3(0.2f, 0.1f, road_length));
		}

		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));

		glBindVertexArray(VAO_0[objnumber]);
		glDrawArrays(GL_TRIANGLES, 0, s1);
	}
}
void Stage1()
{
	// 3 + 0 + 1
	Hero();
	glBindVertexArray(VAO_1[0]);
	glDrawArrays(GL_TRIANGLES, 0, s1);

	Block_jump();
	Block_wall();
	Block_slide();
	Portal();

	for (int objnumber = 1; objnumber < obj_num1; objnumber++) {
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

		if (objnumber == 1) {
			model = glm::translate(model, glm::vec3(-0.4f, -0.4f, -road_length));
		}

		else if (objnumber  == 2) {
			model = glm::translate(model, glm::vec3(0.f, -0.4f, -road_length));
		}

		else {
			model = glm::translate(model, glm::vec3(0.4f, -0.4f, -road_length));
		}

		// 사이즈 조절
		if (objnumber >= 1 && objnumber <= 3) {
			model = glm::scale(model, glm::vec3(0.2f, 0.1f, road_length));
		}

		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));

		glBindVertexArray(VAO_1[objnumber]);
		glDrawArrays(GL_TRIANGLES, 0, s1);
	}
}
void Stage2()
{
	// 9 + 2 + 1
	Hero();
	glBindVertexArray(VAO_2[0]);
	glDrawArrays(GL_TRIANGLES, 0, s1);

	Block_jump();
	Block_wall();
	Block_slide();
	Portal();

	for (int objnumber = 1; objnumber < obj_num2; objnumber++) {
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

		if (objnumber >= 1 && objnumber <= road_number2 * 3) {
			switch ((objnumber - 1) / 3) {
				// 세로길 폭 조절
			case 0:
			case 2: {
				if (objnumber % 3 == 1) {
					model = glm::translate(model, glm::vec3(-0.4f, -0.4f, -road_length));
				}

				else if (objnumber % 3 == 2) {
					model = glm::translate(model, glm::vec3(0.f, -0.4f, -road_length));
				}

				else {
					model = glm::translate(model, glm::vec3(0.4f, -0.4f, -road_length));
				}

				break;
			}
				  // 가로길 폭 조절
			case 1: {
				if (objnumber % 3 == 1) {
					model = glm::translate(model, glm::vec3(-road_length, -0.4f, 0.4f));
				}

				else if (objnumber % 3 == 2) {
					model = glm::translate(model, glm::vec3(-road_length, -0.4f, 0.f));
				}

				else {
					model = glm::translate(model, glm::vec3(-road_length, -0.4f, -0.4f));
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
				model = glm::translate(model, glm::vec3(-corner_length, 0.f, -2 * road_length - corner_length));
				break;
			}
			case 2: {
				model = glm::translate(model, glm::vec3(-2 * (road_length + corner_length), 0.f, -2 * (road_length + corner_length)));

				break;
			}
			}
		}

		else if (objnumber == 10 || objnumber== 11) {
			GLfloat a = 0.0f;

			// 위치에 맞게 코너 놓기
			switch (objnumber - road_number2 * 3) {
			case 1: {
				model = glm::translate(model, glm::vec3(a, -0.4f, -2 * road_length - corner_length));
				break;
			}
			case 2: {
				a = -2 * (road_length + corner_length);
				model = glm::translate(model, glm::vec3(a, -0.4f, -2 * road_length - corner_length));
				break;
			}
			}
		}

		// 사이즈 조절
		if (objnumber >= 1 && objnumber <= road_number2 * 3) {
			switch ((objnumber - 1) / 3) {
				// 세로길1
			case 0: {
				model = glm::scale(model, glm::vec3(0.2f, 0.1f, road_length));

				break;
			}
				  // 가로길2
			case 1: {
				model = glm::scale(model, glm::vec3(road_length, 0.1f, 0.2f));

				break;
			}
				  //세로길3
			case 2: {
				model = glm::scale(model, glm::vec3(0.2f, 0.1f, road_length));

				break;
			}
				  
			}
		}
		else {
			model = glm::scale(model, glm::vec3(corner_length, 0.1f, corner_length));
		}

		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));

		glBindVertexArray(VAO_2[objnumber]);
		glDrawArrays(GL_TRIANGLES, 0, s1);
	}
}
void Stage3()
{
	// 5 * 3 + 4 + 1 = 15 + 4 + 1
	Hero();
	glBindVertexArray(VAO_3[0]);
	glDrawArrays(GL_TRIANGLES, 0, s1);

	Block_jump();
	Block_wall();
	Block_slide();
	Portal();

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
				if (objnumber % 3 == 1) {
					model = glm::translate(model, glm::vec3(-0.4f, -0.4f, -road_length));
				}

				else if (objnumber % 3 == 2) {
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
				if (objnumber % 3 == 1) {
					model = glm::translate(model, glm::vec3(road_length, -0.4f, 0.4f));
				}

				else if (objnumber % 3 == 2) {
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

	Block_jump();
	Block_wall();
	Block_slide();
	Portal();

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
void Block_jump()
{
	model = glm::mat4(1.0f);
	view = glm::mat4(1.0f);
	proj = glm::mat4(1.0f);

	glUseProgram(s_program[0]);

	unsigned int modelLocation = glGetUniformLocation(s_program[0], "m");
	unsigned int viewLocation = glGetUniformLocation(s_program[0], "v");
	unsigned int projectionLocation = glGetUniformLocation(s_program[0], "p");

	proj = glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 100.0f);
	proj = glm::translate(proj, glm::vec3(0.0, 0.0, -2.0));

	glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, &proj[0][0]);

	view = glm::lookAt(cameraPos, cameraDirection, cameraUp);

	glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &view[0][0]);

	switch (stage_number) {
	case 0: {
		model = glm::translate(model, glm::vec3(0.f, -0.16f, -6.6f - 0.6f));
		model = glm::scale(model, glm::vec3(0.6f, 0.14f, 0.6f));

		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));

		glBindVertexArray(VAO_block[0]);
		glDrawArrays(GL_TRIANGLES, 0, s1);
		break;
	}
	case 1: {
		break;
	}
	case 2: {
		for (int i = 0; i < 2; i++) {
			model = glm::mat4(1.0f);

			switch (i) {
			case 0: {
				model = glm::translate(model, glm::vec3(13.3f - 0.6f - 20.6f, -0.16f, -20.6f));
				model = glm::rotate(model, glm::radians(90.f), glm::vec3(0.f, 1.f, 0.f));
				model = glm::scale(model, glm::vec3(0.6f, 0.14f, 0.6f));

				glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, &proj[0][0]);
				glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &view[0][0]);
				glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));

				glBindVertexArray(VAO_block[0]);
				glDrawArrays(GL_TRIANGLES, 0, s1);

				break;
			}

			case 1: {
				model = glm::translate(model, glm::vec3(-0.6f - 20.6f, -0.16f, -6.6f - 0.6f - 20.6f));
				model = glm::scale(model, glm::vec3(0.6f, 0.14f, 0.6f));

				glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, &proj[0][0]);
				glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &view[0][0]);
				glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));

				glBindVertexArray(VAO_block[0]);
				glDrawArrays(GL_TRIANGLES, 0, s1);

				break;
			}
			}
		}
		break;
	}
	case 3: {
		break;
	}
	case 4: {
		break;
	}
	}
}
void Block_wall()
{
	model = glm::mat4(1.0f);
	view = glm::mat4(1.0f);
	proj = glm::mat4(1.0f);

	glUseProgram(s_program[0]);

	unsigned int modelLocation = glGetUniformLocation(s_program[0], "m");
	unsigned int viewLocation = glGetUniformLocation(s_program[0], "v");
	unsigned int projectionLocation = glGetUniformLocation(s_program[0], "p");

	proj = glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 100.0f);
	proj = glm::translate(proj, glm::vec3(0.0, 0.0, -2.0));

	glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, &proj[0][0]);

	view = glm::lookAt(cameraPos, cameraDirection, cameraUp);

	glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &view[0][0]);

	switch (stage_number) {
	case 0: {
		break;
	}
	case 1: {
		model = glm::translate(model, glm::vec3(0.2f, 0.05f, -6.6f - 0.6f));
		model = glm::scale(model, glm::vec3(0.4f, 0.35f, 0.6f));

		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));

		glBindVertexArray(VAO_block[1]);
		glDrawArrays(GL_TRIANGLES, 0, s1);

		model = glm::mat4(1.f);
		model = glm::translate(model, glm::vec3(-0.2f, 0.05f, -13.3f - 0.6f));
		model = glm::scale(model, glm::vec3(0.4f, 0.35f, 0.6f));

		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));

		glBindVertexArray(VAO_block[1]);
		glDrawArrays(GL_TRIANGLES, 0, s1);

		break;
	}
	case 2: {
		for (int i = 0; i < 2; i++) {
			model = glm::mat4(1.0f);

			switch (i) {
			case 0: {
				model = glm::translate(model, glm::vec3(0.2f, 0.05f, -13.3f - 0.6f));
				model = glm::scale(model, glm::vec3(0.4f, 0.35f, 0.6f));

				glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, &proj[0][0]);
				glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &view[0][0]);
				glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));

				glBindVertexArray(VAO_block[1]);
				glDrawArrays(GL_TRIANGLES, 0, s1);

				break;
			}

			case 1: {
				model = glm::translate(model, glm::vec3(6.6f - 0.6f - 20.6f, -0.16f, -20.6f));
				model = glm::rotate(model, glm::radians(90.f), glm::vec3(0.f, 1.f, 0.f));
				model = glm::scale(model, glm::vec3(0.4f, 0.35f, 0.6f));

				glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, &proj[0][0]);
				glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &view[0][0]);
				glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));

				glBindVertexArray(VAO_block[1]);
				glDrawArrays(GL_TRIANGLES, 0, s1);

				break;
			}
			}
		}

		break;
	}
	case 3: {
		break;
	}
	case 4: {
		break;
	}
	}
}
void Block_slide()
{
	model = glm::mat4(1.0f);
	view = glm::mat4(1.0f);
	proj = glm::mat4(1.0f);

	glUseProgram(s_program[0]);

	unsigned int modelLocation = glGetUniformLocation(s_program[0], "m");
	unsigned int viewLocation = glGetUniformLocation(s_program[0], "v");
	unsigned int projectionLocation = glGetUniformLocation(s_program[0], "p");

	proj = glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 100.0f);
	proj = glm::translate(proj, glm::vec3(0.0, 0.0, -2.0));

	glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, &proj[0][0]);

	view = glm::lookAt(cameraPos, cameraDirection, cameraUp);

	glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &view[0][0]);

	switch (stage_number) {
	case 0: {
		model = glm::translate(model, glm::vec3(0.f, 0.3f, -13.3f - 0.6f));
		model = glm::scale(model, glm::vec3(0.6f, 0.5f, 0.6f));

		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));

		glBindVertexArray(VAO_block[2]);
		glDrawArrays(GL_TRIANGLES, 0, s1);

		break;
	}
	case 1: {
		break;
	}
	case 2: {
		for (int i = 0; i < 2; i++) {
			model = glm::mat4(1.0f);

			switch (i) {
			case 0: {
				model = glm::translate(model, glm::vec3(0.f, 0.3f, -6.6f - 0.6f));
				model = glm::scale(model, glm::vec3(0.6f, 0.5f, 0.6f));

				glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, &proj[0][0]);
				glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &view[0][0]);
				glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));

				glBindVertexArray(VAO_block[2]);
				glDrawArrays(GL_TRIANGLES, 0, s1);

				break;
			}

			case 1: {
				model = glm::translate(model, glm::vec3(-0.6f - 20.6f, 0.3f, -13.3f - 0.6f - 20.6f));
				model = glm::scale(model, glm::vec3(0.6f, 0.5f, 0.6f));

				glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, &proj[0][0]);
				glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &view[0][0]);
				glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));

				glBindVertexArray(VAO_block[2]);
				glDrawArrays(GL_TRIANGLES, 0, s1);

				break;
			}
			}
		}

		break;
	}
	case 3: {
		break;
	}
	case 4: {
		break;
	}
	}
}
void Portal()
{
	model = glm::mat4(1.0f);
	view = glm::mat4(1.0f);
	proj = glm::mat4(1.0f);

	glUseProgram(s_program[0]);

	unsigned int modelLocation = glGetUniformLocation(s_program[0], "m");
	unsigned int viewLocation = glGetUniformLocation(s_program[0], "v");
	unsigned int projectionLocation = glGetUniformLocation(s_program[0], "p");

	proj = glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 100.0f);
	proj = glm::translate(proj, glm::vec3(0.0, 0.0, -2.0));

	glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, &proj[0][0]);

	view = glm::lookAt(cameraPos, cameraDirection, cameraUp);

	glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &view[0][0]);

	switch (stage_number) {
	case 0: {
		model = glm::translate(model, glm::vec3(0.f, -0.2f, -20.f));
		model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));

		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));

		glBindVertexArray(VAO_portal);
		glDrawArrays(GL_TRIANGLES, 0, s2);
		break;
	}
	case 1: {
		model = glm::translate(model, glm::vec3(0.f, -0.2f, -20.f));
		model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));

		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));

		glBindVertexArray(VAO_portal);
		glDrawArrays(GL_TRIANGLES, 0, s2);

		break;
	}
	case 2: {
		model = glm::translate(model, glm::vec3(-20.6f, -0.2f, -40.f));
		model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));

		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));

		glBindVertexArray(VAO_portal);
		glDrawArrays(GL_TRIANGLES, 0, s2);

		break;
	}
	case 3: {
		break;
	}
	case 4: {
		break;
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

	define_color_portal();
	define_color_enemy();
	define_color_stage_0_to_3();
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
	glGenVertexArrays(1, &VAO_portal);
	glGenBuffers(2, VBO);
	glUseProgram(s_program[0]);
	glBindVertexArray(VAO_portal);

	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, obr.outvertex.size() * sizeof(glm::vec3), &obr.outvertex[0], GL_STATIC_DRAW);
	GLint pAttribute = glGetAttribLocation(s_program[0], "aPos");
	glVertexAttribPointer(pAttribute, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
	glEnableVertexAttribArray(pAttribute);

	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	glBufferData(GL_ARRAY_BUFFER, obr.outnormal.size() * sizeof(glm::vec3), color_portal, GL_STATIC_DRAW);
	GLint nAttribute = glGetAttribLocation(s_program[0], "in_Color");
	glVertexAttribPointer(nAttribute, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
	glEnableVertexAttribArray(nAttribute);

	for (int i = 0; i < 3; i++) {
		glGenVertexArrays(1, &VAO_enemy[i]);
		glGenBuffers(2, VBO);
		glUseProgram(s_program[0]);
		glBindVertexArray(VAO_enemy[i]);

		glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
		glBufferData(GL_ARRAY_BUFFER, obr.outvertex.size() * sizeof(glm::vec3), &obr.outvertex[0], GL_STATIC_DRAW);
		GLint pAttribute = glGetAttribLocation(s_program[0], "aPos");
		glVertexAttribPointer(pAttribute, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
		glEnableVertexAttribArray(pAttribute);

		glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
		glBufferData(GL_ARRAY_BUFFER, obr.outnormal.size() * sizeof(glm::vec3), color_enemy[0], GL_STATIC_DRAW);
		GLint nAttribute = glGetAttribLocation(s_program[0], "in_Color");
		glVertexAttribPointer(nAttribute, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
		glEnableVertexAttribArray(nAttribute);
	}

	for (int i = 0; i < 3; i++) {
		glGenVertexArrays(1, &VAO_block[i]);
		glGenBuffers(2, VBO);

		glUseProgram(s_program[0]);
		glBindVertexArray(VAO_block[i]);
		glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
		glBufferData(GL_ARRAY_BUFFER, objReader.outvertex.size() * sizeof(glm::vec3), &objReader.outvertex[0], GL_STATIC_DRAW);
		GLint pAttribute = glGetAttribLocation(s_program[0], "aPos");
		glVertexAttribPointer(pAttribute, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
		glEnableVertexAttribArray(pAttribute);

		glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
		glBufferData(GL_ARRAY_BUFFER, objReader.outnormal.size() * sizeof(glm::vec3), color_enemy[0], GL_STATIC_DRAW);
		GLint nAttribute = glGetAttribLocation(s_program[0], "in_Color");
		glVertexAttribPointer(nAttribute, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
		glEnableVertexAttribArray(nAttribute);
	}

		for (int i = 0; i < obj_num0; i++) {
			glGenVertexArrays(1, &VAO_0[i]);
			glGenBuffers(2, VBO);

			glUseProgram(s_program[0]);
			glBindVertexArray(VAO_0[i]);
			glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
			glBufferData(GL_ARRAY_BUFFER, objReader.outvertex.size() * sizeof(glm::vec3), &objReader.outvertex[0], GL_STATIC_DRAW);
			GLint pAttribute = glGetAttribLocation(s_program[0], "aPos");
			glVertexAttribPointer(pAttribute, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
			glEnableVertexAttribArray(pAttribute);

			glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
			if (i == 0)
				glBufferData(GL_ARRAY_BUFFER, objReader.outnormal.size() * sizeof(glm::vec3), color_stage_0_to_3[0], GL_STATIC_DRAW);
			else
				glBufferData(GL_ARRAY_BUFFER, objReader.outnormal.size() * sizeof(glm::vec3), color_stage_0_to_3[i], GL_STATIC_DRAW);
			GLint nAttribute = glGetAttribLocation(s_program[0], "in_Color");
			glVertexAttribPointer(nAttribute, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
			glEnableVertexAttribArray(nAttribute);
		}
		for (int i = 0; i < obj_num1; i++) {
			glGenVertexArrays(1, &VAO_1[i]);
			glGenBuffers(2, VBO);

			glUseProgram(s_program[0]);
			glBindVertexArray(VAO_1[i]);
			glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
			glBufferData(GL_ARRAY_BUFFER, objReader.outvertex.size() * sizeof(glm::vec3), &objReader.outvertex[0], GL_STATIC_DRAW);
			GLint pAttribute = glGetAttribLocation(s_program[0], "aPos");
			glVertexAttribPointer(pAttribute, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
			glEnableVertexAttribArray(pAttribute);

			glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
			if (i == 0)
				glBufferData(GL_ARRAY_BUFFER, objReader.outnormal.size() * sizeof(glm::vec3), color_stage_0_to_3[0], GL_STATIC_DRAW);
			else
				glBufferData(GL_ARRAY_BUFFER, objReader.outnormal.size() * sizeof(glm::vec3), color_stage_0_to_3[i], GL_STATIC_DRAW);
			GLint nAttribute = glGetAttribLocation(s_program[0], "in_Color");
			glVertexAttribPointer(nAttribute, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
			glEnableVertexAttribArray(nAttribute);
		}
		for (int i = 0; i < obj_num2; i++) {
			glGenVertexArrays(1, &VAO_2[i]);
			glGenBuffers(2, VBO);

			glUseProgram(s_program[0]);
			glBindVertexArray(VAO_2[i]);
			glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
			glBufferData(GL_ARRAY_BUFFER, objReader.outvertex.size() * sizeof(glm::vec3), &objReader.outvertex[0], GL_STATIC_DRAW);
			GLint pAttribute = glGetAttribLocation(s_program[0], "aPos");
			glVertexAttribPointer(pAttribute, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
			glEnableVertexAttribArray(pAttribute);

			glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
			if(i > 9)
				glBufferData(GL_ARRAY_BUFFER, objReader.outnormal.size() * sizeof(glm::vec3), color_stage_0_to_3[4], GL_STATIC_DRAW);
			else if (i == 0)
				glBufferData(GL_ARRAY_BUFFER, objReader.outnormal.size() * sizeof(glm::vec3), color_stage_0_to_3[0], GL_STATIC_DRAW);
			else if (i >= 1 && i <= 3 * road_number2)
				glBufferData(GL_ARRAY_BUFFER, objReader.outnormal.size() * sizeof(glm::vec3), color_stage_0_to_3[i % 3 + 1], GL_STATIC_DRAW);
				
			GLint nAttribute = glGetAttribLocation(s_program[0], "in_Color");
			glVertexAttribPointer(nAttribute, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
			glEnableVertexAttribArray(nAttribute);
		}
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
				glBufferData(GL_ARRAY_BUFFER, objReader.outnormal.size() * sizeof(glm::vec3), color_stage_0_to_3[0], GL_STATIC_DRAW);
			else if (i >= 1 && i <= 3 * road_number3)
				glBufferData(GL_ARRAY_BUFFER, objReader.outnormal.size() * sizeof(glm::vec3), color_stage_0_to_3[i % 3 + 1], GL_STATIC_DRAW);
			else
				glBufferData(GL_ARRAY_BUFFER, objReader.outnormal.size() * sizeof(glm::vec3), color_stage_0_to_3[4], GL_STATIC_DRAW);
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

	switch (stage_number) {
	case 0: {
		Stage0();
		break;
	}
	case 1: {
		Stage1();
		break;
	}
	case 2: {
		  Stage2();

		  break;
	}
	case 3: {
		Stage3();
		break;
	}
	case 4: {
		Stage4();
		break;
	}
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
	object_speed = great_speed;
	switch (key) {
		// stage_number
		{case '0':
			Reset_Position();
			stage_number = 0;
			break;
		case '1':
			Reset_Position();
			stage_number = 1;
			break;
		case '2':
			Reset_Position();
			stage_number = 2;
			break;
		case '3':
			Reset_Position();
			stage_number = 3;
			break;
		case '4':
			Reset_Position();
			stage_number = 4;
			break; }

		// wasd+-	 
		{case 'w': case 'W':
			move_object_z -= object_speed;
			break;
		case 's': case 'S':
			move_object_z += object_speed;
			break;
		case 'a': case 'A':
			move_object_x -= object_speed;
			break;
		case 'd': case 'D':
			move_object_x += object_speed;
			break;
		case '+': case '=':
			object_speed += average_speed;
			if (object_speed > average_speed * 2)
				object_speed = average_speed * 2;
			break;
		case '-': case '_':
			object_speed -= average_speed;
			if (object_speed < average_speed / 2)
				object_speed = average_speed / 2;
			break; }

		// jump , slide
		{
		case 'j': case 'J':
			JUMP = true;
			SLIDE = false;
			glutTimerFunc(100, TimerFunction, 0);
			break;
		case 'l': case 'L':
			JUMP = false;
			SLIDE = true;
			glutTimerFunc(100, TimerFunction, 1);
			break;
		}

		/*{
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
		}*/

	case 'q': case 'Q':
		for (int i = 0; i < obj_num0; i++) {
			glDeleteVertexArrays(1, &VAO_0[i]);
			glDeleteBuffers(2, VBO);
		}
		for (int i = 0; i < obj_num1; i++) {
			glDeleteVertexArrays(1, &VAO_1[i]);
			glDeleteBuffers(2, VBO);
		}
		for (int i = 0; i < obj_num2; i++) {
			glDeleteVertexArrays(1, &VAO_2[i]);
			glDeleteBuffers(2, VBO);
		}
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

void TimerFunction(int value)
{
	if (JUMP) {
		move_object_y += object_jump_dir * object_jump_speed;
		if (move_object_y <= -0.23f) {
			move_object_y = -0.23f;
			object_jump_dir *= -1;
			JUMP = false;
		}
		else if (move_object_y > -0.09f)
			object_jump_dir *= -1;
	}
	if (SLIDE) {
		object_scale_y -= object_slide_dir * object_slide_speed;
		if (object_scale_y >= 0.07f) {
			object_scale_y = 0.07f;
			object_slide_dir *= -1;
			SLIDE = false;
		}
		else if (object_scale_y <= 0.02f)
			object_slide_dir *= -1;

	}

	InitBuffer();
	glutPostRedisplay();
	if (JUMP || SLIDE)
		glutTimerFunc(100, TimerFunction, 1);
}
