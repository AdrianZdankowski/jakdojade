#include <iostream>

using namespace std;

// same 1 na 18 bitach
#define MAX_18BIT 262143
#define StringsMapSize MAX_18BIT+1

#define STRING_SIZE 16

struct String {
	char letters[STRING_SIZE] = { 0 }; 

	bool operator == (String& other) {

		for (int i = 0; i < STRING_SIZE; i++) {
			if (letters[i] != other.letters[i]) {
				return false;
			}
		}

		return true;
	}

};

struct StringNode {
	String name;
	unsigned numberCity = 0;
	StringNode* next = nullptr;
};


struct List {
	StringNode stringNode;
	unsigned counter = 0;
	StringNode* tail = nullptr;

	~List() {
		StringNode* node = stringNode.next;

		while (node != nullptr) {
			StringNode* node2 = node;
			delete node2;
			node = node->next;


		}
	}

};

struct Position {
	unsigned x = 0;
	unsigned y = 0;

	Position operator+(Position& other) {
		Position newPosition;
		newPosition.x = x + other.x;
		newPosition.y = y + other.y;
		return newPosition;
	}

};


struct Data {
	unsigned* map = nullptr;
	Position* positions = nullptr;
	String** names = nullptr;
	unsigned x = 0;
	unsigned y = 0;
	unsigned numberOfCities = 0;


	Data(unsigned x, unsigned y, unsigned numberOfCities) {
		this->x = x;
		this->y = y;
		this->numberOfCities = numberOfCities;

		positions = new Position[numberOfCities + 1];  // n+1 poniewaz:  "n" to ostatni indeks miasta, 0 indeks jest nieuzywany, bo mapa domyslnie jest wypelniona zerami
		names = new String * [numberOfCities + 1];
		map = new unsigned[x * y];

		for (unsigned i = 0; i < x * y; i++) {
			map[i] = 0;
		}
	}

	unsigned getIndexOfCity(unsigned i) {
		return map[i];
	}

	void insertData(unsigned x, unsigned y, unsigned xMax, unsigned index, String* name)
	{
		unsigned i = x + y * xMax;
		map[i] = index;
		positions[index].x = x;
		positions[index].y = y;
		names[index] = name;
	}

	~Data() {
		delete[] positions;

		for (unsigned i = 1; i < numberOfCities + 1; i++) {
			delete names[i];
		}
		delete[] names;

		delete[] map;
	}

};


class StringsMap {
public:
	// mozliwe ze lepiej tablice wskaznikow
	StringsMap() {
		strings = new List[StringsMapSize];
	}
	~StringsMap() {
		delete[] strings;
	}
	List* strings;

	unsigned getCityId(String& nameToFind) {
		auto key = calculateIndex(nameToFind);
		List& l = strings[key];
		auto counter = l.counter;
		if (counter == 1) {
			return l.stringNode.numberCity;
		}
		else {
			// trzeba skakac po calej liscie az znajdzie sie taki sam string
			StringNode* current = &l.stringNode;
			return getDataFromList(current, counter, nameToFind);
		}
	}

	void insertCity(unsigned numberCity, String& name) {
		auto key = calculateIndex(name);
		List& l = strings[key];
		auto counter = l.counter;
		if (counter == 0) {
			l.stringNode.name = name;
			l.stringNode.numberCity = numberCity;
			l.tail = &l.stringNode;
			l.counter++;
		}
		else {
			StringNode* current = l.tail;
			StringNode* createdStringNode = new StringNode;
			createdStringNode->name = name;
			createdStringNode->numberCity = numberCity;
			l.counter++;

			current->next = createdStringNode;
			l.tail = createdStringNode;
		}
	}

private:

	unsigned getDataFromList(StringNode* current, unsigned counter, String& nameToFind) {
		for (unsigned i = 0; i < counter; i++) {
			if (current->name == nameToFind) {
				return current->numberCity;
			}
			current = current->next;
		}
		return 0; // nie powinno tutaj wejsc
	}

	unsigned calculateIndex(const String& string) {
		// tylko 3 pierwsze znaki biora udzial w liczeniu klucza
		// odejmuje '0', bo to najnizsza mozliwa wartosc z tablicy ascii
		// 'Z' - '0' = 42 ( miesci sie na 6 bitach )
		// wiec zeby zmiescic wszystkie 3 literki potrzeba 18 bitow.
		unsigned key = (string.letters[0]) - '0';

		if (string.letters[1]) {
			key |= (string.letters[1] - '0') << 6;
		}

		if (string.letters[2]) {
			key |= (string.letters[2] - '0') << 12;
		}

		return key;
	}
};



unsigned calculateCities(char* text, unsigned inputSize) {
	unsigned n = 0;
	for (unsigned i = 0; i < inputSize; i++) {
		if (text[i] == '*') {
			n++;
		}
	}
	return n;
}

bool canSymbolBeName(char symbol) {
	return (symbol >= 'A' && symbol <= 'Z') || (symbol >= '0' && symbol <= '9');
}

String* readString(char* text, unsigned i) {
	String* name = new String;
	unsigned iName = 0;
	unsigned index = i + iName;
	while (canSymbolBeName(text[index])) {

		name->letters[iName] = text[index];
		iName++;
		index = i + iName;
	}
	return name;
}

unsigned getFirstLetterIndex(char* text, unsigned i) {
	unsigned index = i;
	while (canSymbolBeName(text[index - 1])) {
		index--;
	}

	return index;
}

String* getName(unsigned xMax, char* text, unsigned i) {

	// znak na prawo od * mozemy po prostu wczytac, bo bedzie to na pewno pierwsza literka
	// pozostale nazwy miast trzeba znalezc: 1. znalezc poczatek nazwy miasta , wczytac nazwe miasta

	auto rowOffset = xMax;

	// prawo
	auto index = i + 1;
	if (canSymbolBeName(text[index])) {
		return readString(text, index);
	}
	index = i + 1 - rowOffset;
	if (canSymbolBeName(text[index])) {
		index = getFirstLetterIndex(text, index);
		return readString(text, index);
	}
	index = i + 1 + rowOffset;
	if (canSymbolBeName(text[index])) {
		index = getFirstLetterIndex(text, index);
		return readString(text, index);
	}

	// lewo
	index = i - 1;
	if (canSymbolBeName(text[index])) {
		index = getFirstLetterIndex(text, index);
		return readString(text, index);
	}
	index = i - 1 - rowOffset;
	if (canSymbolBeName(text[index])) {
		index = getFirstLetterIndex(text, index);
		return readString(text, index);
	}
	index = i - 1 + rowOffset;
	if (canSymbolBeName(text[index])) {
		index = getFirstLetterIndex(text, index);
		return readString(text, index);
	}

	// srodek
	index = i - rowOffset;
	if (canSymbolBeName(text[index])) {
		index = getFirstLetterIndex(text, index);
		return readString(text, index);
	}

	index = i + rowOffset;
	if (canSymbolBeName(text[index])) {
		index = getFirstLetterIndex(text, index);
		return readString(text, index);
	}

	return nullptr; // nie powinno tutaj wejsc

}

unsigned calculateCities(unsigned inputSize, char* text) {
	unsigned n = 0;
	for (unsigned i = 0; i < inputSize; ++i) {
		if (text[i] == '*') {
			++n;
		}
	}
	return n;
}

unsigned setNames(unsigned inputSize, char* text, unsigned xMax, unsigned yMax, StringsMap& stringsMap, Data& data) {
	unsigned n = 0;
	for (unsigned i = 0; i < inputSize; ++i) {
		if (text[i] == '*') {
			++n;
			String* name = getName(xMax, text, i);
			unsigned y = i / xMax;
			unsigned x = i % xMax;
			data.insertData(x, y, xMax, n, name);
			stringsMap.insertCity(n, *name);
		}
	}
	return n;
}

struct Node {
	unsigned position;
	unsigned distance;
	Node* next = nullptr;
	Node* prev = nullptr;
};

struct Queue {
	Node* head = nullptr;
	Node* tail = nullptr;

	~Queue() {
		Node* node = head;

		while (node != nullptr) {
			Node* prev = node;
			node = node->next;
			delete prev;
		}
	}

	void addToQueue(unsigned position, unsigned distance) {
		if (head == nullptr) {
			head = new Node;
			tail = head;
			head->position = position;
			head->distance = distance;
		}
		else {
			Node* node = new Node;
			node->position = position;
			node->distance = distance;
			node->prev = tail;
			tail->next = node;
			tail = node;
		}
	}
	Node* getFromQueue() {
		if (head == tail) {
			Node* node = head;
			head = nullptr;
			tail = nullptr;
			return node;
		}
		Node* node = head;
		head = head->next;
		head->prev = nullptr;
		return node;
	}
};


struct NumberOfCityNode {
	unsigned numberOfCity = 0;
	unsigned distance = 0;
	NumberOfCityNode* next = nullptr;
};

struct NumberOfCityList {
	NumberOfCityNode* head = nullptr;
	NumberOfCityNode* tail = nullptr;

	~NumberOfCityList() {
		NumberOfCityNode* node = head;

		while (node != nullptr) {
			NumberOfCityNode* prev = node;
			node = node->next;
			delete prev;
		}
	}
};

struct Graf {
	unsigned numberOfCities;
	NumberOfCityList* graf;

	Graf(unsigned numberOfCities) : numberOfCities(numberOfCities) {
		graf = new NumberOfCityList[numberOfCities + 1]; // +1, bo indeksujemy od 1
	}

	~Graf() {
		// loop over and delete all nodes;
		delete[] graf;
	}

	void addToGraf(unsigned source, unsigned destination, unsigned distance) {

		if (graf[source].head == nullptr) {
			NumberOfCityNode* node = new NumberOfCityNode;
			node->distance = distance;
			node->numberOfCity = destination;
			graf[source].head = node;
			graf[source].tail = node;
		}
		else {

			NumberOfCityNode* node = graf[source].head;
			while (node != nullptr && node->numberOfCity != destination) {
				node = node->next;
			}

			if (node == nullptr) {
				node = new NumberOfCityNode;
				graf[source].tail->next = node;
				graf[source].tail = node;
			}

			if (node->distance == 0) { // brakuje polaczenia
				node->distance = distance;
				node->numberOfCity = destination;
			}
			else if (distance < node->distance) { // aktualizuje tylko jesli jest krotsza droga
				node->distance = distance;
				node->numberOfCity = destination;
			}

		}
	}

	NumberOfCityNode* getConnections(unsigned number) {
		return graf[number].head;
	}
};


void breadthFirstSearch(Data& data, unsigned numberCityStart, unsigned inputSize, char* input, unsigned xMax, Graf& graf) {

	bool* visited = new bool[inputSize];
	for (unsigned i = 0; i < inputSize; i++) {
		visited[i] = false;
	}

	auto positionStart = data.positions[numberCityStart];
	unsigned position = positionStart.x + positionStart.y * xMax;
	visited[position] = true;
	Queue queue;
	queue.addToQueue(position, 0);


	while (queue.head != nullptr) {
		Node* node = queue.getFromQueue();
		position = node->position;
		unsigned distance = node->distance;
		delete node;

		static int changeDirections[4] = {
			1,      // right
			-1,     // left
			xMax,   // down
			-(int)xMax   // up
		};

		for (unsigned int i = 0; i < 4; i++) {
			unsigned positionToVisit = position + changeDirections[i];
			if (visited[positionToVisit] == false) {
				visited[positionToVisit] = true;

				if (input[positionToVisit] == '#') {
					queue.addToQueue(positionToVisit, distance + 1);
				}
				else if (data.map[positionToVisit] > 0) { //  przechowuje indexy miast zaczynajac liczenie od 1
					unsigned numberCity = data.getIndexOfCity(positionToVisit);
					graf.addToGraf(numberCityStart, numberCity, distance + 1);

				}
			}
		}
	}
	delete[] visited;
}

void readFligtName(String& source, String& destination, unsigned& distance) {
	cin >> source.letters;
	cin >> destination.letters;
	cin >> distance;
}

void readDijkstraQuery(String& source, String& destination, bool& isPath) {
	cin >> source.letters;
	cin >> destination.letters;
	cin >> isPath;
}


struct PriorityNode {
	unsigned distance;
	unsigned numberCity;
};

struct PriorityQueue {
	unsigned size = 0;
	unsigned capacity = 0;


	~PriorityQueue() {
		delete[] originalQueue;
	}

	PriorityNode* queue = nullptr;
	PriorityNode* originalQueue = nullptr;

	PriorityQueue(unsigned n) : capacity(n) {
		queue = new PriorityNode[n];
		originalQueue = queue;
	}

	void sortHeap(unsigned index) {

		if (index == 0) {
			return;
		}

		auto upIndex = (index - 1) / 2;

		// jesli na gorze jest wiekszy, to trzeba zmieniac, az znajdzie sie niewiekszy
		// lub dojdzie sie na sama gore czyli index=0
		while (queue[index].distance < queue[upIndex].distance)
		{

			unsigned numberCity = queue[index].numberCity;
			unsigned distance = queue[index].distance;
			queue[index].numberCity = queue[upIndex].numberCity;
			queue[index].distance = queue[upIndex].distance;
			queue[upIndex].numberCity = numberCity;
			queue[upIndex].distance = distance;

			index = upIndex;
			upIndex = (index - 1) / 2;

			if (index == 0) {
				break;
			}

		}

	}

	void addPriorityNode(unsigned numberCity, unsigned distance) {


		auto difference = queue - originalQueue;

		if (difference + 1 >= capacity) {
			capacity = capacity * 2;
			PriorityNode* queuetmp = new PriorityNode[capacity];

			for (unsigned i = 0; i < size; i++) {
				queuetmp[i].distance = originalQueue[i].distance;
				queuetmp[i].numberCity = originalQueue[i].numberCity;
			}

			delete[] originalQueue;
			queue = queuetmp;
			originalQueue = queue;
		}

		auto index = size;
		size++;
		queue[index].distance = distance;
		queue[index].numberCity = numberCity;

		sortHeap(index);
	}

	unsigned getPriorityNode() {
		if (size == 0) {
			return 0;
		}

		auto numberCity = queue[0].numberCity;
		size--;
		queue++;
		return numberCity;
	}
};


void initializeDistances(unsigned* distances, unsigned n) {

	const unsigned maxDistance = 0xffffffff;

	for (unsigned i = 0; i < n; i++) {
		distances[i] = maxDistance;
	}
}

void initalizeVisited(bool* visited, unsigned n) {
	for (unsigned i = 0; i < n; i++) {
		visited[i] = false;
	}
}

void initializePrevNumbers(unsigned* prevNumbers, unsigned n) {
	for (unsigned i = 0; i < n; i++) {
		prevNumbers[i] = 0;
	}
}



unsigned* dijkstrasAlgorithmWithPath(Graf& graf, unsigned sourceNumber, unsigned destinationNumber, unsigned nCities, unsigned& distanceFound, unsigned& pathSize) {

	const unsigned priorityQueueCapacity = 25600;
	PriorityQueue priorityQueue(priorityQueueCapacity);

	bool* visited = new bool[nCities + 1]; // bo indeksujemy od 1
	initalizeVisited(visited, nCities + 1);

	unsigned* distances = new unsigned[nCities + 1];
	initializeDistances(distances, nCities + 1);

	unsigned* prevNumbers = new unsigned[nCities + 1];
	initializePrevNumbers(prevNumbers, nCities + 1);

	auto current = sourceNumber;
	distances[current] = 0; // dla pierwszego trzeba dac 0


	while (current != destinationNumber) {

		visited[current] = true;

		auto connection = graf.getConnections(current);

		while (connection) {
			auto numberCity = connection->numberOfCity;

			if (visited[numberCity] == false) {
				if (distances[numberCity] > connection->distance + distances[current]) {
					distances[numberCity] = connection->distance + distances[current];
					prevNumbers[numberCity] = current;
					priorityQueue.addPriorityNode(numberCity, distances[numberCity]);
				}
			}
			connection = connection->next;
		}
		current = priorityQueue.getPriorityNode();

		if (current == 0) {
			break;
		}
	}

	auto distance = distances[destinationNumber];
	distanceFound = distance;

	// nie ma sciezki jesli bylo do siebie
	if (sourceNumber == destinationNumber) {
		delete[] distances;
		delete[] visited;
		delete[] prevNumbers;
		return nullptr;
	}

	unsigned pathIndex = prevNumbers[destinationNumber];

	// nie ma sciezki jesli nie bylo miast pomiedzy
	if (sourceNumber == pathIndex) {
		delete[] distances;
		delete[] visited;
		delete[] prevNumbers;
		return nullptr;
	}

	unsigned i = 0;
	unsigned* path = new unsigned[distance];

	while (true) {
		path[i] = pathIndex;
		pathIndex = prevNumbers[pathIndex];
		i++;
		if (pathIndex == sourceNumber) {
			break;
		}
	}

	pathSize = i;


	delete[] distances;
	delete[] visited;
	delete[] prevNumbers;

	return path;
}

unsigned dijkstrasAlgorithm(Graf& graf, unsigned sourceNumber, unsigned destinationNumber, unsigned nCities) {

	const unsigned priorityQueueCapacity = 25600;
	PriorityQueue priorityQueue(priorityQueueCapacity);

	bool* visited = new bool[nCities + 1]; // bo indeksujemy od 1
	initalizeVisited(visited, nCities + 1);

	unsigned* distances = new unsigned[nCities + 1];
	initializeDistances(distances, nCities + 1);

	auto current = sourceNumber;
	distances[current] = 0; // dla pierwszego trzeba dac 0

	while (current != destinationNumber) {

		visited[current] = true;
		auto connection = graf.getConnections(current);

		while (connection) {
			auto numberCity = connection->numberOfCity;

			if (visited[numberCity] == false) {
				if (distances[numberCity] > connection->distance + distances[current]) {
					distances[numberCity] = connection->distance + distances[current];

					priorityQueue.addPriorityNode(numberCity, distances[numberCity]);
				}
			}
			connection = connection->next;
		}
		current = priorityQueue.getPriorityNode();
	}

	auto distance = distances[destinationNumber];
	delete[] distances;
	delete[] visited;

	return distance;
}

bool hasRoad(Data& data, unsigned numberCityStart, char* input, unsigned xMax) {
	auto positionStart = data.positions[numberCityStart];
	unsigned position = positionStart.x + positionStart.y * xMax;

	static int changeDirections[4] = {
	1,      // right
	-1,     // left
	xMax,   // down
	-(int)xMax   // up
	};

	for (unsigned i = 0; i < 4; i++) {
		unsigned positionToVisit = position + changeDirections[i];
		if (input[positionToVisit] == '#' || input[positionToVisit] == '*') {
			return true;
		}


	}
	return false;
}


int main()
{

	char* text = nullptr;
	unsigned xMax = 0;
	unsigned yMax = 0;

	cin >> xMax;
	cin >> yMax;

	xMax = xMax + 2;
	yMax = yMax + 2;

	cin.get();

	auto inputSize = xMax * yMax;
	text = new char[inputSize];

	for (unsigned i = 0; i < inputSize; i++) {
		text[i] = 0;
	}

	// przesuniecie o 1 kratke w prawo i jeden wiersz w dol
	unsigned offsetInput = xMax + 1;

	for (unsigned y = 0; y < yMax - 2; y++) {
		unsigned offset = y * xMax + offsetInput;
		cin.get(text + offset, inputSize);
		cin.get();
	}


	auto numberOfCities = calculateCities(text, inputSize);

	StringsMap citiesIds;
	Data* data = new Data(xMax, yMax, numberOfCities);
	setNames(inputSize, text, xMax, yMax, citiesIds, *data);

	Graf graf(numberOfCities);
	for (unsigned i = 0; i < numberOfCities; i++) {
		if (hasRoad(*data, i + 1, text, xMax)) {
			breadthFirstSearch(*data, i + 1, inputSize, text, xMax, graf);
		}

	}

	unsigned flights = 0;
	cin >> flights;
	for (unsigned i = 0; i < flights; i++) {
		String source;
		String destination;
		unsigned distance;
		readFligtName(source, destination, distance);

		auto sourceNumber = citiesIds.getCityId(source);
		auto destinationNumber = citiesIds.getCityId(destination);
		graf.addToGraf(sourceNumber, destinationNumber, distance);

	}

	delete[] text;



	unsigned queries = 0;
	cin >> queries;

	for (unsigned i = 0; i < queries; i++) {
		String source;
		String destination;
		bool isPath;
		readDijkstraQuery(source, destination, isPath);


		auto sourceNumber = citiesIds.getCityId(source);
		auto destinationNumber = citiesIds.getCityId(destination);

		if (isPath) {
			unsigned distance = 0;
			unsigned pathSize = 0;
			unsigned* path = dijkstrasAlgorithmWithPath(graf, sourceNumber, destinationNumber, numberOfCities, distance, pathSize);
			cout << distance;
			for (unsigned int i = 0; i < pathSize; i++) {
				unsigned index = pathSize - i - 1;
				unsigned numberCity = path[index];
				cout << " " << data->names[numberCity]->letters;
			}
			cout << endl;
			delete[] path;

		}
		else {
			auto result = dijkstrasAlgorithm(graf, sourceNumber, destinationNumber, numberOfCities);
			cout << result << endl;
		}
	}


	delete data;

	return 0;
}


