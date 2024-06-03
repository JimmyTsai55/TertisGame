#include <SFML/Graphics.hpp>
#include <vector>
int main() {
	const int fieldWidth = 10;    //����@�Ӯ�l���פμe��
	const int fieldHeight = 20;

	enum class Type {
		None,
		O,
		I,
	};

	Type field[fieldWidth][fieldHeight] = {};			//�����l�O�_�d�b��a���}��

	sf::Texture yellowTexture;   //�}�Ҷ������Ϥ�
	if (!yellowTexture.loadFromFile("yellow.png")) {
		return EXIT_FAILURE;	//��S���}�Ҫ��ɭԵ����{��	
	}
	sf::Texture lightBlueTextture;
	if (!lightBlueTextture.loadFromFile("light_blue.png")) {
		return EXIT_FAILURE;
	}
	std::map<Type, std::vector<std::vector<sf::Vector2i>>> shapes = {   //�ϥ�vector���x�s(�~�i�H�ƻs��currentShape)
		//shape_Os   ø�s2*2�������� �Q�ήy�Шt�Ψӧ��� 	
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
		//shape_Is    //ø�s1*4�������� �Q�ήy�Шt�Ψӧ���
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

	Type currentType = Type(rand() % 2 + 1);   //(�ϥξ�ƨӪ��ENUM���Ϊk)RAND%2 ��0 �]�w�� O�� �A��1�]�w�� I��
	int currentIndex = 0;						//�Ϥ�������� �Ҧ����������  0 ���o�Ӥ�V

	const int blockWidth = yellowTexture.getSize().x;   //�����l���e�פΰ���
	const int blockHeight = yellowTexture.getSize().y;

	const int windowWidth = fieldWidth * blockWidth;    //�������e�פΰ���
	const int windowHeight = fieldHeight * blockHeight;

	std::map<Type, sf::Sprite> sprites = {					//�ϥ�vector���x�s(�~�i�H�ƻs��currentSprite)
		{Type::O, sf::Sprite(yellowTexture)},
		{Type::I, sf::Sprite(lightBlueTextture)},
	};

	std::vector<sf::Vector2i> currentShape;   //��w�{�b���U�����(�ϥ�rand���H�����@�ӧΪ�)
	sf::Sprite currentSprite;

	const sf::Vector2i origin(fieldWidth / 2, 0);		//�@�}�l�|�b���a������  (���a���e / 2, ���׬� 0)	
	sf::Vector2i pos(origin);							//�^����l�ثe��m

	sf::RenderWindow window(sf::VideoMode(windowWidth, windowHeight), L"�Xù�����");
	sf::Clock clock;
	while (window.isOpen()) {
		currentShape  = shapes[currentType][currentIndex];				//��w����Ϊ�  O...I...  
		currentSprite = sprites[currentType];				//ø�s����C��

		enum class Action {      //�ץ󪺤���
			Hold,
			MoveDown,
			MoveLeft,
			MoveRight,
			Rotate,
		};
		Action action = Action::Hold;   //�@�}�ldefault��Hold

		sf::Event evt;				//�}�ҵ���
		if (window.pollEvent(evt)) {
			if (evt.type == sf::Event::Closed) {             //�]�w�������}��
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

		if (clock.getElapsedTime().asSeconds() >= 0.3f) {    //��Clock�ӳ]�w�C0.3��U���@��  �̫�nrestart�ɶ�
			action = Action::MoveDown;						//default�@�}�l�N�@�����U	
			clock.restart();
		}

		sf::Vector2i nextPos(pos);
		int nextIndex = currentIndex;
		if (action == Action::Rotate) {
			nextIndex = (nextIndex + 1) % shapes[currentType].size();  //�ϥ�%�D��ƨӦ��X�ر��઺��V���W��
		}
		std::vector<sf::Vector2i> nextShape = shapes[currentType][nextIndex];

		switch (action) {									//��switch�ӳ]�w�W�U���k
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
			sf::Vector2i np = nextPos + d;					// 4�Ӯ�l���첾
			if (np.x >= 0 && np.x < fieldWidth &&			//�]�w4�Ӯ�l���U�@�Ӧ�m����W�X��l�������e��
				np.y < fieldHeight &&						//���k���䤣�i�W�L �����I��O�d 
				(np.y < 0 || field[np.x][np.y] == Type::None)) { //���F�ˬd����W�V��l�������e�� �]�n�ݸӦ�m�O�_�w�g����l													 
				countEmpty++;
			}
		}

		if (countEmpty == 4) {
			pos = nextPos;					//��s�ܰʹL�᪺�Ѽ�
			currentIndex = nextIndex;
			currentShape = nextShape;
		}
		else {
			if (action == Action::MoveDown) {
				for (const sf::Vector2i& d : currentShape) {
					sf::Vector2i np = pos + d;
					if (np.y >= 0) {
						field[np.x][np.y] = currentType;
					}										//��I�줣��b���U�ɭ� �O�o�o�Ӯ�l 
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
				pos = origin;								//��I�쩳���� �H�����@�ӧΪ�
				currentType = Type(rand() % 2 + 1);
				currentIndex = 0;							//���s�}�l���ɭԭn���V��Type�����s�]�w
			}
		}
		window.clear();
		//ø�s�I�����a
		for (int x = 0; x < fieldWidth; x++) {
			for (int y = 0; y < fieldHeight; y++) {
				if (field[x][y] == Type::None) continue; 			//�O�d���U����l

				sf::Sprite& s = sprites[field[x][y]];

				s.setPosition(
					float(x * blockWidth),
					float(y * blockHeight));
				window.draw(s);
			}
		}
		// ø�s�������
		for (const sf::Vector2i& d : currentShape) {
			sf::Vector2i np = pos + d;
			currentSprite.setPosition(
				(float)(np.x * blockWidth),
				(float)(np.y * blockHeight));  //ø�s�����l����m  ��쬰���� ���� * ��l���e��
			window.draw(currentSprite);
		}
		window.display();
	}
	return EXIT_SUCCESS;
}