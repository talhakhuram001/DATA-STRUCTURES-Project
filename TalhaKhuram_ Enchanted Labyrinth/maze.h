#pragma once
#include <SFML/Graphics.hpp>
#include<iostream>
#include <queue>
#include <cmath>
#include<time.h>

using namespace sf;
using namespace std;
class Enemy;
class EnemyList;
class Treeasurelist;
class Treasure;

struct Node
{
	int x, y;
	int g; // Cost from start
	int h; // Heuristic (estimated cost to goal)
	Node* parent;

	Node(int x, int y, int g, int h, Node* parent = nullptr)
		: x(x), y(y), g(g), h(h), parent(parent) {}

	int f() const { return g + h; }
};

class CompareNodes
{
public:
	bool operator()(const Node* lhs, const Node* rhs) const
	{
		return lhs->f() > rhs->f();
	}
};

template<typename T>
class MyPriorityQueue
{
	priority_queue<T, vector<T>, CompareNodes> pq;

public:
	void push(const T& value)
	{
		pq.push(value);
	}

	void pop()
	{
		pq.pop();
	}

	const T& top() const
	{
		return pq.top();
	}

	bool empty() const
	{
		return pq.empty();
	}

	size_t size() const
	{
		return pq.size();
	}
};



class Maze {
	Texture walls_texture_horizontal;
	Texture walls_texture_vertical;
	RectangleShape** blocks;
	RectangleShape** foods;
	Color* playerColors;
	int verticalLineGap;
	int horizontalLineGap;

public:
	int n;
	int m;
	Sprite** walls_sprite;
	int** valid;
	int*** coords;

	Maze() {
		srand(time(0));
		n = 12;
		m = 23;
		blocks = new RectangleShape * [m];
		foods = new RectangleShape * [n];
		playerColors = new Color[4];
		playerColors[0] = Color::Red;
		playerColors[1] = Color::Blue;
		playerColors[2] = Color::Green;
		playerColors[3] = Color::Yellow;
		verticalLineGap = 1000 / 15;
		horizontalLineGap = 920 / 15;

		for (int i = 0; i < m; ++i) {
			blocks[i] = new RectangleShape[n];
			for (int j = 0; j < n; ++j) {
				RectangleShape block(Vector2f(verticalLineGap - 4, horizontalLineGap - 4));
				block.setPosition(Vector2f(i * verticalLineGap + 211, j * horizontalLineGap + 201));
				block.setFillColor(Color::Transparent);
				block.setOutlineColor(Color::White);
				block.setOutlineThickness(1);
				blocks[i][j] = block;
			}
		}

		walls_sprite = new Sprite * [n + m];
		for (int i = 0; i < n; ++i) {
			walls_sprite[i] = new Sprite[n];
		}

		valid = new int* [m];
		for (int i = 0; i < m; ++i) {
			valid[i] = new int[n];
			for (int j = 0; j < n; ++j) {
				valid[i][j] = 1;
			}

		}
		int random = rand() % 10;
		for (int i = 1; i < m - 1; i++)
		{
			int walls = n / 3;
			int j = 0;
			while (walls > 0)
			{
				if (rand() % 10 == random && walls != 0) {
					valid[i][j] = 0;
					walls--;
				}
				j++;
				if (j == n) {
					j = 0;
				}
			}
		}





		coords = new int** [m];
		for (int i = 0; i < m; ++i) {
			coords[i] = new int* [n];
			for (int j = 0; j < n; ++j) {
				coords[i][j] = new int[2];
				coords[i][j][0] = i * verticalLineGap + 160;
				coords[i][j][1] = j * horizontalLineGap + 201;
			}
		}
	}


	Node** findShortestPath(int startX, int startY, int goalX, int goalY)
	{
		// Create a priority queue to store nodes to be explored, using the comparison function compareNodes
		MyPriorityQueue<Node*> obj;

		// Create a 2D array to mark visited nodes
		bool** closed = new bool* [m];
		for (int i = 0; i < m; ++i)
			closed[i] = new bool[n](); // Initialize to false

		// Create the start node and add it to the open list
		Node* start = new Node(startX, startY, 0, heuristic(startX, startY, goalX, goalY));
		obj.push(start);

		// Loop until all nodes are explored or the goal is found
		while (!obj.empty())
		{
			// Get the node with the lowest cost from the open list
			Node* current = obj.top();
			obj.pop();

			// If the current node is the goal, reconstruct and return the path
			if (current->x == goalX && current->y == goalY)
			{
				// Found goal, reconstruct path
				Node** path = new Node * [m * n];
				int pathLength = 0;
				while (current != nullptr)
				{
					path[pathLength++] = current;
					current = current->parent;
				}
				path[pathLength] = nullptr; // Mark end of path
				return path;
			}

			// Mark the current node as visited
			closed[current->x][current->y] = true;

			// Explore neighbors of the current node
			for (int dx = -1; dx <= 1; ++dx)
			{
				for (int dy = -1; dy <= 1; ++dy)
				{
					// Skip the current node
					if (dx == 0 && dy == 0)
						continue;

					// Calculate the coordinates of the neighbor
					int nx = current->x + dx;
					int ny = current->y + dy;

					// Check if the neighbor is within bounds and unvisited
					if (nx >= 0 && nx < m && ny >= 0 && ny < n && isValid(nx, ny) && !closed[nx][ny])
					{
						// Check if diagonal move is valid (not blocked by two adjacent blocks)
						if (dx != 0 && dy != 0) {
							if (!isValid(current->x + dx, current->y) || !isValid(current->x, current->y + dy))
							{
								continue; // Skip diagonal moves if blocked by adjacent blocks
							}
						}

						// Calculate the tentative cost to reach the neighbor
						int tentativeG = current->g + 1; // Assuming uniform cost

						// Create the neighbor node
						Node* neighbor = new Node(nx, ny, tentativeG, heuristic(nx, ny, goalX, goalY), current);

						// Add the neighbor to the open list
						obj.push(neighbor);

						// Mark the neighbor as visited
						closed[nx][ny] = true;
					}
				}
			}
		}

		return nullptr; // No path found
	}



	// Heuristic function (Manhattan distance)
	int heuristic(int x1, int y1, int x2, int y2)
	{
		return abs(x2 - x1) + abs(y2 - y1);
	}

	// Compare function for priority queue
	static bool compareNodes(const Node* lhs, const Node* rhs)
	{
		return lhs->f() > rhs->f();
	}



	void drawmaze(RenderWindow& window) {
		Texture bg_texture;
		bg_texture.loadFromFile("black.jpeg");
		Sprite bg_sprite(bg_texture);
		bg_sprite.scale(2, 2);
		bg_sprite.setPosition(0, 200);

		Texture inventory_texture;
		inventory_texture.loadFromFile("black.jpeg");
		Sprite inventory_sprite(inventory_texture);
		inventory_sprite.scale(5, 0.6);
		inventory_sprite.setPosition(0, 0);

		Texture highscore_texture;
		highscore_texture.loadFromFile("black.jpeg");
		Sprite highscore_sprite(highscore_texture);
		highscore_sprite.scale(5, 0.64);
		highscore_sprite.setPosition(1300, 0);

		window.draw(bg_sprite);

		window.draw(inventory_sprite);
		window.draw(highscore_sprite);

		for (int i = 0; i < m; ++i) {
			for (int j = 0; j < n; ++j) {
				if (valid[i][j] == 0) {
					RectangleShape square(Vector2f(verticalLineGap - 4, horizontalLineGap - 4));
					square.setPosition(Vector2f(i * verticalLineGap + 150, j * horizontalLineGap + 211));
					square.setFillColor(Color::White); // Set color as per your preference
					window.draw(square);
				}
			}
		}

		Node** shortestPath = findShortestPath(0, 0, m - 1, n - 1);
		if (shortestPath != nullptr) {
			for (int i = 0; shortestPath[i] != nullptr; ++i) {
				Node* node = shortestPath[i];
				if (valid[node->x][node->y] == 1) { // Only draw dots on valid positions
					CircleShape dot(6); // Adjust size as needed
					dot.setFillColor(Color::Yellow); // Highlight color
					dot.setPosition(Vector2f(node->x * verticalLineGap + 150, node->y * horizontalLineGap + 211));
					window.draw(dot);
				}
			}
		}

	}



	Sprite& getWallSprite(int i, int j) {
		return walls_sprite[i][j];
	}

	bool isValid(int i, int j) const {
		if (i >= m || j >= n) {
			return false;
		}
		return valid[i][j] != 0;
	}

};



class Fire {
public:
	Sprite sprite;
	Texture fire;

	int posX = 0;
	int posY = 0;
	int direction = 0;
	int moved = 0;
	int type;
	bool visible = true;
	Fire* next = nullptr;
	Fire* prev = nullptr;
	Fire() {}
	Fire(Maze& maze, int i, int j, int dir, int type, int subtype) :type(type) {
		if (type == 1) {
			if (subtype == 0)
				fire.loadFromFile("fire1.png");
			if (subtype == 1)
				fire.loadFromFile("newfire_rec1.png");
			if (subtype == 2)
				fire.loadFromFile("fire1.png");
			if (subtype == 3)
				fire.loadFromFile("fire1.png");
			if (subtype == 4)
				fire.loadFromFile("newfire_rec2.png");
			if (subtype == 5)
				fire.loadFromFile("newfire4.png");
			if (subtype == 6)
				fire.loadFromFile("newfire3.png");
			if (subtype == 7)
				fire.loadFromFile("newfire2.png");
			if (subtype == 8)
				fire.loadFromFile("newfire1.png");
		}
		if (type == 2)
			fire.loadFromFile("fire2.png");
		sprite.setTexture(fire);
		sprite.setPosition(maze.coords[i][j][0], maze.coords[i][j][1] + 30);
		sprite.scale(0.1, 0.1);
		direction = dir;
	}
	void set_position(Maze& maze) {
		if (posX >= 0 && posX < 23 && posY >= 0 && posY < 12) {

			sprite.setPosition(maze.coords[posX][posY][0], maze.coords[posX][posY][1] + 30);
		}
		else {
			visible = false;
		}
	}

	void moveUp(Maze& maze) {

		posY--;

		set_position(maze);
	}

	void moveDown(Maze& maze) {

		posY++;

		set_position(maze);
	}

	void moveLeft(Maze& maze) {

		posX--;
		set_position(maze);
	}

	void moveRight(Maze& maze) {

		posX++;
		set_position(maze);
	}
	void move(Maze& maze) {
		int val;
		if (type == 1) {
			val = 5;
		}
		else {
			val = 4;
		}
		if (moved <= val) {
			if (direction == 0) {
				moveUp(maze);
			}
			else if (direction == 1) {
				moveDown(maze);
			}
			else if (direction == 2) {
				moveLeft(maze);
			}
			else if (direction == 3) {
				moveRight(maze);
			}
			else {

			}
		}
		else {
			visible = false;


		}
		moved++;
	}


	void draw(RenderWindow& window) {
		if (visible == true)
			window.draw(sprite);
	}
};


class Player {
	Sprite sprite;
	Texture player;
public:
	Fire* start = nullptr;
	int health = 500;
	int posX = 0;
	int posY = 0;
	Player(Maze& maze) {
		player.loadFromFile("PLAYER1.png");
		sprite.setTexture(player);
		sprite.setPosition(maze.coords[posX][posY][0], maze.coords[posX][posY][1]);
		sprite.scale(0.12, 0.12);
	}
	void set_position(Maze& maze) {
		sprite.setPosition(maze.coords[posX][posY][0], maze.coords[posX][posY][1]);



	}
	void create_fire(Maze& maze, int direction, int val) {
		Fire* create = new Fire(maze, posX, posY, direction, 1, val);
		create->posX = posX;
		create->posY = posY;

		if (start == nullptr) {
			start = create;
		}
		else {
			Fire* curr = start;
			while (curr->next != nullptr) {
				curr = curr->next;
			}
			curr->next = create;
			create->prev = curr;
		}
		create->move(maze);
	}

	void updateFirePositions(Maze& maze) {
		Fire* curr = start;
		while (curr != nullptr) {
			curr->move(maze);

			if (curr->moved > 4) {

				if (curr == start) {
					start = curr->next;
					if (start)
						start->prev = nullptr;
				}
				else {
					if (curr->prev)
						curr->prev->next = curr->next;
					if (curr->next)
						curr->next->prev = curr->prev;
				}

				Fire* nextFire = curr->next;
				delete curr;
				curr = nextFire;
			}
			else {
				curr = curr->next;
			}
		}
	}


	void moveUp(Maze& maze) {
		if (posY > 0 && maze.isValid(posX, posY - 1)) {
			posY--;

			set_position(maze);
		}
	}

	void moveDown(Maze& maze) {
		if (posY < 35 && maze.isValid(posX, posY + 1)) {
			posY++;

			set_position(maze);
		}
	}

	void moveLeft(Maze& maze) {
		if (posX > 0 && maze.isValid(posX - 1, posY)) {
			player.loadFromFile("PLAYER4.png");
			sprite.setTexture(player);
			posX--;
			set_position(maze);
		}
	}

	void moveRight(Maze& maze) {
		if (posX < 80 && maze.isValid(posX + 1, posY)) {
			player.loadFromFile("PLAYER1.png");
			sprite.setTexture(player);
			posX++;
			set_position(maze);
		}
	}


	void draw(RenderWindow& window) {
		window.draw(sprite);
		Fire* curr = start;
		while (curr)
		{
			if (curr->visible)
				window.draw(curr->sprite);
			curr = curr->next;
		}
	}

};

//class Enemy {
//	Sprite sprite;
//	Texture enemy;
//public:
//	bool visible = true;;
//	int posX = 0;
//	int posY = 0;
//	int health = 500;
//	Fire* start = nullptr;
//	Enemy* next = nullptr;
//	Enemy() {}
//
//	Enemy(Maze& maze) {
//
//		srand(time(0));
//		int val = rand() % 4;
//		if (val == 0) {
//			enemy.loadFromFile("demons1.png");
//
//		}
//		else  if (val == 1) {
//			enemy.loadFromFile("demons2.png");
//		}
//		else  if (val == 2) {
//			enemy.loadFromFile("demons3.png");
//		}
//		else {
//			enemy.loadFromFile("demons4.png");
//		}
//		int random = rand() % 10;
//		for (int j = 1; j < maze.n; ++j) {
//			int i = 1;
//			for (int i = 1; i < maze.m - 1; ++i) {
//				if (rand() % 10 == random && maze.valid[i][j] != 0 && testing_row(i, j, maze)) {
//					maze.valid[i][j] = 3;
//					posX = i;
//					posY = j;
//					sprite.setTexture(enemy);
//					sprite.setPosition(maze.coords[posX][posY][0], maze.coords[posX][posY][1]);
//					sprite.scale(0.1, 0.1);
//					j++;
//					return;
//				}
//
//			}
//
//		}
//
//
//	}
//	bool testing_row(int i, int j, Maze& maze) {
//		for (int i = 0; i < maze.n; i++)
//		{
//			if (maze.valid[i][j] == 3) {
//				return false;
//			}
//		}
//		return true;
//	}
//
//	void set_position(Maze& maze) {
//		sprite.setPosition(maze.coords[posX][posY][0], maze.coords[posX][posY][1]);
//	}
//
//	void create_fire(Maze& maze, int direction) {
//		Fire* create = new Fire(maze, posX, posY, direction, 2, 0);
//		create->posX = posX;
//		create->posY = posY;
//		if (start == nullptr) {
//			start = create;
//		}
//		else {
//			Fire* curr = start;
//			while (curr->next != nullptr) {
//				curr = curr->next;
//			}
//			curr->next = create;
//			create->prev = curr;
//		}
//		create->move(maze);
//	}
//
//	void updateFirePositions(Maze& maze) {
//		Fire* curr = start;
//		while (curr != nullptr) {
//			curr->move(maze);
//
//			if (curr->moved > 4) {
//
//				if (curr == start) {
//					start = curr->next;
//					if (start)
//						start->prev = nullptr;
//				}
//				else {
//					if (curr->prev)
//						curr->prev->next = curr->next;
//					if (curr->next)
//						curr->next->prev = curr->prev;
//				}
//
//				Fire* nextFire = curr->next;
//				delete curr;
//				curr = nextFire;
//			}
//			else {
//				curr = curr->next;
//			}
//		}
//	}
//
//
//	void moveUp(Maze& maze) {
//		if (posY > 0 && maze.isValid(posX, posY - 1)) {
//			posY--;
//
//			set_position(maze);
//		}
//	}
//
//	void moveDown(Maze& maze) {
//		if (posY < 35 && maze.isValid(posX, posY + 1)) {
//			posY++;
//
//			set_position(maze);
//		}
//	}
//
//	void moveLeft(Maze& maze) {
//		if (posX > 0 && maze.isValid(posX - 1, posY)) {
//
//			posX--;
//			set_position(maze);
//		}
//	}
//
//	void moveRight(Maze& maze) {
//		if (posX < 79 && maze.isValid(posX + 1, posY)) {
//
//			posX++;
//			set_position(maze);
//		}
//	}
//
//	void draw(RenderWindow& window) {
//		if (visible)
//			window.draw(sprite);
//		Fire* curr = start;
//		while (curr)
//		{
//			if (curr->visible)
//				window.draw(curr->sprite);
//			curr = curr->next;
//		}
//	}
//
//};


//class Enemylist {
//public:
//	Enemy* start = nullptr;
//	Enemylist() {}
//	void insert(Maze& maze) {
//		Enemy* create = new Enemy(maze);
//		if (start == nullptr) {
//			start = create;
//		}
//		else {
//			Enemy* curr = start;
//			while (curr->next)
//			{
//				curr = curr->next;
//			}
//			curr->next = create;
//		}
//	}
//	void create(int val, Maze& maze) {
//		for (int i = 0; i < val; i++)
//		{
//			insert(maze);
//		}
//	}
//	void display(RenderWindow& window) {
//		Enemy* curr = start;
//		while (curr)
//		{
//			curr->draw(window);
//			curr = curr->next;
//		}
//
//	}
//	void move(Maze& maze) {
//		Enemy* curr = start;
//		int count = rand() % 5;
//		while (curr)
//		{
//			if (curr->visible)
//				if (count % 2 == 1) {
//
//					int val = rand() % 4;
//					if (val == 0) {
//						curr->moveUp(maze);
//					}
//					else if (val == 1) {
//						curr->moveDown(maze);
//					}
//					else if (val == 2) {
//						curr->moveLeft(maze);
//					}
//					else {
//						curr->moveRight(maze);
//					}
//				}
//			count++;
//
//			curr = curr->next;
//		}
//	}
//
//};

class Enemy {
	Sprite sprite;
	Texture enemy;
public:
	bool visible = true;;
	int posX = 0;
	int posY = 0;
	int health = 500;
	Fire* start = nullptr;
	Enemy* next = nullptr;
	Enemy() {}

	Enemy(Maze& maze) {

		srand(time(0));
		int val = rand() % 4;
		if (val == 0) {
			enemy.loadFromFile("demons1.png");

		}
		else  if (val == 1) {
			enemy.loadFromFile("demons2.png");
		}
		else  if (val == 2) {
			enemy.loadFromFile("demons3.png");
		}
		else {
			enemy.loadFromFile("demons4.png");
		}
		int random = rand() % 10;
		for (int j = 1; j < maze.n; ++j) {
			int i = 1;
			for (int i = 1; i < maze.m - 1; ++i) {
				if (rand() % 10 == random && maze.valid[i][j] != 0 && testing_row(i, j, maze)) {
					maze.valid[i][j] = 3;
					posX = i;
					posY = j;
					sprite.setTexture(enemy);
					sprite.setPosition(maze.coords[posX][posY][0], maze.coords[posX][posY][1]);
					sprite.scale(0.1, 0.1);
					j++;
					return;
				}

			}

		}


	}
	bool testing_row(int n, int j, Maze& maze) {
		for (int i = 0; i < maze.n; i++)
		{
			if (maze.valid[n][j] == 3) {
				return false;
			}
		}
		return true;
	}

	void set_position(Maze& maze) {
		sprite.setPosition(maze.coords[posX][posY][0], maze.coords[posX][posY][1]);
	}

	void create_fire(Maze& maze, int direction) {
		Fire* create = new Fire(maze, posX, posY, direction, 2, 0);
		create->posX = posX;
		create->posY = posY;
		if (start == nullptr) {
			start = create;
		}
		else {
			Fire* curr = start;
			while (curr->next != nullptr) {
				curr = curr->next;
			}
			curr->next = create;
			create->prev = curr;
		}
		create->move(maze);
	}

	void updateFirePositions(Maze& maze) {
		Fire* curr = start;
		while (curr != nullptr) {
			curr->move(maze);

			if (curr->moved > 4) {

				if (curr == start) {
					start = curr->next;
					if (start)
						start->prev = nullptr;
				}
				else {
					if (curr->prev)
						curr->prev->next = curr->next;
					if (curr->next)
						curr->next->prev = curr->prev;
				}

				Fire* nextFire = curr->next;
				delete curr;
				curr = nextFire;
			}
			else {
				curr = curr->next;
			}
		}
	}


	void moveUp(Maze& maze) {
		if (posY > 0 && maze.isValid(posX, posY - 1)) {
			posY--;

			set_position(maze);
		}
	}

	void moveDown(Maze& maze) {
		if (posY < 35 && maze.isValid(posX, posY + 1)) {
			posY++;

			set_position(maze);
		}
	}

	void moveLeft(Maze& maze) {
		if (posX > 0 && maze.isValid(posX - 1, posY)) {

			posX--;
			set_position(maze);
		}
	}

	void moveRight(Maze& maze) {
		if (posX < 79 && maze.isValid(posX + 1, posY)) {

			posX++;
			set_position(maze);
		}
	}

	void draw(RenderWindow& window) {
		if (visible)
			window.draw(sprite);
		Fire* curr = start;
		while (curr)
		{
			if (curr->visible)
				window.draw(curr->sprite);
			curr = curr->next;
		}
	}

};

class Enemylist {
public:
	Enemy* start = nullptr;
	Enemylist() {}
	void insert(Maze& maze) {
		Enemy* create = new Enemy(maze);
		if (start == nullptr) {
			start = create;
		}
		else {
			Enemy* curr = start;
			while (curr->next)
			{
				curr = curr->next;
			}
			curr->next = create;
		}
	}
	void create(int val, Maze& maze) {
		for (int i = 0; i < val; i++)
		{
			insert(maze);
		}
	}
	void display(RenderWindow& window) {
		Enemy* curr = start;
		while (curr)
		{
			curr->draw(window);
			curr = curr->next;
		}

	}
	void move(Maze& maze) {
		Enemy* curr = start;
		int count = rand() % 5;
		while (curr)
		{
			if (curr->visible)
				if (count % 2 == 1) {

					int val = rand() % 4;
					if (val == 0) {
						curr->moveUp(maze);
					}
					else if (val == 1) {
						curr->moveDown(maze);
					}
					else if (val == 2) {
						curr->moveLeft(maze);
					}
					else {
						curr->moveRight(maze);
					}
				}
			count++;

			curr = curr->next;
		}
	}

};





class Treasure {
	Sprite sprite;
	Texture enemy;
public:
	bool visible = true;;
	int posX = 0;
	int posY = 0;
	int health = 1000;
	Treasure* next = nullptr;
	int reward;
	Treasure() {}

	Treasure(Maze& maze, int reward) :reward(reward) {

		int val = rand() % 4;
		if (val == 0) {
			enemy.loadFromFile("treasure1.png");

		}
		else  if (val == 1) {
			enemy.loadFromFile("treasure2.png");
		}
		else  if (val == 2) {
			enemy.loadFromFile("treasure3.png");
		}
		else {
			enemy.loadFromFile("treasure4.png");
		}
		int random = rand() % 10;
		for (int j = 1; j < maze.n; ++j) {

			for (int i = maze.m - 3; i >= 1; --i) {
				if (rand() % 10 == random && maze.valid[i][j] == 1 && testing_row(i, j, maze)) {
					maze.valid[i][j] = 4;
					posX = i;
					posY = j;
					sprite.setTexture(enemy);
					sprite.setPosition(maze.coords[posX][posY][0], maze.coords[posX][posY][1]);
					sprite.scale(0.1, 0.1);
					j++;
					return;
				}

			}

		}


	}
	bool testing_row(int i, int j, Maze& maze) {
		for (int i = 0; i < maze.n; i++)
		{
			if (maze.valid[i][j] == 4) {
				return false;
			}
		}
		return true;
	}

	void set_position(Maze& maze) {
		sprite.setPosition(maze.coords[posX][posY][0], maze.coords[posX][posY][1]);
	}


	void draw(RenderWindow& window) {
		if (visible)
			window.draw(sprite);
	}

};

class Treeasurelist {
public:
	Treasure* start = nullptr;
	Treeasurelist() {}
	void insert(Maze& maze, int reward) {
		Treasure* create = new Treasure(maze, reward);
		if (start == nullptr) {
			start = create;
		}
		else {
			Treasure* curr = start;
			while (curr->next)
			{
				curr = curr->next;
			}
			curr->next = create;
		}
	}
	void create(int val, Maze& maze) {
		for (int i = 0; i < val; i++)
		{
			insert(maze, i + 1);
		}
	}
	void display(RenderWindow& window) {
		Treasure* curr = start;
		while (curr)
		{
			curr->draw(window);
			curr = curr->next;
		}

	}


};