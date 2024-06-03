#include <SFML/Graphics.hpp>
#include <vector>
int main() {
	const int fieldWidth = 10;    //限制一個格子高度及寬度
	const int fieldHeight = 20;

	enum class Type {
		None,
		O,
		I,
	};

	Type field[fieldWidth][fieldHeight] = {};			//控制格子是否留在原地的開關

	sf::Texture yellowTexture;   //開啟黃色方塊圖片
	if (!yellowTexture.loadFromFile("yellow.png")) {
		return EXIT_FAILURE;	//當沒有開啟的時候結束程式	
	}
	sf::Texture lightBlueTextture;
	if (!lightBlueTextture.loadFromFile("light_blue.png")) {
		return EXIT_FAILURE;
	}
	std::map<Type, std::vector<std::vector<sf::Vector2i>>> shapes = {   //使用vector來儲存(才可以複製給currentShape)
		//shape_Os   繪製2*2的黃色方塊 利用座標系統來完成 	
		{
			Type::O,
			{
				{
				sf::Vector2i(0, 0),									//shape_Is  [0][0]
				sf::Vector2i(1, 0),
				sf::Vector2i(0,-1),
				sf::Vector2i(1,-1),
				}
			},
		},
		//shape_Is    //繪製1*4的黃色方塊 利用座標系統來完成
		{
			Type::I,
			{
				{
				sf::Vector2i(-1,0),								//shape_Is  [1][0]
				sf::Vector2i(0,0),
				sf::Vector2i(1,0),
				sf::Vector2i(2,0),
				},
				{
				sf::Vector2i(0,-2),								//shape_Is  [1][0]
				sf::Vector2i(0,-1),
				sf::Vector2i(0, 0),
				sf::Vector2i(0, 1),
				}
			}
		}
	};

	Type currentType = Type(rand() % 2 + 1);   //(使用整數來表示ENUM的用法)RAND%2 為0 設定為 O形 ，為1設定為 I形
	int currentIndex = 0;						//使方塊做旋轉 所有的方塊都有  0 號這個方向

	const int blockWidth = yellowTexture.getSize().x;   //黃色格子的寬度及高度
	const int blockHeight = yellowTexture.getSize().y;

	const int windowWidth = fieldWidth * blockWidth;    //視窗的寬度及高度
	const int windowHeight = fieldHeight * blockHeight;

	std::map<Type, sf::Sprite> sprites = {					//使用vector來儲存(才可以複製給currentSprite)
		{Type::O, sf::Sprite(yellowTexture)},
		{Type::I, sf::Sprite(lightBlueTextture)},
	};

	std::vector<sf::Vector2i> currentShape;   //選定現在落下的方塊(使用rand做隨機抽選一個形狀)
	sf::Sprite currentSprite;

	const sf::Vector2i origin(fieldWidth / 2, 0);		//一開始會在場地的中間  (場地的寬 / 2, 高度為 0)	
	sf::Vector2i pos(origin);							//擷取格子目前位置

	sf::RenderWindow window(sf::VideoMode(windowWidth, windowHeight), L"俄羅斯方塊");
	sf::Clock clock;
	while (window.isOpen()) {
		currentShape  = shapes[currentType][currentIndex];				//選定方塊形狀  O...I...  
		currentSprite = sprites[currentType];				//繪製方塊顏色

		enum class Action {      //案件的互動
			Hold,
			MoveDown,
			MoveLeft,
			MoveRight,
			Rotate,
		};
		Action action = Action::Hold;   //一開始default為Hold

		sf::Event evt;				//開啟視窗
		if (window.pollEvent(evt)) {
			if (evt.type == sf::Event::Closed) {             //設定視窗的開關
				window.close();
			}
			if (evt.type == sf::Event::KeyPressed) {
				switch (evt.key.code) {
				case sf::Keyboard::Left:
					action = Action::MoveLeft;
					break;
				case sf::Keyboard::Right:
					action = Action::MoveRight;
					break;
				case sf::Keyboard::Up:
					action = Action::Rotate;
					break;
				}
			}
		}

		if (clock.getElapsedTime().asSeconds() >= 0.3f) {    //用Clock來設定每0.3秒下降一格  最後要restart時間
			action = Action::MoveDown;						//default一開始就一直往下	
			clock.restart();
		}

		sf::Vector2i nextPos(pos);
		int nextIndex = currentIndex;
		if (action == Action::Rotate) {
			nextIndex = (nextIndex + 1) % shapes[currentType].size();  //使用%求於數來有幾種旋轉的方向的上限
		}
		std::vector<sf::Vector2i> nextShape = shapes[currentType][nextIndex];

		switch (action) {									//用switch來設定上下左右
		case Action::Hold:
			break;
		case Action::MoveDown:
			nextPos.y++;
			break;
		case Action::MoveLeft:
			nextPos.x--;
			break;
		case Action::MoveRight:
			nextPos.x++;
			break;
		}
		int countEmpty = 0;
		for (const sf::Vector2i d : nextShape) {
			sf::Vector2i np = nextPos + d;					// 4個格子的位移
			if (np.x >= 0 && np.x < fieldWidth &&			//設定4個格子的下一個位置不能超出原始視窗的寬高
				np.y < fieldHeight &&						//左右兩邊不可超過 底部碰到保留 
				(np.y < 0 || field[np.x][np.y] == Type::None)) { //除了檢查不能超越原始視窗的寬高 也要看該位置是否已經有格子													 
				countEmpty++;
			}
		}

		if (countEmpty == 4) {
			pos = nextPos;					//更新變動過後的參數
			currentIndex = nextIndex;
			currentShape = nextShape;
		}
		else {
			if (action == Action::MoveDown) {
				for (const sf::Vector2i& d : currentShape) {
					sf::Vector2i np = pos + d;
					if (np.y >= 0) {
						field[np.x][np.y] = currentType;
					}										//當碰到不能在往下時候 記得這個格子 
				}
				for (int y = 0; y < fieldHeight; y++) {
					bool isFull = true;
					for (int x = 0; x < fieldWidth; x++) {
						if (field[x][y] == Type::None) {
							isFull = false;
						}
					}
					if (isFull) {
						for (int ty = y; ty > 0; ty--) {
							for (int x = 0; x < fieldWidth; x++) {
								field[x][ty] = field[x][ty - 1];
							}
						}
						for (int x = 0; x < fieldWidth; x++) {
							field[x][0] = Type::None;
						}
					}
				}
				pos = origin;								//當碰到底部時 隨機抽選一個形狀
				currentType = Type(rand() % 2 + 1);
				currentIndex = 0;							//重新開始的時候要把方向跟Type都重新設定
			}
		}
		window.clear();
		//繪製背景場地
		for (int x = 0; x < fieldWidth; x++) {
			for (int y = 0; y < fieldHeight; y++) {
				if (field[x][y] == Type::None) continue; 			//保留落下的格子

				sf::Sprite& s = sprites[field[x][y]];

				s.setPosition(
					float(x * blockWidth),
					float(y * blockHeight));
				window.draw(s);
			}
		}
		// 繪製控制的物體
		for (const sf::Vector2i& d : currentShape) {
			sf::Vector2i np = pos + d;
			currentSprite.setPosition(
				(float)(np.x * blockWidth),
				(float)(np.y * blockHeight));  //繪製黃色格子的位置  單位為像素 必須 * 格子的寬跟高
			window.draw(currentSprite);
		}
		window.display();
	}
	return EXIT_SUCCESS;
}