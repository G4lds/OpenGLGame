#include "config.h"
#include "shaderClass.h"
#include "Texture.h"

int displayLength = 1200;
int displayHeight = 800;
float ratio = float(displayHeight) / float(displayLength);
bool keypressed[] = {
	false, // key up
	false, // key down
	false, // key right
	false  // key left
};

// Vertices coordinates
GLfloat vertices[] =
	{
		//     COORDINATES     /        COLORS      /   TexCoord  //
		-0.1f, -0.1f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, // Lower left corner
		-0.1f, 0.1f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,  // Upper left corner
		0.1f, 0.1f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,	  // Upper right corner
		0.1f, -0.1f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f	  // Lower right corner
};

// Indices for vertices order
GLuint indices[] =
	{
		0, 2, 1, // Upper triangle
		0, 3, 2	 // Lower triangle
};

class EBO
{
public:
	// ID reference of Elements Buffer Object
	GLuint ID;
	// Constructor that generates a Elements Buffer Object and links it to indices
	EBO(GLuint *indices, GLsizeiptr size)
	{
		glGenBuffers(1, &ID);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ID);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, indices, GL_STATIC_DRAW);
	}

	// Binds the EBO
	void Bind()
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ID);
	}

	// Unbinds the EBO
	void Unbind()
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	// Deletes the EBO
	void Delete()
	{
		glDeleteBuffers(1, &ID);
	}
};

class VBO
{
public:
	// Reference ID of the Vertex Buffer Object
	GLuint ID;
	// Constructor that generates a Vertex Buffer Object and links it to vertices
	VBO(GLfloat *vertices, GLsizeiptr size)
	{
		glGenBuffers(1, &ID);
		glBindBuffer(GL_ARRAY_BUFFER, ID);
		glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
	}

	// Binds the VBO
	void Bind()
	{
		glBindBuffer(GL_ARRAY_BUFFER, ID);
	}

	// Unbinds the VBO
	void Unbind()
	{
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	// Deletes the VBO
	void Delete()
	{
		glDeleteBuffers(1, &ID);
	}
};

class VAO
{
public:
	// ID reference for the Vertex Array Object
	GLuint ID;
	VAO()
	{
		glGenVertexArrays(1, &ID);
	}

	// Links a VBO Attribute such as a position or color to the VAO
	void LinkAttrib(VBO &VBO, GLuint layout, GLuint numComponents, GLenum type, GLsizeiptr stride, void *offset)
	{
		VBO.Bind();
		glVertexAttribPointer(layout, numComponents, type, GL_FALSE, stride, offset);
		glEnableVertexAttribArray(layout);
		VBO.Unbind();
	}

	// Binds the VAO
	void Bind()
	{
		glBindVertexArray(ID);
	}

	// Unbinds the VAO
	void Unbind()
	{
		glBindVertexArray(0);
	}

	// Deletes the VAO
	void Delete()
	{
		glDeleteVertexArrays(1, &ID);
	}
};

class Thing
{
private:
	VAO VAOthing;
	VBO VBOthing;
	EBO EBOthing;
	Texture texturething;
	Texture texturethingbad;
	Texture texturethinggood;
	Shader shaderthing;
	GLuint uniID;
	GLuint xtranlate;
	GLuint ytranlate;
	GLuint scale;

public:
	float x, y;
	int colider;
	int tipe;
	float tick;

	Thing(float x, float y, int colider, int tipe, float *vertices, size_t verticesSize, unsigned int *indices, size_t indicesSize, Shader shader)
		: x(x), y(y), colider(colider), tipe(tipe), VBOthing(vertices, verticesSize), EBOthing(indices, indicesSize),
		  texturething("../img/thing.png", GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE), texturethingbad("../img/thingbad.png", GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE),
		  texturethinggood("../img/thinggood.png", GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE), shaderthing(shader)
	{
		tick = 1000.0f;

		uniID = glGetUniformLocation(shaderthing.ID, "ratio");
		scale = glGetUniformLocation(shaderthing.ID, "scale");
		xtranlate = glGetUniformLocation(shaderthing.ID, "xtranlate");
		ytranlate = glGetUniformLocation(shaderthing.ID, "ytranlate");

		// Generate and bind VAO
		VAOthing.Bind();

		// Link VBO attributes such as coordinates and colors to VAO
		VAOthing.LinkAttrib(VBOthing, 0, 3, GL_FLOAT, 8 * sizeof(float), (void *)0);
		VAOthing.LinkAttrib(VBOthing, 1, 3, GL_FLOAT, 8 * sizeof(float), (void *)(3 * sizeof(float)));
		VAOthing.LinkAttrib(VBOthing, 2, 2, GL_FLOAT, 8 * sizeof(float), (void *)(6 * sizeof(float)));

		// Unbind all to prevent accidentally modifying them
		VAOthing.Unbind();
		VBOthing.Unbind();
		EBOthing.Unbind();

		// Set texture
		texturething.texUnit(shaderthing, "tex0", 0);
		texturethingbad.texUnit(shaderthing, "tex0", 0);
		texturethinggood.texUnit(shaderthing, "tex0", 0);

		Spawn();
	}

	void Spawn()
	{
		y = float(rand() % displayLength * 2 - displayLength) / 1.2;
		x = float(rand() % displayHeight * 2 - displayHeight) / 1.2;
		tipe = rand() % 3 - 1;
		tick = rand() % 500 + 500;
	}

	void Tick()
	{
		tick -= 1;
		if (tick <= 0)
		{
			Spawn();
		}
	}

	void Draw()
	{

		// Assigns a value to the uniform; NOTE: Must always be done after activating the Shader Program
		glUniform1f(uniID, ratio);
		glUniform1f(scale, float((tick + 1000.0f) / 2000.0f));
		glUniform1f(xtranlate, y / displayLength);
		glUniform1f(ytranlate, x / displayHeight);

		if (tipe == 0)
			texturething.Bind();
		else if (tipe == 1)
			texturethinggood.Bind();
		else if (tipe == -1)
			texturethingbad.Bind();

		// Draw primitives, number of indices, datatype of indices, index of indices
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	}

	void Delete()
	{
		VAOthing.Delete();
		VBOthing.Delete();
		EBOthing.Delete();
		texturething.Delete();
		shaderthing.Delete();
	}
};

class Player
{
public:
	float x, y;
	float speed;
	int colider;
	float points;

	Player(float x, float y, float speed, int colider, int points)
		: x(x), y(y), speed(speed), colider(colider), points(points) {}

	void Move(bool left, bool right, bool up, bool down)
	{
		x -= speed * (right - left);
		y -= speed * (down - up);

		if ((y - colider/2 >= displayLength))
		{
			y = y * -1 + 10;
		}
		else if ((y + colider/2 <= -displayLength))
		{
			y = y * -1 + -10;
		}
		if ((x - colider/2 >= displayHeight))
		{
			x = x * -1 + 10;
		}
		else if ((x + colider/2 <= -displayHeight))
		{
			x = x * -1 + -10;
		}
	}

	void Colide(std::vector<Thing> &things)
	{
		for (auto &thing : things)
		{
			if (((x + colider) > (thing.x - thing.colider)) && ((x - colider) < (thing.x + thing.colider)))
				if (((y + colider) > (thing.y - thing.colider)) && ((y - colider) < (thing.y + thing.colider)))
				{
					points += thing.tipe;
					thing.Spawn();
				}
		}
	}

	bool Tick()
	{
		points = points/1.0005 - 0.005f;
		if (points < 1.0f)
			return true;
		else
			return false;

	}
};

Player *player = new Player(0.0f, 0.0f, 5.0f, 30, 10.0f);

void debug(std::vector<Thing> &things)
{
	system("clear");
	for (auto &thing : things)
	{
		std::cout << "X:" << thing.x << " Y:" << thing.y << " tipe:" << thing.tipe << std::endl;
	}
	std::cout << "X:" << player->x << " Y:" << player->y << " points:" << player->points << std::endl;

	std::cout << keypressed[0] + "=) UP" << std::endl;
	std::cout << keypressed[1] + "=) DOWN" << std::endl;
	std::cout << keypressed[2] + "=) RIGHT" << std::endl;
	std::cout << keypressed[3] + "=) LEFT" << std::endl;
}

void keyInput(GLFWwindow *window)
{
	// "ESC" fecha a janela
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	// "SETAS" movem o personagem
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS && !keypressed[0])
		keypressed[0] = true;
	else if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_RELEASE)
		keypressed[0] = false;
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS && !keypressed[1])
		keypressed[1] = true;
	else if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_RELEASE)
		keypressed[1] = false;
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS && !keypressed[2])
		keypressed[2] = true;
	else if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_RELEASE)
		keypressed[2] = false;
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS && !keypressed[3])
		keypressed[3] = true;
	else if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_RELEASE)
		keypressed[3] = false;
}

int main()
{
	// inicia o GLFW
	GLFWwindow *window;
	if (!glfwInit())
	{
		return -1;
	}
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);

	// cria a janela
	window = glfwCreateWindow(displayLength, displayHeight, "Jogo para pegar coisas (Reury)", NULL, NULL);
	glfwMakeContextCurrent(window);

	// inicializa o GLAD
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Couldn't load opengl" << std::endl;
		glfwTerminate();
		return -1;
	}

	// define o tamanho do viewport
	glViewport(0, 0, displayLength, displayHeight);

	// define a cor de fundo do viewport
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);

	// cria o shader program
	Shader shaderProgram(
		"../src/shaders/vertex.txt",
		"../src/shaders/fragment.txt");
	GLuint uniID = glGetUniformLocation(shaderProgram.ID, "ratio");
	GLuint scale = glGetUniformLocation(shaderProgram.ID, "scale");
	GLuint xtranlate = glGetUniformLocation(shaderProgram.ID, "xtranlate");
	GLuint ytranlate = glGetUniformLocation(shaderProgram.ID, "ytranlate");
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// <------------------------------------------------- ↓ player ↓ ------------------------------------------------>
	//
	// Generates Vertex Array Object and binds it
	VAO VAOplayer;
	VAOplayer.Bind();

	// Generates Vertex Buffer Object and links it to vertices
	VBO VBOplayer(vertices, sizeof(vertices));
	// Generates Element Buffer Object and links it to indices
	EBO EBOplayer(indices, sizeof(indices));

	// Links VBO attributes such as coordinates and colors to VAO
	VAOplayer.LinkAttrib(VBOplayer, 0, 3, GL_FLOAT, 8 * sizeof(float), (void *)0);
	VAOplayer.LinkAttrib(VBOplayer, 1, 3, GL_FLOAT, 8 * sizeof(float), (void *)(3 * sizeof(float)));
	VAOplayer.LinkAttrib(VBOplayer, 2, 2, GL_FLOAT, 8 * sizeof(float), (void *)(6 * sizeof(float)));
	// Unbind all to prevent accidentally modifying them
	VAOplayer.Unbind();
	VBOplayer.Unbind();
	EBOplayer.Unbind();
	// Texture
	Texture textureplayer(("../img/pop_cat.png"), GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE);
	textureplayer.texUnit(shaderProgram, "tex0", 0);
	//
	// <------------------------------------------------- ↑ player ↑ ------------------------------------------------>

	// <------------------------------------------------- ↓ placar ↓ ------------------------------------------------>
	//
	// Generates Vertex Array Object and binds it
	VAO VAODisplay;
	VAODisplay.Bind();

	// Generates Vertex Buffer Object and links it to vertices
	VBO VBODisplay(vertices, sizeof(vertices));
	// Generates Element Buffer Object and links it to indices
	EBO EBODisplay(indices, sizeof(indices));

	// Links VBO attributes such as coordinates and colors to VAO
	VAODisplay.LinkAttrib(VBODisplay, 0, 3, GL_FLOAT, 8 * sizeof(float), (void *)0);
	VAODisplay.LinkAttrib(VBODisplay, 1, 3, GL_FLOAT, 8 * sizeof(float), (void *)(3 * sizeof(float)));
	VAODisplay.LinkAttrib(VBODisplay, 2, 2, GL_FLOAT, 8 * sizeof(float), (void *)(6 * sizeof(float)));
	// Unbind all to prevent accidentally modifying them
	VAODisplay.Unbind();
	VBODisplay.Unbind();
	EBODisplay.Unbind();
	// Texture
	Texture textureDisplay(("../img/placar.png"), GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE);
	textureDisplay.texUnit(shaderProgram, "tex0", 0);
	//
	// <------------------------------------------------- ↑ placar ↑ ------------------------------------------------>

	// <------------------------------------------------- ↓ coisas ↓ ------------------------------------------------>
	//
	std::vector<Thing> things;
	for (int i : {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14})
	{
		things.emplace_back(displayHeight * 2, displayLength * 2, 30, 1, vertices, sizeof(vertices), indices, sizeof(indices), shaderProgram);
	}
	//
	// <------------------------------------------------- ↑ coisas ↑ ------------------------------------------------>

	// loop principali
	while (!glfwWindowShouldClose(window))
	{
		// move o player
		player->Move(keypressed[0], keypressed[1], keypressed[2], keypressed[3]);

		if (player->Tick())
			break;

		// checa a colisao do player
		player->Colide(things);

		// limpa a cena
		glClear(GL_COLOR_BUFFER_BIT);

		// <------------------------------------------------ ↓ player ↓ ----------------------------------------------->
		//
		// Tell OpenGL which Shader Program we want to use
		shaderProgram.Activate();
		// Assigns a value to the uniform; NOTE: Must always be done after activating the Shader Program
		glUniform1f(uniID, ratio);
		glUniform1f(scale, 1.0f);
		glUniform1f(xtranlate, player->y / displayLength);
		glUniform1f(ytranlate, player->x / displayHeight);
		textureplayer.Bind();
		// Bind the VAO so OpenGL knows to use it
		VAOplayer.Bind();
		// Draw primitives, number of indices, datatype of indices, index of indices
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		//
		// <------------------------------------------------ ↑ player ↑ ----------------------------------------------->

		// <------------------------------------------------ ↓ placar ↓ ----------------------------------------------->
		//
		// Tell OpenGL which Shader Program we want to use
		shaderProgram.Activate();
		// Assigns a value to the uniform; NOTE: Must always be done after activating the Shader Program
		glUniform1f(uniID, float(player->points-1)*2.0f);
		glUniform1f(scale, 0.2f);
		glUniform1f(xtranlate, 0);
		glUniform1f(ytranlate, 0.9f);
		textureDisplay.Bind();
		// Bind the VAO so OpenGL knows to use it
		VAODisplay.Bind();
		// Draw primitives, number of indices, datatype of indices, index of indices
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		//
		// <------------------------------------------------ ↑ placar ↑ ----------------------------------------------->

		// <------------------------------------------------ ↓ coisas ↓ ----------------------------------------------->
		//
		for (auto &thing : things)
		{
			thing.Draw();
			thing.Tick();
		}
		//
		// <------------------------------------------------ ↑ coisas ↑ ----------------------------------------------->

		// troca para a cena renderizada
		glfwSwapBuffers(window);

		// checa por eventos
		glfwPollEvents();
		keyInput(window);

		// debuga
		debug(things);
	}

	// destroi todos os objetoscriados, shaders e a janela
		VAOplayer.Delete();
		VBOplayer.Delete();
		EBOplayer.Delete();
		textureplayer.Delete();
		shaderProgram.Delete();
	for (auto &thing : things)
	{
		thing.Delete();
	}
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}
